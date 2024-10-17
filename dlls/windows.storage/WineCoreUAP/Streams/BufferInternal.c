/* WinRT Windows.Storage.Streams.Buffer Implementation
 *
 * Written by Weather
 *
 * This is a reverse engineered implementation of Microsoft's OneCoreUAP binaries.
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

HRESULT WINAPI buffer_Create( UINT32 capacity, IBuffer **value )
{
    HRESULT hr = S_OK;
    struct buffer *newBuffer;
    if (!(newBuffer = calloc( 1, sizeof(*newBuffer) ))) return E_OUTOFMEMORY;

    newBuffer->IBuffer_iface.lpVtbl = &buffer_vtbl;
    newBuffer->IBufferByteAccess_iface.lpVtbl = &bufferaccess_vtbl;

    newBuffer->Buffer = (BYTE *)malloc( capacity );

    if ( capacity > 0x7fffffffu )
        hr = E_INVALIDARG;
    else
        newBuffer->Capacity = capacity;

    *value = &newBuffer->IBuffer_iface;

    return hr;
}