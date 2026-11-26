/* 
 * AppX Block Map Implementation
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

#include <atomic>
#include <new>

#include "Manifest.hpp"
#include "Helpers.hpp"
#include "bcrypt.h"
#include "shlwapi.h"
#include "corecrt_wstdio.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

using AppxManifestPackageDependenciesEnumerator = AppxEnumerator<IAppxManifestPackageDependenciesEnumerator, IAppxManifestPackageDependency, AppxManifestPackageDependency>;

// dirty implementation of https://withinrafael.com/2018/01/28/whats-that-gobbledygook-in-my-package-family-name
static BOOL PublisherIdFromPublisherString( LPCWSTR str, LPWSTR result, SIZE_T result_cch )
{
    static LPCWSTR alphabet = (LPCWSTR)L"0123456789abcdefghjkmnpqrstvwxyz";
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_HASH_HANDLE hHash = NULL;
    BYTE hash[32];
    BYTE group;
    BYTE bit;
    NTSTATUS status;
    DWORD hashObjectSize = 0;
    DWORD cbData = 0;
    BYTE *hashObject = NULL;
    SIZE_T inputBytes;
    SIZE_T iterator;
    SIZE_T bitPos;
    INT iterator2;

    if ( !str || !result || result_cch < 14 )
        return FALSE;

    inputBytes = lstrlenW(str) * sizeof(WCHAR);
    status = BCryptOpenAlgorithmProvider( &hAlg, BCRYPT_SHA256_ALGORITHM, NULL, 0 );
    if ( FAILED( status ) )
        goto _CLEANUP;

    status = BCryptGetProperty( hAlg, BCRYPT_OBJECT_LENGTH, (PUCHAR)&hashObjectSize, sizeof(hashObjectSize), &cbData, 0 );
    if ( FAILED( status ) )
        goto _CLEANUP;

    hashObject = (BYTE *)HeapAlloc( GetProcessHeap(), 0, hashObjectSize );
    if (!hashObject)
        goto _CLEANUP;
        
    status = BCryptCreateHash( hAlg, &hHash, hashObject, hashObjectSize, NULL, 0, 0 );
    if ( FAILED( status ) )
        goto _CLEANUP;

    status = BCryptHashData( hHash, (PUCHAR)str, (ULONG)inputBytes, 0 );
    if ( FAILED( status ) )
        goto _CLEANUP;

    status = BCryptFinishHash( hHash, hash, sizeof(hash), 0 );

    if (status < 0)
        goto _CLEANUP;

    //encode the first 64 hash bits into 13 base32 characters.
    for ( iterator = 0; iterator < 13; iterator++ )
    {
        group = 0;
        for ( iterator2 = 0; iterator2 < 5; iterator2++ )
        {
            bitPos = iterator * 5 + iterator2;
            bit = 0;
            if ( bitPos < 64 )
            {
                bit = (hash[bitPos / 8] >> (7 - (bitPos % 8))) & 1;
            }
            group = (BYTE)((group << 1) | bit);
        }
        result[iterator] = alphabet[group];
    }

    result[13] = L'\0';
    HeapFree( GetProcessHeap(), 0, hashObject );
    BCryptDestroyHash( hHash );
    BCryptCloseAlgorithmProvider( hAlg, 0 );
    return TRUE;

_CLEANUP:
    if (hashObject)
        HeapFree( GetProcessHeap(), 0, hashObject );
    if (hHash)
        BCryptDestroyHash( hHash );
    if (hAlg)
        BCryptCloseAlgorithmProvider( hAlg, 0 );
    return FALSE;
}


static HRESULT ParseVersion( LPCWSTR version, UINT64 *packageVersion ) noexcept
{
    WCHAR extra;
    USHORT versioning[4];

    if ( !version || !packageVersion )
        return E_INVALIDARG;

    if ( swscanf_s( reinterpret_cast<const wchar_t *>(version), L"%hu.%hu.%hu.%hu%c",
                    &versioning[0], &versioning[1],
                    &versioning[2], &versioning[3], &extra ) != 4 )
        return E_INVALIDARG;

    *packageVersion = ((UINT64)versioning[0] << 48) |
                      ((UINT64)versioning[1] << 32) |
                      ((UINT64)versioning[2] << 16) |
                      (UINT64)versioning[3];

    return S_OK;
}

// ------------- IAppxManifestPackageDependency ------------- //
AppxManifestPackageDependency::AppxManifestPackageDependency( IXMLDOMElement *element )
    : xmlElement( element )
{
    element->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxManifestPackageDependency::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxManifestPackageDependency ) )
    {
        AddRef();
        *out = static_cast<IAppxManifestPackageDependency *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxManifestPackageDependency::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxManifestPackageDependency::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( xmlElement ) xmlElement->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxManifestPackageDependency Methods */
