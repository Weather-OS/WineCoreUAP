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
    FIXME( "iface %p, packageId %p stub!\n", this, packageId );
    return E_NOTIMPL;
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