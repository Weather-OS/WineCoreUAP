/* 
 * AppX Package Reader Implementation
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
#include "BlockMap.hpp"
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
    IAppxFile *file = nullptr;
    IAppxBlockMapReader *reader = nullptr;
   
    TRACE( "iface %p, blockMapReader %p.\n", this, blockMapReader );

    if ( !blockMapReader )
        return E_POINTER;

    if ( FAILED( hr = GetFootprintFile( APPX_FOOTPRINT_FILE_TYPE_BLOCKMAP, &file ) ) )
        return hr;

    reader = new(std::nothrow) AppxBlockMapReader( file );
    file->Release();
    if ( !reader ) 
    {
        return E_OUTOFMEMORY;
    }

    *blockMapReader = reader;

    return S_OK;
}

HRESULT WINAPI
AppxPackageReader::GetFootprintFile( APPX_FOOTPRINT_FILE_TYPE type, IAppxFile **file ) noexcept
{
    HRESULT hr = S_OK;

    TRACE( "iface %p, type %d, file %p.\n", this, type, file );

    switch ( type )
    {
        case APPX_FOOTPRINT_FILE_TYPE_MANIFEST:
            hr = GetFile( (LPCWSTR)L"/AppxManifest.xml", file );
            return hr;

        case APPX_FOOTPRINT_FILE_TYPE_BLOCKMAP:
            hr = GetFile( (LPCWSTR)L"/AppxBlockMap.xml", file );
            return hr;

        case APPX_FOOTPRINT_FILE_TYPE_SIGNATURE:
            hr = GetFile( (LPCWSTR)L"/AppxSignature.p7x", file );
            return hr;

        case APPX_FOOTPRINT_FILE_TYPE_CODEINTEGRITY:
            hr = GetFile( (LPCWSTR)L"/AppxMetadata/CodeIntegrity.cat", file );
            return hr;

        case APPX_FOOTPRINT_FILE_TYPE_CONTENTGROUPMAP:
            hr = GetFile( (LPCWSTR)L"/AppxContentGroupMap.xml", file );
            return hr;

        default:
            return E_INVALIDARG;
    }
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
    HRESULT hr;
    LPWSTR current_name;
    BOOL exists;
    IAppxFilesEnumerator *enumerator = nullptr;
    IAppxFile *current_file = nullptr;

    TRACE( "iface %p, fileName %s, file %p.\n", this, debugstr_w( fileName ), file );

    if ( !fileName )
        return E_INVALIDARG;

    if ( !file )
        return E_POINTER;

    if ( FAILED( hr = GetPayloadFiles( &enumerator ) ) )
        return hr;

    hr = enumerator->GetHasCurrent( &exists );
    if ( FAILED( hr ) )
    {
        enumerator->Release();
        return hr;
    }
    if ( !exists )
    {
        enumerator->Release();
        return APPX_E_INVALID_BLOCKMAP;
    }

    do
    {
        if ( FAILED( hr = enumerator->GetCurrent( &current_file ) ) )
            break;

        if ( FAILED( hr = current_file->GetName( &current_name ) ) )
            break;

        if ( lstrcmpW( current_name, fileName ) == 0 )
        {
            *file = current_file;
            break;
        }

        CoTaskMemFree( current_name );
        current_name = nullptr;
        current_file->Release();
        current_file = nullptr;

        if ( FAILED( hr = enumerator->MoveNext( &exists ) ) )
            break;
        
    } while ( exists );

    if ( FAILED( hr ) )
        if ( current_file ) current_file->Release();
    if ( current_name ) CoTaskMemFree( current_name );

    enumerator->Release();

    return hr;
}

HRESULT WINAPI
AppxPackageReader::GetPayloadFiles( IAppxFilesEnumerator **filesEnumerator ) noexcept
{
    BOOL exists;
    LPWSTR current_name = nullptr;
    SIZE_T count = 0;
    SIZE_T iterator = 0;
    HRESULT hr = S_OK;
    IAppxFile **appx_files = nullptr;
    IAppxFile **appx_files_tmp = nullptr;
    IAppxBlockMapFile *current_file = nullptr;
    IAppxBlockMapReader *reader = nullptr;
    IAppxBlockMapFilesEnumerator *blockMapFiles = nullptr;

    TRACE( "iface %p, filesEnumerator %p.\n", this, filesEnumerator );

    if ( !filesEnumerator )
        return E_POINTER;

    if ( FAILED( hr = GetBlockMap( &reader ) ) )
        return hr;

    hr = reader->GetFiles( &blockMapFiles );
    reader->Release();
    if ( FAILED( hr ) )
        return hr;

    appx_files = (IAppxFile **)CoTaskMemAlloc( sizeof(IAppxFile *) );
    if ( !appx_files )
        return E_OUTOFMEMORY;

    hr = blockMapFiles->GetHasCurrent( &exists );
    if ( FAILED( hr ) )
    {
        blockMapFiles->Release();
        return hr;
    }
    if ( !exists )
    {
        blockMapFiles->Release();
        return APPX_E_INVALID_BLOCKMAP;
    }

    do
    {
        if ( FAILED( hr = blockMapFiles->GetCurrent( &current_file ) ) )
            break;

        if ( FAILED( hr = current_file->GetName( &current_name ) ) )
            break;

        if ( FAILED( hr = GetFile( current_name, &appx_files[count] ) ) )
            break;

        CoTaskMemFree( current_name );
        current_name = nullptr;

        count++;

        appx_files_tmp = (IAppxFile **)CoTaskMemRealloc( appx_files, (count) * sizeof(IAppxFile *) );
        if ( !appx_files_tmp )
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        appx_files = appx_files_tmp;

        current_file->Release();
        current_file = nullptr;

        if ( FAILED( hr = blockMapFiles->MoveNext( &exists ) ) )
            break;
        
    } while ( exists );

    if ( FAILED( hr ) )
    {
        for ( iterator = 0; iterator < count; iterator++ )
            appx_files[iterator]->Release();
        CoTaskMemFree( appx_files );
    }

    if ( current_file ) current_file->Release();
    if ( current_name ) CoTaskMemFree( current_name );
    blockMapFiles->Release();

    if ( SUCCEEDED( hr ) )
    {
        // ownership of all files are handed to AppxFilesEnumerator
        *filesEnumerator = new(std::nothrow) AppxFilesEnumerator( appx_files, count );
        if ( !*filesEnumerator ) 
            hr = E_OUTOFMEMORY;
    }

    return hr;
}

/* Internal Methods */