HRESULT WINAPI 
AppxManifestPackageDependency::GetName( LPWSTR *name ) noexcept
{
    TRACE( "iface %p, name %p.\n", this, name );

    if ( !name )
        return E_POINTER;

    return ObtainAttributeFromXMLElement( xmlElement, (const OLECHAR *)L"Name", name );
}

HRESULT WINAPI 
AppxManifestPackageDependency::GetPublisher( LPWSTR *publisher ) noexcept
{
    TRACE( "iface %p, publisher %p.\n", this, publisher );

    if ( !publisher )
        return E_POINTER;

    return ObtainAttributeFromXMLElement( xmlElement, (const OLECHAR *)L"Publisher", publisher );
}

HRESULT WINAPI 
AppxManifestPackageDependency::GetMinVersion( UINT64 *minVersion ) noexcept
{
    LPWSTR version = nullptr;
    HRESULT hr = S_OK;

    TRACE( "iface %p, minVersion %p.\n", this, minVersion );

    if ( !minVersion )
        return E_POINTER;

    hr = ObtainAttributeFromXMLElement( xmlElement, (const OLECHAR *)L"MinVersion", &version );
    if ( FAILED( hr ) )
        return hr;

    ParseVersion( version, minVersion );
    CoTaskMemFree( version );

    return hr;
}

/* IAppxManifestPackageDependency2 Methods */
HRESULT WINAPI 
AppxManifestPackageDependency::GetMaxMajorVersionTested( UINT16 *maxMajorVersionTested ) noexcept
{
    INT versionLong;
    LPWSTR version = nullptr;
    HRESULT hr = S_OK;

    TRACE( "iface %p, maxMajorVersionTested %p.\n", this, maxMajorVersionTested );

    if ( !maxMajorVersionTested )
        return E_POINTER;

    hr = ObtainAttributeFromXMLElement( xmlElement, (const OLECHAR *)L"MaxMajorVersionTested", &version );
    if ( FAILED( hr ) )
        return hr;

    versionLong = StrToIntW( version );
    CoTaskMemFree( version );

    if ( versionLong > SHRT_MAX )
        return E_BOUNDS;

    *maxMajorVersionTested = static_cast<UINT16>(versionLong);

    return hr;
}

/* IAppxManifestPackageDependency3 Methods */
HRESULT WINAPI 
AppxManifestPackageDependency::GetIsOptional( BOOL *isOptional ) noexcept
{
    TRACE( "iface %p, isOptional %p.\n", this, isOptional );

    if ( !isOptional )
        return E_POINTER;

    return ObtainAttributeFromXMLElement( xmlElement, (const OLECHAR *)L"uap6:Optional", isOptional );
}

