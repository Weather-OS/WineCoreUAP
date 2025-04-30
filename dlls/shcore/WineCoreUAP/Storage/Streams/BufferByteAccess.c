/* WinRT robuffer.h:IBufferByteAccess Implementation
 *
 * Copyright 2024 Onni Kukkonen
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

//This needs to be implemented as a per-library basis.

#include "BufferInternal.h"

_ENABLE_DEBUGGING_

static inline struct buffer *impl_from_IBufferByteAccess( IBufferByteAccess *iface )
{
    return CONTAINING_RECORD( iface, struct buffer, IBufferByteAccess_iface );
}

static HRESULT WINAPI bufferaccess_impl_QueryInterface( IBufferByteAccess *iface, REFIID iid, void **out )
{
    struct buffer *impl = impl_from_IBufferByteAccess( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IBufferByteAccess ))
    {
        *out = &impl->IBufferByteAccess_iface;
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI bufferaccess_impl_AddRef( IBufferByteAccess *iface )
{
    struct buffer *impl = impl_from_IBufferByteAccess( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI bufferaccess_impl_Release( IBufferByteAccess *iface )
{
    struct buffer *impl = impl_from_IBufferByteAccess( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI bufferaccess_impl_get_Buffer( IBufferByteAccess *iface, BYTE **value )
{
    struct buffer *impl = impl_from_IBufferByteAccess( iface );

    TRACE( "iface %p value %p \n", iface, value );

    if ( !value ) return E_POINTER;

    *value = impl->Buffer;
    return S_OK;
}

struct IBufferByteAccessVtbl bufferaccess_vtbl =
{
    bufferaccess_impl_QueryInterface,
    bufferaccess_impl_AddRef,
    bufferaccess_impl_Release,
    /* IBufferByteAccess methods */
    bufferaccess_impl_get_Buffer
};