// AppxPackageReader::GetPayloadFiles is only supposed to fetch files 
// outside of Footprint files.
HRESULT WINAPI
AppxPackageReader::GetFile( LPCWSTR fileName, IAppxFile **file ) noexcept
{
    HRESULT hr;
    IOpcPart *part = nullptr;
    IOpcPartSet *part_set = nullptr;
    IOpcPartUri *part_uri = nullptr;
    IOpcFactory *opc_factory = nullptr;
    IAppxFile *appx_file;

    TRACE( "iface %p, fileName %s, file %p.\n", this, debugstr_w( fileName ), file );

    if ( !fileName )
        return E_INVALIDARG;

    if ( !file )
        return E_POINTER;

    hr = CoCreateInstance( CLSID_OpcFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &opc_factory ) );
    if ( FAILED( hr ) ) 
        return hr;

    hr = opc_factory->CreatePartUri( fileName, &part_uri );
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
        return hr;

    appx_file = new(std::nothrow) AppxFile( part );
    part->Release();
    if ( !appx_file ) 
        return E_OUTOFMEMORY;

    *file = appx_file;

    return S_OK;
}

HRESULT WINAPI
AppxPackageReader::GetFiles( IAppxFilesEnumerator **enumerator ) noexcept
{
    HRESULT hr;
    SIZE_T count = 0;
    SIZE_T iterator = 0;
    BOOL exists;
    IOpcPart *current_part = nullptr;
    IOpcPartSet *part_set = nullptr;
    IOpcPartEnumerator *opc_part_enumerator = nullptr;
    IAppxFile **appx_files = nullptr;
    IAppxFile **appx_files_tmp = nullptr;

    TRACE( "iface %p, enumerator %p.\n", this, enumerator );

    if ( !enumerator )
        return E_POINTER;

    if ( FAILED( hr = package->GetPartSet( &part_set ) ) )
        goto _CLEANUP;

    hr = part_set->GetEnumerator( &opc_part_enumerator );
    if ( FAILED( hr ) )
        goto _CLEANUP;

    if ( FAILED( hr = opc_part_enumerator->MoveNext( &exists ) ) )
    {
        if ( hr == OPC_E_ENUM_INVALID_POSITION )
            hr = APPX_E_CORRUPT_CONTENT;
        goto _CLEANUP;
    }

    appx_files = (IAppxFile **)CoTaskMemAlloc( sizeof(IAppxFile *) );
    if ( !appx_files )
    {
        hr = E_OUTOFMEMORY;
        goto _CLEANUP;
    }

    while ( exists )
    {
        if ( FAILED( hr = opc_part_enumerator->GetCurrent( &current_part ) ) )
            goto _CLEANUP;

        appx_files_tmp = (IAppxFile **)CoTaskMemRealloc( appx_files, (count + 1) * sizeof(IAppxFile *) );
        if ( !appx_files_tmp )
        {
            hr = E_OUTOFMEMORY;
            current_part->Release();
            current_part = nullptr;
            goto _CLEANUP;
        }
        appx_files = appx_files_tmp;

        appx_files[count] = new(std::nothrow) AppxFile( current_part );
        current_part->Release();
        current_part = nullptr;
        if ( !appx_files[count] ) 
        {
            hr = E_OUTOFMEMORY;
            goto _CLEANUP;
        }

        count++;

        if ( FAILED( hr = opc_part_enumerator->MoveNext( &exists ) ) )
            goto _CLEANUP;
    }

    *enumerator = new(std::nothrow) AppxFilesEnumerator( appx_files, count );
    if ( !*enumerator ) 
        hr = E_OUTOFMEMORY;

_CLEANUP:
    if ( FAILED( hr ) )
    {
        if ( current_part ) current_part->Release();
        for ( iterator = 0; iterator < count; iterator++ )
            appx_files[iterator]->Release();
        CoTaskMemFree( appx_files );
    }
    if ( part_set ) part_set->Release();
    if ( opc_part_enumerator ) opc_part_enumerator->Release();
    return hr;
}