// ------------- IAppxManifestPackageId ------------- //
AppxManifestProperties::AppxManifestProperties( IXMLDOMElement *element )
    : xmlElement( element )
{
    element->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxManifestProperties::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxManifestProperties ) )
    {
        AddRef();
        *out = static_cast<IAppxManifestProperties *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxManifestProperties::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxManifestProperties::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( xmlElement ) xmlElement->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxManifestProperties Methods */
HRESULT WINAPI 
AppxManifestProperties::GetBoolValue( LPCWSTR name, BOOL *value ) noexcept
{
    BSTR nameStr;
    BSTR val;
    HRESULT hr = S_OK;
    IXMLDOMNode *elemNode = nullptr;
    IXMLDOMNode *childNode = nullptr;

    TRACE( "iface %p, name %s, value %p.\n", this, debugstr_w( name ), value );

    if ( !name )
        return E_INVALIDARG;

    if ( !value )
        return E_POINTER;
    
    nameStr = SysAllocString( name );
    if ( !nameStr )
        return E_OUTOFMEMORY;

    if ( FAILED( hr = xmlElement->QueryInterface( &elemNode ) ) )
    {
        SysFreeString( nameStr );
        return hr;
    }

    hr = elemNode->selectSingleNode( nameStr, &childNode );
    SysFreeString( nameStr );
    elemNode->Release();
    if ( FAILED( hr ) ) 
        return hr;

    if ( !childNode )
        return E_INVALIDARG;

    hr = childNode->get_text( &val );
    childNode->Release();
    if ( FAILED( hr ) ) 
        return hr;

    *value = lstrcmpW( val, (LPCWSTR)L"true" ) == 0;
    SysFreeString( val );

    return hr;
}

HRESULT WINAPI 
AppxManifestProperties::GetStringValue( LPCWSTR name, LPWSTR *value ) noexcept
{
    BSTR xpathStr;
    BSTR nameStr;
    BSTR val;
    WCHAR xpath[512];
    HRESULT hr = S_OK;
    IXMLDOMNode *elemNode = nullptr;
    IXMLDOMNode *childNode = nullptr;

    TRACE( "iface %p, name %s, value %p.\n", this, debugstr_w( name ), value );

    if ( !name )
        return E_INVALIDARG;

    if ( !value )
        return E_POINTER;
    
    nameStr = SysAllocString( name );
    if ( !nameStr )
        return E_OUTOFMEMORY;

    if ( FAILED( hr = xmlElement->QueryInterface( &elemNode ) ) )
    {
        SysFreeString( nameStr );
        return hr;
    }

    // node path is defined per namespace
    swprintf_s( reinterpret_cast<wchar_t *>(xpath), ARRAYSIZE( xpath ),
                L"*[local-name()='%s' and namespace-uri()='%s']",
                name, GetResourceW( IDS_XMLNS_MANIFEST_URI ) );

    xpathStr = SysAllocString( xpath );
    if ( !xpathStr )
    {
        elemNode->Release();
        return E_OUTOFMEMORY;
    }

    hr = elemNode->selectSingleNode( xpathStr, &childNode );
    SysFreeString( xpathStr );
    elemNode->Release();
    if ( FAILED( hr ) ) 
        return hr;

    if ( !childNode )
        return E_INVALIDARG;

    hr = childNode->get_text( &val );
    childNode->Release();
    if ( FAILED( hr ) ) 
        return hr;

    // API calls for CoTaskMemAlloc
    *value = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( val ) + sizeof(WCHAR) );
    if ( !*value )
    {
        SysFreeString( val );
        return E_OUTOFMEMORY;
    }
    lstrcpynW( *value, val, SysStringLen( val ) + 1 );
    SysFreeString( val );

    return hr;
}

