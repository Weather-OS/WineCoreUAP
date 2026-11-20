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
#include "bcrypt.h"
#include "corecrt_wstdio.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

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
        hr = APPX_E_INVALID_BLOCKMAP;
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
        hr = APPX_E_INVALID_BLOCKMAP;
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
    WCHAR extra;
    LPWSTR version = nullptr;
    USHORT versioning[4];
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
        hr = APPX_E_INVALID_BLOCKMAP;
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

    if ( swscanf_s( reinterpret_cast<const wchar_t *>(version), L"%hu.%hu.%hu.%hu%c", &versioning[0], &versioning[1], &versioning[2], &versioning[3], &extra ) != 4 )
        return E_INVALIDARG;

    *packageVersion = ((UINT64)versioning[0] << 48) |
        ((UINT64)versioning[1] << 32) |
        ((UINT64)versioning[2] << 16) |
        (UINT64)versioning[3];

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
        ERR( "for manifest package id %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
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
        ERR( "for blockmap file %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
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
        delete this;
    }
    
    return curr;
}

/* IAppxManifestReader Methods */
HRESULT WINAPI
AppxManifestReader::GetPackageId( IAppxManifestPackageId **packageId ) noexcept
{
    IXMLDOMDocument3 *document = nullptr;
    IXMLDOMNode *packageNode = nullptr;
    IXMLDOMElement *packageElem = nullptr;
    IXMLDOMElement *rootElem = nullptr;
    HRESULT hr = S_OK;
    VARIANT val;
    VARIANT nsval;
    BSTR attributeName;

    TRACE( "iface %p, packageId %p.\n", this, packageId );

    if ( !packageId )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"xmlns" );
    if ( !attributeName )
        return E_OUTOFMEMORY;

    hr = FetchDocument( &document );
    if ( FAILED( hr ) )
    {
        SysFreeString( attributeName );
        return hr;
    }

    hr = document->get_documentElement( &rootElem );
    if ( FAILED( hr ) || !rootElem )
    {
        if ( rootElem )
            rootElem->Release();

        document->Release();
        SysFreeString( attributeName );

        return FAILED( hr ) ? hr : APPX_E_INVALID_BLOCKMAP;
    }

    VariantInit( &val );
    
    hr = rootElem->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for manifest reader %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            // uri needs validation
            if ( lstrcmpiW( V_BSTR( &val ), GetResourceW( IDS_XMLNS_MANIFEST_URI ) ) == 0 )
            {
                VariantInit( &nsval );
                nsval.vt = VT_BSTR;
                nsval.bstrVal = SysAllocStringLen( nullptr, lstrlenW( (LPWSTR)L"xmlns:ns='" ) + SysStringLen( V_BSTR( &val ) ) + lstrlenW( (LPWSTR)L"'" ) );
                if ( !nsval.bstrVal )
                {
                    hr = E_OUTOFMEMORY;
                }
                else
                {
                    lstrcpyW( nsval.bstrVal, (LPWSTR)L"xmlns:ns='" );
                    lstrcatW( nsval.bstrVal, V_BSTR( &val ) );
                    lstrcatW( nsval.bstrVal, (LPWSTR)L"'" );
                }

                if ( SUCCEEDED( hr ) )
                {
                    document->setProperty( (BSTR)L"SelectionNamespaces", nsval );
                    VariantClear( &nsval );

                    hr = document->selectSingleNode( (BSTR)L"/ns:Package/ns:Identity", &packageNode );
                    if ( SUCCEEDED( hr ) && packageNode )
                    {
                        hr = packageNode->QueryInterface( &packageElem );
                        packageNode->Release();
                        if ( SUCCEEDED( hr ) )
                        {
                            *packageId = new(std::nothrow) AppxManifestPackageId( packageElem );
                            packageElem->Release();
                            if ( !*packageId ) 
                                hr = E_OUTOFMEMORY;
                        }
                    } else if ( !packageNode )
                    {
                        hr = APPX_E_INVALID_BLOCKMAP;
                    }
                }
            } else 
            {
                ERR( "for manifest reader %p, got wrong xmlns %s!\n", this, debugstr_w( V_BSTR( &val ) ) );
                hr = APPX_E_INVALID_BLOCKMAP;
            }
        }
    }

    document->Release();
    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI
AppxManifestReader::GetProperties( IAppxManifestProperties **packageProperties ) noexcept
{
    FIXME( "iface %p, packageProperties %p stub!\n", this, packageProperties );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestReader::GetPackageDependencies( IAppxManifestPackageDependenciesEnumerator **dependencies ) noexcept
{
    FIXME( "iface %p, dependencies %p stub!\n", this, dependencies );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestReader::GetCapabilities( APPX_CAPABILITIES *capabilities ) noexcept
{
    FIXME( "iface %p, capabilities %p stub!\n", this, capabilities );
    return E_NOTIMPL;
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
AppxManifestReader::GetPrerequisite( LPCWSTR name, UINT64 *value )noexcept
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
    HRESULT hr = S_OK;
    VARIANT var;
    VARIANT_BOOL bLoaded = VARIANT_FALSE;

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
    }
       
    hr = xmlDocument->QueryInterface( document );

    return hr;
}