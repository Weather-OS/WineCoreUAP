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
    FIXME( "iface %p, compressionOption %p.\n", this, compressionOption );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxFile::GetContentType( LPWSTR *contentType ) noexcept
{
    FIXME( "iface %p, contentType %p.\n", this, contentType );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxFile::GetName( LPWSTR *fileName ) noexcept
{
    FIXME( "iface %p, fileName %p.\n", this, fileName );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxFile::GetSize( UINT64 *size ) noexcept
{
    FIXME( "iface %p, size %p.\n", this, size );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxFile::GetStream( IStream **stream ) noexcept
{
    FIXME( "iface %p, stream %p.\n", this, stream );
    return E_NOTIMPL;
}

// ------------- IAppxBlockMapReader ------------- //
AppxBlockMapReader::AppxBlockMapReader( IStream *fileStream )
    : blockMapFileStream( fileStream )
{
    fileStream->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapReader::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapReader ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapReader *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapReader::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapReader::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( blockMapFileStream ) blockMapFileStream->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxBlockMapReader Methods */
HRESULT WINAPI
AppxBlockMapReader::GetFile( LPCWSTR filename, IAppxBlockMapFile **file ) noexcept
{
    FIXME( "iface %p, filename %s, file %p.\n", this, debugstr_w( filename ), file );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxBlockMapReader::GetFiles( IAppxBlockMapFilesEnumerator **enumerator ) noexcept
{
    FIXME( "iface %p, enumerator %p stub!\n", this, enumerator );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxBlockMapReader::GetHashMethod( IUri **hashMethod ) noexcept
{
    FIXME( "iface %p, hashMethod %p stub!\n", this, hashMethod );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxBlockMapReader::GetStream( IStream **blockMapStream ) noexcept
{
    FIXME( "iface %p, blockMapStream %p stub!\n", this, blockMapStream );
    return E_NOTIMPL;
}