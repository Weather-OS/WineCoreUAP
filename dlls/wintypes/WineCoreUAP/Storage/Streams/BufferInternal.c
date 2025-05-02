/* WinRT Windows.Storage.Streams.Buffer Implementation
 *
 * Written by Weather
 *
 * This is an implementation of Microsoft's OneCoreUAP binaries.
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

#include "BufferInternal.h"

_ENABLE_DEBUGGING_

HRESULT WINAPI buffer_Create( UINT32 capacity, IBuffer **value )
{
    HRESULT status = S_OK;

    struct buffer *newBuffer;

    TRACE( "capacity %d, value %p\n", capacity, value );

    if (!(newBuffer = calloc( 1, sizeof(*newBuffer) ))) return E_OUTOFMEMORY;

    newBuffer->IBuffer_iface.lpVtbl = &buffer_vtbl;
    newBuffer->IBufferByteAccess_iface.lpVtbl = &bufferaccess_vtbl;

    newBuffer->Buffer = (BYTE *)malloc( capacity );

    if ( capacity > 0x7fffffffu )
        status = E_INVALIDARG;
    else
        newBuffer->Capacity = capacity;

    *value = &newBuffer->IBuffer_iface;

    return status;
}

//Dynamic Reallocation
HRESULT WINAPI buffer_Grow( IBuffer *iface, UINT32 growSize )
{    
    HRESULT status = S_OK;
    BYTE *grownBuffer;

    struct buffer *buffer = impl_from_IBuffer( iface );

    TRACE( "iface %p, growSize %d\n", iface, growSize );

    buffer->Capacity += growSize;

    grownBuffer = (BYTE*)realloc( buffer->Buffer, buffer->Capacity );
    if ( !grownBuffer )
        return E_OUTOFMEMORY;
    
    buffer->Buffer = grownBuffer;

    return status;
}