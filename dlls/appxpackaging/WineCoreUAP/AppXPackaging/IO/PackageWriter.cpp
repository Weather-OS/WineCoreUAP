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

#include <new>
#include <atomic>

#include "PackageIO.hpp"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

AppxPackageWriter::AppxPackageWriter( IStream *fileStream )
    : packageFileStream( fileStream )
{
    fileStream->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxPackageWriter::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxPackageWriter ) )
    {
        AddRef();
        *out = static_cast<IAppxPackageWriter *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxPackageWriter::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxPackageWriter::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( packageFileStream ) packageFileStream->Release();
        delete this;
    }
    
    return curr;
}

// P.S: Don't waste your time implementing package writers.
// No one is packaging appx on non-Windows platforms.

/* IAppxPackageWriter Methods */
HRESULT WINAPI 
AppxPackageWriter::AddPayloadFile( LPCWSTR fileName, LPCWSTR contentType, APPX_COMPRESSION_OPTION compressionOption, 
                IStream *inputStream ) noexcept
{
    FIXME( "iface %p, fileName %s, contentType %s, compressionOption %d, inputStream %p stub!\n", 
        this, debugstr_w(fileName), debugstr_w(contentType), compressionOption, inputStream );
    return E_NOTIMPL;
}

HRESULT WINAPI 
AppxPackageWriter::Close( IStream *manifest ) noexcept
{
    FIXME( "iface %p, manifest %p stub!\n", this, manifest );
    return E_NOTIMPL;
}