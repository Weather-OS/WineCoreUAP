/* 
 * AppX File readers implementation.
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

// ------------- IAppxFile ------------- //
AppxFile::AppxFile( IOpcPart *part )
    : filePart( part )
{
    part->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxFile::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxFile ) )
    {
        AddRef();
        *out = static_cast<IAppxFile *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxFile::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxFile::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( filePart ) filePart->Release();

        delete this;
    }
    
    return curr;
}

/* IAppxFile Methods */
HRESULT WINAPI
AppxFile::GetCompressionOption( APPX_COMPRESSION_OPTION *compressionOption ) noexcept
{
    HRESULT hr = S_OK;
    OPC_COMPRESSION_OPTIONS option;

    TRACE( "iface %p, compressionOption %p.\n", this, compressionOption );

    if ( FAILED( hr = filePart->GetCompressionOptions( &option ) ) )
        return hr;

    // OPC_COMPRESSION_OPTIONS maps almost perfectly to APPX_COMPRESSION_OPTION.
    // we just need to move it 1 step to the right.
    
    *compressionOption = (APPX_COMPRESSION_OPTION)(option + 1);

    return S_OK;
}

HRESULT WINAPI
AppxFile::GetContentType( LPWSTR *contentType ) noexcept
{
    TRACE( "iface %p, contentType %p.\n", this, contentType );

    return filePart->GetContentType( contentType );
}

HRESULT WINAPI
AppxFile::GetName( LPWSTR *fileName ) noexcept
{
    BSTR path;
    HRESULT hr = S_OK;
    IOpcPartUri *part_uri = nullptr;

    TRACE( "iface %p, fileName %p.\n", this, fileName );

    if ( !fileName )
        return E_POINTER;

    if ( FAILED( hr = filePart->GetName( &part_uri ) ) )
        return hr;

    hr = part_uri->GetPath( &path );
    part_uri->Release();
    if ( FAILED( hr ) )
        return hr;

    *fileName = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( path ) + sizeof(WCHAR) );
    if ( !*fileName )
    {
        SysFreeString( path );
        return E_OUTOFMEMORY;
    }
    lstrcpynW( *fileName, path, SysStringLen( path ) + 1 );
    SysFreeString( path );

    return hr;
}

HRESULT WINAPI
AppxFile::GetSize( UINT64 *size ) noexcept
{
    HRESULT hr = S_OK;
    IStream *stream = nullptr;
    LARGE_INTEGER move = {};
    ULARGE_INTEGER newPosition = {};

    TRACE( "iface %p, size %p.\n", this, size );

    hr = filePart->GetContentStream( &stream );
    if ( FAILED( hr ) )
        return hr;

    // IOpcPart stream does not support stat in wine.
    hr = stream->Seek( move, STREAM_SEEK_END, &newPosition );
    stream->Release();

    *size = newPosition.QuadPart;

    return hr;
}

HRESULT WINAPI
AppxFile::GetStream( IStream **stream ) noexcept
{
    TRACE( "iface %p, stream %p.\n", this, stream );

    return filePart->GetContentStream( stream );
}

// ------------- IAppxFilesEnumerator ------------- //
AppxFilesEnumerator::AppxFilesEnumerator( IAppxFile **files, SIZE_T size )
    : files( files ), filesCount( size )
{
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxFilesEnumerator::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxFilesEnumerator ) )
    {
        AddRef();
        *out = static_cast<IAppxFilesEnumerator *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxFilesEnumerator::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxFilesEnumerator::Release() noexcept
{
    SIZE_T iterator;
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        for ( iterator = 0; iterator < filesCount; iterator++ )
        {
            files[iterator]->Release();
        }

        delete this;
    }
    
    return curr;
}

/* IAppxFilesEnumerator Methods */
HRESULT WINAPI 
AppxFilesEnumerator::GetCurrent( IAppxFile **file ) noexcept
{
    TRACE( "iface %p, file %p.\n", this, file );

    if ( index >= filesCount )
        return E_BOUNDS;

    *file = files[index];
    files[index]->AddRef();

    return S_OK;
}

HRESULT WINAPI 
AppxFilesEnumerator::GetHasCurrent( BOOL *hasCurrent ) noexcept
{
    TRACE( "iface %p, hasCurrent %p.\n", this, hasCurrent );

    *hasCurrent = FALSE;

    if ( index < filesCount )
        *hasCurrent = TRUE;

    return S_OK;
}

HRESULT WINAPI
AppxFilesEnumerator::MoveNext( BOOL *hasNext ) noexcept
{
    TRACE( "iface %p, hasNext %p.\n", this, hasNext );

    *hasNext = FALSE;

    if ( index >= filesCount )
        return E_BOUNDS;

    index++;

    if ( index < filesCount )
        *hasNext = TRUE;

    return S_OK;
}