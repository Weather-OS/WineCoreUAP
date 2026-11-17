/* 
 * AppXFactory Class Factory Implementation.
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

#include "PackageIO.hpp"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

AppxPackageReader::AppxPackageReader( IStream *fileStream, IOpcPackage *opcPackage )
    : packageFileStream( fileStream ), package( opcPackage )
{
    fileStream->AddRef();
    opcPackage->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxPackageReader::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxPackageReader ) )
    {
        AddRef();
        *out = static_cast<IAppxPackageReader *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxPackageReader::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxPackageReader::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( packageFileStream ) packageFileStream->Release();
        if ( package ) package->Release();

        delete this;
    }
    
    return curr;
}

/* IAppxPackageReader Methods */
HRESULT WINAPI
AppxPackageReader::GetBlockMap( IAppxBlockMapReader **blockMapReader ) noexcept
{
    HRESULT hr;
    IStream *part_stream = nullptr;
    IOpcPart *part = nullptr;
    IOpcPartSet *part_set = nullptr;
    IOpcPartUri *part_uri = nullptr;
    IOpcFactory *opc_factory = nullptr;
    AppxBlockMapReader *reader;

    TRACE( "iface %p, blockMapReader %p stub!\n", this, blockMapReader );

    hr = CoCreateInstance( CLSID_OpcFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &opc_factory ) );
    if ( FAILED( hr ) ) 
        return hr;

    hr = opc_factory->CreatePartUri( (LPWSTR)L"/AppxBlockMap.xml", &part_uri );
    opc_factory->Release();
    if ( FAILED( hr ) ) 
        return hr;

    if ( FAILED( hr = package->GetPartSet( &part_set ) ) )
    {
        part_uri->Release();
        return hr;
    }

    hr = part_set->GetPart( part_uri, &part );
    part_uri->Release();
    part_set->Release();
    if ( FAILED( hr ) )
    {
        if ( hr == OPC_E_NO_SUCH_PART )
            ERR("Could not find AppxBlockMap.xml. AppX Package is malformed!\n");

        return hr;
    }

    hr = part->GetContentStream( &part_stream );
    part->Release();
    if ( FAILED( hr ) )
        return hr;

    reader = new(std::nothrow) AppxBlockMapReader( part_stream );
    if ( !reader ) 
    {
        part_stream->Release();
        return E_OUTOFMEMORY;
    }

    *blockMapReader = reader;

    return S_OK;
}

HRESULT WINAPI
AppxPackageReader::GetFootprintFile( APPX_FOOTPRINT_FILE_TYPE type, IAppxFile **file ) noexcept
{
    FIXME( "iface %p, type %d, file %p stub!\n", this, type, file );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxPackageReader::GetManifest( IAppxManifestReader **manifestReader ) noexcept
{
    FIXME( "iface %p, manifestReader %p stub!\n", this, manifestReader );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxPackageReader::GetPayloadFile( LPCWSTR fileName, IAppxFile **file ) noexcept
{
    FIXME( "iface %p, fileName %s, file %p stub!\n", this, debugstr_w(fileName), file );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxPackageReader::GetPayloadFiles( IAppxFilesEnumerator **filesEnumerator ) noexcept
{
    FIXME( "iface %p, filesEnumerator %p stub!\n", this, filesEnumerator );
    return E_NOTIMPL;
}