// ------------- IAppxManifestPackageId ------------- //
AppxManifestPackageId::AppxManifestPackageId( IXMLDOMElement *element )
    : xmlElement( element )
{
    element->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxManifestPackageId::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxManifestPackageId ) )
    {
        AddRef();
        *out = static_cast<IAppxManifestPackageId *>(this);
        return S_OK;
    }
    else if ( iid == __uuidof( IAppxManifestPackageId2 ) )
    {
        AddRef();
        *out = static_cast<IAppxManifestPackageId2 *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxManifestPackageId::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxManifestPackageId::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( xmlElement ) xmlElement->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxManifestPackageId Methods */
HRESULT WINAPI
AppxManifestPackageId::GetName( LPWSTR *name ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, name %p.\n", this, name );

    if ( !name )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"Name" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for manifest package id %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_MANIFEST;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            *name = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( V_BSTR( &val ) ) + sizeof(WCHAR) );
            if ( *name )
                lstrcpynW( *name, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
            else
                hr = E_OUTOFMEMORY;
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI
AppxManifestPackageId::GetArchitecture( APPX_PACKAGE_ARCHITECTURE *architecture ) noexcept
{
    TRACE( "iface %p, architecture %p.\n", this, architecture );

    return GetArchitecture2( reinterpret_cast<APPX_PACKAGE_ARCHITECTURE2 *>(architecture) );
}

HRESULT WINAPI
AppxManifestPackageId::GetPublisher( LPWSTR *publisher ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, publisher %p.\n", this, publisher );

    if ( !publisher )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"Publisher" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for manifest package id %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_MANIFEST;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            *publisher = (LPWSTR)CoTaskMemAlloc( 14 * sizeof(WCHAR) );
            if ( *publisher )
                lstrcpynW( *publisher, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
            else
                hr = E_OUTOFMEMORY;
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI
AppxManifestPackageId::GetVersion( UINT64 *packageVersion ) noexcept
{
    BSTR attributeName;
    LPWSTR version = nullptr;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, packageVersion %p.\n", this, packageVersion );

    if ( !packageVersion )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"Version" );
    if ( !attributeName )
        return E_OUTOFMEMORY;

    VariantInit( &val );

    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for manifest package id %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_MANIFEST;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            version = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( V_BSTR( &val ) ) + sizeof(WCHAR) );
            if ( version )
                lstrcpynW( version, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
            else
                hr = E_OUTOFMEMORY;
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    if ( FAILED( hr ) )
    {
        CoTaskMemFree( version );
        return hr;
    }

    hr = ParseVersion( version, packageVersion );
    CoTaskMemFree( version );

    return hr;
}

HRESULT WINAPI
AppxManifestPackageId::GetResourceId( LPWSTR *resourceId ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, resourceId %p.\n", this, resourceId );

    if ( !resourceId )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"ResourceId" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        WARN( "for manifest package id %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        // element is optional
        hr = S_FALSE;
    }

    if ( SUCCEEDED( hr ) && hr != S_FALSE )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            *resourceId = (LPWSTR)CoTaskMemAlloc( 14 * sizeof(WCHAR) );
            if ( *resourceId )
                lstrcpynW( *resourceId, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
            else
                hr = E_OUTOFMEMORY;
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI
AppxManifestPackageId::ComparePublisher( LPCWSTR other, BOOL *isSame ) noexcept
{
    HRESULT hr = S_OK;
    LPWSTR publisher;

    TRACE( "iface %p, resourceId %s, isSame %p.\n", this, debugstr_w( other ), isSame );

    if ( !other )
        return E_INVALIDARG;

    if ( !isSame )
        return E_POINTER;

    *isSame = FALSE;

    if ( FAILED( hr = GetPublisher( &publisher ) ) )
        return hr;

    if ( lstrcmpW( publisher, other ) == 0 )
        *isSame = TRUE;
    
    CoTaskMemFree( publisher );

    return S_OK;;
}

HRESULT WINAPI
AppxManifestPackageId::GetPackageFullName( LPWSTR *packageFullName ) noexcept
{
    APPX_PACKAGE_ARCHITECTURE2 architecture;
    HRESULT hr = S_OK;
    UINT64 packageVer; 
    LPWSTR name;
    LPWSTR resourceId = (LPWSTR)L"";
    LPWSTR publisher;
    WCHAR publisherId[14];
    WCHAR version[26];
    WCHAR architectureStr[8];

    TRACE( "iface %p, packageFullName %p.\n", this, packageFullName );

    if ( !packageFullName )
        return E_POINTER;

    if ( FAILED( hr = GetVersion( &packageVer ) ) )
        return hr;

    swprintf_s( reinterpret_cast<wchar_t *>(version), sizeof(version)/sizeof(WCHAR), L"%u.%u.%u.%u", 
                (UINT64)((packageVer >> 48) & 0xFFFF), 
                (UINT64)((packageVer >> 32) & 0xFFFF), 
                (UINT64)((packageVer >> 16) & 0xFFFF), 
                (UINT64)(packageVer & 0xFFFF) );

    if ( FAILED( hr = GetArchitecture2( &architecture ) ) )
        return hr;

    switch ( architecture )
    {
        case APPX_PACKAGE_ARCHITECTURE2_X86:
            lstrcpyW( architectureStr, (LPCWSTR)L"x86" );
            break;

        case APPX_PACKAGE_ARCHITECTURE2_ARM:
            lstrcpyW( architectureStr, (LPCWSTR)L"arm" );
            break;

        case APPX_PACKAGE_ARCHITECTURE2_X64:
            lstrcpyW( architectureStr, (LPCWSTR)L"x64" );
            break;

        case APPX_PACKAGE_ARCHITECTURE2_NEUTRAL:
            lstrcpyW( architectureStr, (LPCWSTR)L"neutral" );
            break;

        case APPX_PACKAGE_ARCHITECTURE2_ARM64:
            lstrcpyW( architectureStr, (LPCWSTR)L"arm64" );
            break;

        case APPX_PACKAGE_ARCHITECTURE2_X86_ON_ARM64:
            lstrcpyW( architectureStr, (LPCWSTR)L"x86a64" );
            break;

        case APPX_PACKAGE_ARCHITECTURE2_UNKNOWN:
            lstrcpyW( architectureStr, (LPCWSTR)L"unknown" );
            break;
    }

    if ( SUCCEEDED( hr = GetName( &name ) ) )
    {
        if ( SUCCEEDED( hr = GetPublisher( &publisher ) ) )
        {
            PublisherIdFromPublisherString( publisher, publisherId, 14 );
            CoTaskMemFree( publisher );
            if ( SUCCEEDED( hr = GetResourceId( &resourceId ) ) )
            {
                *packageFullName = (LPWSTR)CoTaskMemAlloc( ((lstrlenW(name) + 1 + 1) * sizeof(WCHAR)) +
                                                           ((lstrlenW(version) + 1 + 1) * sizeof(WCHAR)) +
                                                           ((lstrlenW(architectureStr) + 1 + 1) * sizeof(WCHAR)) +
                                                           ((lstrlenW(resourceId) + 1 + 1) * sizeof(WCHAR)) +
                                                           ((lstrlenW(publisherId) + 1) * sizeof(WCHAR)) );
                wsprintfW( *packageFullName, (LPCWSTR)L"%s_%s_%s_%s_%s", name, version, architectureStr, resourceId, publisherId );
                if ( resourceId ) CoTaskMemFree( resourceId );
            } 
        }
    }

    if ( hr == S_FALSE )
        hr = S_OK;

    CoTaskMemFree( name );
    
    return hr;
}

HRESULT WINAPI
AppxManifestPackageId::GetPackageFamilyName( LPWSTR *packageFamilyName ) noexcept
{
    HRESULT hr = S_OK;
    LPWSTR name;
    LPWSTR publisher;
    WCHAR publisherId[14];

    TRACE( "iface %p, packageFamilyName %p.\n", this, packageFamilyName );

    if ( !packageFamilyName )
        return E_POINTER;

    if ( SUCCEEDED( hr = GetName( &name ) ) )
    {
        if ( SUCCEEDED( hr = GetPublisher( &publisher ) ) )
        {
            PublisherIdFromPublisherString( publisher, publisherId, 14 );
            CoTaskMemFree( publisher );
            *packageFamilyName = (LPWSTR)CoTaskMemAlloc( ((lstrlenW(name) + 1 + 1) * sizeof(WCHAR)) +
                                                         ((lstrlenW(publisherId) + 1) * sizeof(WCHAR)) );
            wsprintfW( *packageFamilyName, (LPCWSTR)L"%s_%s", name, publisherId );
        }
    }

    if ( hr == S_FALSE )
        hr = S_OK;

    CoTaskMemFree( name );
    
    return hr;
}

/* IAppxManifestPackageId2 Methods */
HRESULT WINAPI
AppxManifestPackageId::GetArchitecture2( APPX_PACKAGE_ARCHITECTURE2 *architecture ) noexcept
{
    BSTR attributeName;
    LPWSTR architectureString = nullptr;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, architecture %p.\n", this, architecture );

    if ( !architecture )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"ProcessorArchitecture" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for manifest package id file %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_MANIFEST;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            architectureString = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( V_BSTR( &val ) ) + sizeof(WCHAR) );
            if ( architectureString )
                lstrcpynW( architectureString, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
            else
                hr = E_OUTOFMEMORY;
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    if ( SUCCEEDED( hr ) )
    {
        TRACE("architectureString is %s\n", debugstr_w(architectureString));
        if ( lstrcmpiW( architectureString, (LPWSTR)L"x86" ) == 0 )
            *architecture = APPX_PACKAGE_ARCHITECTURE2_X86;
        else if ( lstrcmpiW( architectureString, (LPWSTR)L"arm" ) == 0 )
            *architecture = APPX_PACKAGE_ARCHITECTURE2_ARM;
        else if ( lstrcmpiW( architectureString, (LPWSTR)L"x64" ) == 0 )
            *architecture = APPX_PACKAGE_ARCHITECTURE2_X64;
        else if ( lstrcmpiW( architectureString, (LPWSTR)L"neutral" ) == 0 )
            *architecture = APPX_PACKAGE_ARCHITECTURE2_NEUTRAL;
        else if ( lstrcmpiW( architectureString, (LPWSTR)L"arm64" ) == 0 )
            *architecture = APPX_PACKAGE_ARCHITECTURE2_ARM64;
        else if ( lstrcmpiW( architectureString, (LPWSTR)L"x86a64" ) == 0 )
            *architecture = APPX_PACKAGE_ARCHITECTURE2_X86_ON_ARM64;
        else
            *architecture = APPX_PACKAGE_ARCHITECTURE2_UNKNOWN;
    }

    if ( architectureString ) CoTaskMemFree( architectureString );

    return hr;
}

// ------------- IAppxManifestReader ------------- //
AppxManifestReader::AppxManifestReader( IAppxFile *file )
    : appxFile( file )
{
    file->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxManifestReader::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxManifestReader ) )
    {
        AddRef();
        *out = static_cast<IAppxManifestReader *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxManifestReader::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxManifestReader::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( appxFile ) appxFile->Release();
        if ( xmlDocument ) xmlDocument->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxManifestReader Methods */
HRESULT WINAPI
AppxManifestReader::GetPackageId( IAppxManifestPackageId **packageId ) noexcept
{
    BSTR nodePath;
    HRESULT hr;
    AppxManifestPackageId *package = nullptr;

    TRACE( "iface %p, packageId %p.\n", this, packageId );
    
    if ( !packageId )
        return E_POINTER;

    nodePath = SysAllocString( (const OLECHAR *)L"/ns:Package/ns:Identity" );
    if ( !nodePath )
        return E_OUTOFMEMORY;

    hr = FetchSingleNode<AppxManifestPackageId>( nodePath, &package );
    SysFreeString( nodePath );
    if ( SUCCEEDED( hr ) )
        *packageId = package;

    return hr;
}

HRESULT WINAPI
AppxManifestReader::GetProperties( IAppxManifestProperties **packageProperties ) noexcept
{
    BSTR nodePath;
    HRESULT hr;
    AppxManifestProperties *package = nullptr;

    TRACE( "iface %p, packageProperties %p.\n", this, packageProperties );
    
    if ( !packageProperties )
        return E_POINTER;

    nodePath = SysAllocString( (const OLECHAR *)L"/ns:Package/ns:Properties" );
    if ( !nodePath )
        return E_OUTOFMEMORY;

    hr = FetchSingleNode<AppxManifestProperties>( nodePath, &package );
    SysFreeString( nodePath );
    if ( SUCCEEDED( hr ) )
        *packageProperties = package;

    return hr;
}

HRESULT WINAPI
AppxManifestReader::GetPackageDependencies( IAppxManifestPackageDependenciesEnumerator **dependencies ) noexcept
{
    BSTR nodePath;
    BSTR nodes;
    HRESULT hr;
    AppxManifestPackageDependenciesEnumerator *enumerator = nullptr;

    TRACE( "iface %p, dependencies %p.\n", this, dependencies );
    
    if ( !dependencies )
        return E_POINTER;

    nodePath = SysAllocString( (const OLECHAR *)L"/ns:Package/ns:Dependencies" );
    if ( !nodePath )
        return E_OUTOFMEMORY;

    nodes = SysAllocString( (const OLECHAR *)L"ns:PackageDependency" );
    if ( !nodes )
    {
        SysFreeString( nodePath );
        return E_OUTOFMEMORY;
    }

    hr = FetchNodeList<AppxManifestPackageDependenciesEnumerator>( nodePath, nodes, &enumerator );
    SysFreeString( nodePath );
    SysFreeString( nodes );
    if ( SUCCEEDED( hr ) )
        *dependencies = enumerator;

    return hr;
}

HRESULT WINAPI
AppxManifestReader::GetCapabilities( APPX_CAPABILITIES *capabilities ) noexcept
{
    IXMLDOMDocument3 *document = nullptr;
    IXMLDOMNodeList *nodeList = nullptr;
    IXMLDOMElement *elem = nullptr;
    IXMLDOMNode *node = nullptr;
    HRESULT hr = S_OK;
    LPWSTR capability;
    LONG listLength;
    LONG iterator;
    BSTR nodePath;
    BSTR nodes;

    TRACE( "iface %p, capabilities %p.\n", this, capabilities );

    if ( !capabilities )
        return E_POINTER;

    nodePath = SysAllocString( (const OLECHAR *)L"/ns:Capabilities" );
    if ( !nodePath )
        return E_OUTOFMEMORY;

    nodes = SysAllocString( (const OLECHAR *)L"*[local-name()='Capability']" );
    if ( !nodes )
        return E_OUTOFMEMORY;

    hr = FetchDocument( &document );
    if ( FAILED( hr ) )
    {
        SysFreeString( nodePath );
        SysFreeString( nodes );
        return hr;
    }

    hr = document->selectSingleNode( nodePath, &node );
    if ( SUCCEEDED( hr ) && node )
    {
        hr = node->selectNodes( nodes, &nodeList ); 
    } else if ( !node )
        hr = APPX_E_INVALID_MANIFEST;

    document->Release();
    SysFreeString( nodes );
    SysFreeString( nodePath );
    node->Release();

    if ( FAILED( hr ) )
    {
        nodeList->Release();
        return hr;
    }

    if ( FAILED( hr = nodeList->get_length( &listLength ) ) )
    {
        nodeList->Release();
        return hr;
    }

    for ( iterator = 0; iterator < listLength; iterator++ )
    {
        if ( SUCCEEDED( hr = nodeList->get_item( iterator, &node ) ) )
        {
            hr = node->QueryInterface( &elem );
            node->Release();
            if ( SUCCEEDED( hr ) )
            {
                hr = ObtainAttributeFromXMLElement( elem, (const OLECHAR *)L"Name", &capability );
                elem->Release();
            }
        }

        if ( SUCCEEDED( hr ) )
        {
            if ( lstrcmpW( capability, (LPCWSTR)L"internetClient" ) )
                *capabilities &= APPX_CAPABILITY_INTERNET_CLIENT;
            else if ( lstrcmpW( capability, (LPCWSTR)L"contacts" ) )
                *capabilities &= APPX_CAPABILITY_CONTACTS;
            else if ( lstrcmpW( capability, (LPCWSTR)L"enterpriseAuthentication" ) )
                *capabilities &= APPX_CAPABILITY_ENTERPRISE_AUTHENTICATION;
            else if ( lstrcmpW( capability, (LPCWSTR)L"internetClientServer" ) )
                *capabilities &= APPX_CAPABILITY_INTERNET_CLIENT_SERVER;
            else if ( lstrcmpW( capability, (LPCWSTR)L"musicLibrary" ) )
                *capabilities &= APPX_CAPABILITY_MUSIC_LIBRARY;
            else if ( lstrcmpW( capability, (LPCWSTR)L"picturesLibrary" ) )
                *capabilities &= APPX_CAPABILITY_PICTURES_LIBRARY;
            else if ( lstrcmpW( capability, (LPCWSTR)L"removableStorage" ) )
                *capabilities &= APPX_CAPABILITY_REMOVABLE_STORAGE;
            else if ( lstrcmpW( capability, (LPCWSTR)L"sharedUserCertificates" ) )
                *capabilities &= APPX_CAPABILITY_SHARED_USER_CERTIFICATES;
            else if ( lstrcmpW( capability, (LPCWSTR)L"videosLibrary" ) )
                *capabilities &= APPX_CAPABILITY_VIDEOS_LIBRARY;
            else if ( lstrcmpW( capability, (LPCWSTR)L"documentsLibrary" ) )
                *capabilities &= APPX_CAPABILITY_DOCUMENTS_LIBRARY;
            else if ( lstrcmpW( capability, (LPCWSTR)L"privateNetworkClientServer" ) )
                *capabilities &= APPX_CAPABILITY_PRIVATE_NETWORK_CLIENT_SERVER;
            else if ( lstrcmpW( capability, (LPCWSTR)L"appointments" ) )
                *capabilities &= APPX_CAPABILITY_APPOINTMENTS;
        } else
            break;
    }

    nodeList->Release();

    return hr;
}

HRESULT WINAPI
AppxManifestReader::GetResources( IAppxManifestResourcesEnumerator **resources ) noexcept
{
    FIXME( "iface %p, resources %p stub!\n", this, resources );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestReader::GetDeviceCapabilities( IAppxManifestDeviceCapabilitiesEnumerator **deviceCapabilities ) noexcept
{
    FIXME( "iface %p, deviceCapabilities %p stub!\n", this, deviceCapabilities );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestReader::GetPrerequisite( LPCWSTR name, UINT64 *value ) noexcept
{
    FIXME( "iface %p, name %s, value %p stub!\n", this, debugstr_w( name ), value );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestReader::GetApplications( IAppxManifestApplicationsEnumerator **applications ) noexcept
{
    FIXME( "iface %p, applications %p stub!\n", this, applications );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestReader::GetStream( IStream **manifestStream ) noexcept
{
    TRACE( "iface %p, manifestStream %p.\n", this, manifestStream );

    return appxFile->GetStream( manifestStream );
}

/* IAppxManifestReader2 Methods */
HRESULT WINAPI
AppxManifestReader::GetQualifiedResources( IAppxManifestQualifiedResourcesEnumerator **resources ) noexcept
{
    FIXME( "iface %p, resources %p stub!\n", this, resources );
    return E_NOTIMPL;
}

/* Internal Methods */
HRESULT WINAPI 
AppxManifestReader::FetchDocument( IXMLDOMDocument3 **document ) noexcept
{
    IStream *manifestStream = nullptr;
    IXMLDOMElement *rootElem = nullptr;
    HRESULT hr = S_OK;
    VARIANT var;
    VARIANT nsval;
    VARIANT_BOOL bLoaded = VARIANT_FALSE;
    BSTR attributeName;

    TRACE( "iface %p, document %p.\n", this, document );

    if ( !document )
        return E_POINTER;
    
    if ( !xmlDocument )
    {
        hr = CoCreateInstance( __uuidof(DOMDocument60), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &xmlDocument ) );
        if ( FAILED( hr ) ) 
            return hr;

        hr = xmlDocument->put_async( VARIANT_FALSE );
        if ( FAILED( hr ) ) 
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return hr;
        }

        hr = GetStream( &manifestStream );
        if ( FAILED( hr ) ) 
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return hr;
        }

        VariantInit( &var );
        var.vt = VT_UNKNOWN;
        var.punkVal = static_cast<IUnknown *>( manifestStream );
        manifestStream->AddRef();

        hr = xmlDocument->load( var, &bLoaded );
        manifestStream->Release();
        VariantClear( &var );
        if ( FAILED( hr ) || bLoaded != VARIANT_TRUE ) 
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return hr;
        }

        attributeName = SysAllocString( (OLECHAR *)L"xmlns" );
        if ( !attributeName )
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return E_OUTOFMEMORY;
        }

        hr = xmlDocument->get_documentElement( &rootElem );
        if ( SUCCEEDED( hr ) && rootElem )
        {
            VariantInit( &var );
            hr = rootElem->getAttribute( attributeName, &var );
            if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &var ) == VT_EMPTY || V_VT( &var ) == VT_NULL )) )
                hr = APPX_E_INVALID_MANIFEST;

            if ( SUCCEEDED( hr ) )
            {
                hr = VariantChangeType( &var, &var, 0, VT_BSTR );
                if ( SUCCEEDED( hr ) )
                {
                    if ( lstrcmpiW( V_BSTR( &var ), GetResourceW( IDS_XMLNS_MANIFEST_URI ) ) == 0 )
                    {
                        VariantInit( &nsval );
                        nsval.vt = VT_BSTR;
                        nsval.bstrVal = SysAllocStringLen( nullptr, lstrlenW( (LPWSTR)L"xmlns:ns='" ) + SysStringLen( V_BSTR( &var ) ) + lstrlenW( (LPWSTR)L"'" ) );
                        if ( !nsval.bstrVal )
                        {
                            hr = E_OUTOFMEMORY;
                        }
                        else
                        {
                            lstrcpyW( nsval.bstrVal, (LPWSTR)L"xmlns:ns='" );
                            lstrcatW( nsval.bstrVal, V_BSTR( &var ) );
                            lstrcatW( nsval.bstrVal, (LPWSTR)L"'" );
                        }

                        if ( SUCCEEDED( hr ) )
                            hr = xmlDocument->setProperty( (BSTR)L"SelectionNamespaces", nsval );

                        VariantClear( &nsval );
                    } else 
                    {
                        hr = APPX_E_INVALID_MANIFEST;
                    }
                }
            }

            VariantClear( &var );
            rootElem->Release();
        } else if ( SUCCEEDED( hr ) )
            hr = APPX_E_INVALID_MANIFEST;

        SysFreeString( attributeName );

        if ( FAILED( hr ) )
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return hr;
        }
    }
       
    hr = xmlDocument->QueryInterface( document );

    return hr;
}

