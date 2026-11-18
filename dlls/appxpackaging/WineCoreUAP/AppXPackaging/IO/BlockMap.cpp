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

// ------------- IAppxBlockMapFile ------------- //
AppxBlockMapFile::AppxBlockMapFile( IXMLDOMElement *element )
    : xmlElement( element )
{
    element->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapFile::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapFile ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapFile *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapFile::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapFile::Release() noexcept
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

/* IAppxBlockMapFile Methods */
HRESULT WINAPI 
AppxBlockMapFile::GetBlocks( IAppxBlockMapBlocksEnumerator **blocks ) noexcept
{
    FIXME( "iface %p, blocks %p stub!\n", this, blocks );
    return E_NOTIMPL;
}

HRESULT WINAPI 
AppxBlockMapFile::GetLocalFileHeaderSize( UINT32 *lfhSize ) noexcept
{
    FIXME( "iface %p, lfhSize %p stub!\n", this, lfhSize );
    return E_NOTIMPL;
}

HRESULT WINAPI 
AppxBlockMapFile::GetName( LPWSTR *name ) noexcept
{
    FIXME( "iface %p, name %p stub!\n", this, name );
    return E_NOTIMPL;
}

HRESULT WINAPI 
AppxBlockMapFile::GetUncompressedSize( UINT64 *size ) noexcept
{
    FIXME( "iface %p, size %p stub!\n", this, size );
    return E_NOTIMPL;
}

HRESULT WINAPI 
AppxBlockMapFile::ValidateFileHash( IStream *fileStream, BOOL *isValid ) noexcept
{
    FIXME( "iface %p, fileStream %p, isValid %p stub!\n", this, fileStream, isValid );
    return E_NOTIMPL;
}

// ------------- IAppxBlockMapFilesEnumerator ------------- //
AppxBlockMapFilesEnumerator::AppxBlockMapFilesEnumerator( IXMLDOMDocument *xmlDocument )
    : document( xmlDocument )
{
    xmlDocument->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapFilesEnumerator::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapFilesEnumerator ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapFilesEnumerator *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapFilesEnumerator::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapFilesEnumerator::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        document->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxBlockMapFilesEnumerator Methods */
HRESULT WINAPI 
AppxBlockMapFilesEnumerator::GetCurrent( IAppxBlockMapFile **file ) noexcept
{
    FIXME( "iface %p, file %p stub!\n", this, file );
    return E_NOTIMPL;
}

HRESULT WINAPI 
AppxBlockMapFilesEnumerator::GetHasCurrent( BOOL *hasCurrent ) noexcept
{
    FIXME( "iface %p, hasCurrent %p stub!\n", this, hasCurrent );
    return E_NOTIMPL;
}

HRESULT WINAPI
AppxBlockMapFilesEnumerator::MoveNext( BOOL *hasNext ) noexcept
{
    FIXME( "iface %p, hasNext %p stub!\n", this, hasNext );
    return E_NOTIMPL;
}

// ------------- IAppxBlockMapReader ------------- //
AppxBlockMapReader::AppxBlockMapReader( IAppxFile *file )
    : appxFile( file )
{
    file->AddRef();
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
        if ( appxFile ) appxFile->Release();
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