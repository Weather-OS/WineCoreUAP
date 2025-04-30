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

#ifndef BUFFER_INTERNAL_H
#define BUFFER_INTERNAL_H

#include "../../../private.h"
#include "wine/debug.h"

extern struct IBufferVtbl buffer_vtbl;
extern struct IBufferByteAccessVtbl bufferaccess_vtbl;

struct buffer
{
    IBuffer IBuffer_iface;    
    IBufferByteAccess IBufferByteAccess_iface;
    BYTE *Buffer;    
    UINT Capacity;
    UINT Length;    
    LONG ref;
};

struct buffer_statics
{
    //Derivatives
    IActivationFactory IActivationFactory_iface;    
    IBufferFactory IBufferFactory_iface;
    IBufferStatics IBufferStatics_iface;
    LONG ref;
};

struct buffer *impl_from_IBuffer( IBuffer *iface );

HRESULT WINAPI buffer_Create( UINT32 capacity, IBuffer **value );
HRESULT WINAPI buffer_Grow( IBuffer *iface, UINT32 growSize );

#endif