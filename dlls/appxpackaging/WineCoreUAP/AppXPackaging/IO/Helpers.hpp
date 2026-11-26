/* 
 * AppXPackage Reader and Writer interfaces.
 *
 * Written by Weather.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_APPX_HELPERS_IO_H
#define __WINE_APPX_HELPERS_IO_H

#include <atomic>
#include <type_traits>

#include "../../../private.h"

#include "wincrypt.h"

struct AppxBlockMapHash
{
public:
    UINT32 bufferSize;
    BYTE *buffer;
};

template<typename T>
HRESULT
ObtainAttributeFromXMLElement( IXMLDOMElement *elem, const OLECHAR *attribName, T *value )
{
    BSTR attributeName;
    DWORD hashSize = 0;
    LPWSTR tmpVal;
    HRESULT hr = S_OK;
    VARIANT val;

    attributeName = SysAllocString( attribName );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = elem->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
        hr = E_INVALIDARG;

    if ( SUCCEEDED( hr ) )
    {
        if constexpr ( std::is_same_v<T, LPWSTR> )
        {
            hr = VariantChangeType( &val, &val, 0, VT_BSTR );
            if ( SUCCEEDED( hr ) )
            {
                *value = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( V_BSTR( &val ) ) + sizeof(WCHAR) );
                if ( *value )
                    lstrcpynW( *value, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
                else
                    hr = E_OUTOFMEMORY;
            }
        } else if constexpr ( std::is_same_v<T, UINT32> )
        {
            // UINT64 should not be used for versioning. Use LPWSTR instead.
            hr = VariantChangeType( &val, &val, 0, VT_UI4 );
            if ( SUCCEEDED( hr ) )
                *value = V_UI4( &val );
        } else if constexpr ( std::is_same_v<T, UINT64> )
        {
            // UINT64 should not be used for versioning. Use LPWSTR instead.
            hr = VariantChangeType( &val, &val, 0, VT_UI8 );
            if ( SUCCEEDED( hr ) )
                *value = V_UI8( &val );
        } else if constexpr ( std::is_same_v<T, BOOL> )
        {
            hr = VariantChangeType( &val, &val, 0, VT_BSTR );
            if ( SUCCEEDED( hr ) )
            {
                tmpVal = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( V_BSTR( &val ) ) + sizeof(WCHAR) );
                if ( tmpVal )
                    lstrcpynW( tmpVal, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
                else
                    hr = E_OUTOFMEMORY;

                if ( lstrcmpiW( (LPCWSTR)L"true", tmpVal ) == 0 )
                    *value = TRUE;
                else
                    *value = FALSE;
            }
        } else if constexpr ( std::is_same_v<T, AppxBlockMapHash> )
        {
            hr = VariantChangeType( &val, &val, 0, VT_BSTR );
            if ( SUCCEEDED( hr ) )
            {
                if ( !CryptStringToBinaryW( V_BSTR( &val ), 0, CRYPT_STRING_BASE64, NULL, &hashSize, NULL, NULL ) )
                    hr = HRESULT_FROM_WIN32( GetLastError() );

                if ( SUCCEEDED( hr ) )
                {
                    value->buffer = static_cast<BYTE *>( CoTaskMemAlloc( hashSize ) );
                    if ( !value->buffer )
                    {
                        hr = E_OUTOFMEMORY;
                    } else if ( !CryptStringToBinaryW( V_BSTR( &val ), 0, CRYPT_STRING_BASE64, value->buffer, &hashSize, NULL, NULL ) )
                    {
                        CoTaskMemFree( value->buffer );
                        hr = HRESULT_FROM_WIN32( GetLastError() );
                    }
                    value->bufferSize = hashSize;
                }
            }
        } else if constexpr ( std::is_same_v<T, IUri*> )
        {
            hr = VariantChangeType( &val, &val, 0, VT_BSTR );
            if ( SUCCEEDED( hr ) )
            {
                hr = CreateUri( V_BSTR( &val ), Uri_CREATE_CANONICALIZE, 0, value );
            }
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

template <typename Interface, typename CurrentInterface, typename Object>
class AppxEnumerator 
    : public Interface
{
public:
    AppxEnumerator(const AppxEnumerator&) = delete;
    AppxEnumerator& operator=(const AppxEnumerator&) = delete;

    explicit AppxEnumerator( IXMLDOMNodeList *nodes )
        : nodes(nodes)
    {
        nodes->AddRef();
    }

    virtual ~AppxEnumerator() = default;

    HRESULT WINAPI 
    QueryInterface( REFIID iid, void **out ) noexcept override
    {
        if ( !out )
            return E_POINTER;

        *out = nullptr;

        if ( iid == __uuidof( IUnknown ) ||
             iid == __uuidof( Interface ) )
        {
            AddRef();
            *out = static_cast<Interface *>(this);
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG WINAPI 
    AddRef() noexcept override
    {
        ULONG curr = static_cast<ULONG>(++ref);
        return curr;
    }

    ULONG WINAPI 
    Release() noexcept override
    {
        ULONG curr = static_cast<ULONG>(--ref);
        if ( !curr )
        {
            nodes->Release();
            delete this;
        }
        return curr;
    }

    HRESULT WINAPI
    GetCurrent( CurrentInterface **object ) noexcept override
    {
        LONG len;
        HRESULT hr;
        IXMLDOMNode *node;
        IXMLDOMElement *elem;

        if ( !object )
            return E_POINTER;

        *object = nullptr;

        if ( FAILED( hr = nodes->get_length( &len ) ) )
            return hr;

        if ( iterator >= len )
            return E_BOUNDS;

        if ( FAILED( hr = nodes->get_item( iterator, &node ) ) )
            return hr;

        hr = node->QueryInterface( &elem );
        node->Release();

        if ( FAILED( hr ) )
            return hr;

        *object = new(std::nothrow) Object( elem );
        elem->Release();

        return *object ? S_OK : E_OUTOFMEMORY;
    }

    HRESULT WINAPI 
    GetHasCurrent( BOOL *hasCurrent ) noexcept override
    {
        LONG len;
        HRESULT hr;

        if ( !hasCurrent )
            return E_POINTER;

        if ( FAILED( hr = nodes->get_length( &len ) ) )
            return hr;

        *hasCurrent = iterator < len;
        return S_OK;
    }

    HRESULT WINAPI MoveNext(BOOL *hasNext) noexcept override
    {
        LONG len;
        HRESULT hr;

        if ( !hasNext )
            return E_POINTER;

        if ( FAILED( hr = nodes->get_length( &len ) ) )
            return hr;

        if ( iterator < len )
            iterator++;

        *hasNext = iterator < len;
        return S_OK;
    }

protected:
    IXMLDOMNodeList *nodes;
    std::atomic<LONG> ref = 1;
    LONG iterator = 0;
};

#endif