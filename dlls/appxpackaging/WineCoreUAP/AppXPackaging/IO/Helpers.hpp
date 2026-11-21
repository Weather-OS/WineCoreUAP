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

template<typename T>
HRESULT
ObtainAttributeFromXMLElement( IXMLDOMElement *elem, OLECHAR* attribName, T *value )
{
    BSTR attributeName;
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
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

#endif