template <typename Object>
HRESULT WINAPI
AppxManifestReader::FetchSingleNode( BSTR nodePath, Object **out ) noexcept
{
    IXMLDOMDocument3 *document = nullptr;
    IXMLDOMElement *nodeElem = nullptr;
    IXMLDOMNode *node = nullptr;
    HRESULT hr = S_OK;

    hr = FetchDocument( &document );
    if ( FAILED( hr ) )
        return hr;

    hr = document->selectSingleNode( nodePath, &node );
    if ( SUCCEEDED( hr ) && node )
    {
        hr = node->QueryInterface( &nodeElem );
        node->Release();
        if ( SUCCEEDED( hr ) )
        {
            *out = new(std::nothrow) Object( nodeElem );
            nodeElem->Release();
            if ( !*out ) 
                hr = E_OUTOFMEMORY;
        }
    } else if ( !node )
        hr = APPX_E_INVALID_MANIFEST;

    document->Release();

    return hr;
}

template <typename Object>
HRESULT WINAPI
AppxManifestReader::FetchNodeList( BSTR nodePath, BSTR nodes, Object **out ) noexcept
{
    IXMLDOMDocument3 *document = nullptr;
    IXMLDOMNodeList *nodeList = nullptr;
    IXMLDOMNode *node = nullptr;
    HRESULT hr = S_OK;

    hr = FetchDocument( &document );
    if ( FAILED( hr ) )
        return hr;

    hr = document->selectSingleNode( nodePath, &node );
    if ( SUCCEEDED( hr ) && node )
    {
        hr = node->selectNodes( nodes, &nodeList );
        if ( SUCCEEDED( hr ) )
        {
            *out = new(std::nothrow) Object( nodeList );
            nodeList->Release();
            if ( !*out ) 
                hr = E_OUTOFMEMORY;
        }
    } else if ( !node )
        hr = APPX_E_INVALID_MANIFEST;

    document->Release();
    if ( node ) node->Release();

    return hr;
}