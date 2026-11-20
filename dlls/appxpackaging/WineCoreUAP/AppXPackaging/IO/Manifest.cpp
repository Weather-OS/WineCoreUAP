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
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

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
    FIXME( "iface %p, name %p stub!\n", this, name );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestPackageId::GetArchitecture( APPX_PACKAGE_ARCHITECTURE *architecture ) noexcept
{
    FIXME( "iface %p, architecture %p stub!\n", this, architecture );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestPackageId::GetPublisher( LPWSTR *publisher ) noexcept
{
    FIXME( "iface %p, publisher %p stub!\n", this, publisher );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestPackageId::GetVersion( UINT64 *packageVersion ) noexcept
{
    FIXME( "iface %p, packageVersion %p stub!\n", this, packageVersion );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestPackageId::GetResourceId( LPWSTR *resourceId ) noexcept
{
    FIXME( "iface %p, resourceId %p stub!\n", this, resourceId );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestPackageId::ComparePublisher( LPCWSTR other, BOOL *isSame ) noexcept
{
    FIXME( "iface %p, resourceId %s, isSame %p stub!\n", this, debugstr_w( other ), isSame );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestPackageId::GetPackageFullName( LPWSTR *packageFullName ) noexcept
{
    FIXME( "iface %p, resourceId %p stub!\n", this, packageFullName );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxManifestPackageId::GetPackageFamilyName( LPWSTR *packageFamilyName ) noexcept
{
    FIXME( "iface %p, packageFamilyName %p stub!\n", this, packageFamilyName );
    return E_NOTIMPL;
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
                ERR( "for blockmap reader %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
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