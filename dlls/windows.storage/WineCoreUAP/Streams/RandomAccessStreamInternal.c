/* WinRT Windows.Storage.Streams.RandomAccessStream Implementation
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
#include "RandomAccessStreamInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( buffer_uint32_async_with_progress, IAsyncOperationWithProgressCompletedHandler_IBuffer_UINT32, IAsyncOperationWithProgress_IBuffer_UINT32 )

HRESULT WINAPI random_access_stream_statics_Copy( IInputStream *source, IOutputStream *destination, UINT64 bytesToCopy, IAsyncOperationWithProgress_UINT64_UINT64 **operation )
{
    HRESULT status = S_OK;
    DWORD asyncResult;

    IBuffer *buffer;
    IBuffer *readBuffer;
    IAsyncOperationWithProgress_IBuffer_UINT32 *buffer_uint32_async_with_progress_operation;

    TRACE( "source %p, destination %p\n", source, destination );

    buffer_Create( bytesToCopy, &buffer );

    status = IInputStream_ReadAsync( source, buffer, bytesToCopy, InputStreamOptions_None, &buffer_uint32_async_with_progress_operation );
    if( FAILED( status ) ) return status;

    asyncResult = await_IAsyncOperationWithProgress_IBuffer_UINT32( buffer_uint32_async_with_progress_operation, INFINITE );
    if ( !asyncResult ) return E_ABORT;

    status = IAsyncOperationWithProgress_IBuffer_UINT32_GetResults( buffer_uint32_async_with_progress_operation, &readBuffer );
    if( FAILED( status ) ) return status;

    //This is not a good idea but it works.
    return IOutputStream_WriteAsync( destination, readBuffer, (IAsyncOperationWithProgress_UINT32_UINT32 **)operation );
}