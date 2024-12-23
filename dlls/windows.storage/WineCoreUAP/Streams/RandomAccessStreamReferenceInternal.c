/* WinRT Windows.Storage.Streams.RandomAccessStreamReference Implementation
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

#include "RandomAccessStreamInternal.h"
#include "RandomAccessStreamReferenceInternal.h"

_ENABLE_DEBUGGING_

HRESULT WINAPI random_access_stream_reference_CreateStream( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;

    struct random_access_stream *stream;
    struct random_access_stream_reference *reference = impl_from_IRandomAccessStreamReference( (IRandomAccessStreamReference *)invoker );

    TRACE( "invoker %p, result %p\n", invoker, result );

    if (!(stream = calloc( 1, sizeof(*stream) ))) return E_OUTOFMEMORY;

    stream->IRandomAccessStream_iface.lpVtbl = &random_access_stream_vtbl;
    stream->IClosable_iface.lpVtbl = &closable_stream_vtbl;
    stream->streamSize = reference->streamSize;
    stream->closableRef = 1;
    stream->Position = 0;
    stream->CanRead = reference->canRead;
    stream->CanWrite = reference->canWrite;
    stream->ref = 1;

    if ( reference->canRead && reference->canWrite )
    {
        stream->stream = CreateFileW( WindowsGetStringRawBuffer( reference->handlePath, NULL ), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    } else if ( reference->canRead )
    {
        stream->stream = CreateFileW( WindowsGetStringRawBuffer( reference->handlePath, NULL ), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    } else if ( reference->canWrite )
    {
        stream->stream = CreateFileW( WindowsGetStringRawBuffer( reference->handlePath, NULL ), GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    }

    if ( SUCCEEDED( status ) ) 
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)&stream->IRandomAccessStream_iface;
    }

    return status;
}