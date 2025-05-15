/* WinRT Windows.Storage.Streams.RandomAccessStream Implementation
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

#include "RandomAccessStreamInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( buffer_uint32_async_with_progress, IAsyncOperationWithProgressCompletedHandler_IBuffer_UINT32, IAsyncOperationWithProgress_IBuffer_UINT32 )

HRESULT WINAPI random_access_stream_statics_Copy( IInputStream *source, IOutputStream *destination, UINT64 bytesToCopy, IAsyncOperationWithProgress_UINT64_UINT64 **operation )
{
    HRESULT status = S_OK;
    UINT32 bytesReadInSegment = BUFFER_SIZE;
    UINT32 bytesRead = 0;    
    DWORD asyncResult;
    BYTE *byteBuffer;
    BYTE *tmpByteBuffer;

    IBuffer *buffer;
    IBuffer *readBuffer;
    IBuffer *segmentBuffer;
    IBufferFactory *bufferFactory;
    IBufferByteAccess *segmentBufferByteAccess;
    IBufferByteAccess *readBufferByteAccess;
    IAsyncOperationWithProgress_IBuffer_UINT32 *buffer_uint32_async_with_progress_operation;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_Buffer, bufferFactory, IID_IBufferFactory );

    TRACE( "source %p, destination %p\n", source, destination );

    byteBuffer = (BYTE *)malloc(BUFFER_SIZE);

    if ( bytesToCopy )
    {
        IBufferFactory_Create( bufferFactory, bytesToCopy, &buffer );
        
        status = IInputStream_ReadAsync( source, buffer, bytesToCopy, InputStreamOptions_None, &buffer_uint32_async_with_progress_operation );
        if( FAILED( status ) ) goto _FAIL;

        asyncResult = await_IAsyncOperationWithProgress_IBuffer_UINT32( buffer_uint32_async_with_progress_operation, INFINITE );
        if ( asyncResult ) goto _FAIL;

        status = IAsyncOperationWithProgress_IBuffer_UINT32_GetResults( buffer_uint32_async_with_progress_operation, &readBuffer );
        if( FAILED( status ) ) goto _FAIL;
    } else
    {
        //Copy in 4096 byte segments until bytes read is not 4096.
        while ( bytesReadInSegment == BUFFER_SIZE )
        {
            IBufferFactory_Create( bufferFactory, BUFFER_SIZE, &buffer );

            status = IInputStream_ReadAsync( source, buffer, BUFFER_SIZE, InputStreamOptions_None, &buffer_uint32_async_with_progress_operation );
            if( FAILED( status ) ) goto _FAIL;

            asyncResult = await_IAsyncOperationWithProgress_IBuffer_UINT32( buffer_uint32_async_with_progress_operation, INFINITE );
            if ( asyncResult ) goto _FAIL;

            status = IAsyncOperationWithProgress_IBuffer_UINT32_GetResults( buffer_uint32_async_with_progress_operation, &segmentBuffer );
            if( FAILED( status ) ) goto _FAIL;

            IBuffer_get_Length( segmentBuffer, &bytesReadInSegment );

            if ( bytesReadInSegment == 0 ) break;

            status = IBuffer_QueryInterface( segmentBuffer, &IID_IBufferByteAccess, (void **)&segmentBufferByteAccess );
            if( FAILED( status ) ) goto _FAIL;

            IBufferByteAccess_Buffer( segmentBufferByteAccess, &tmpByteBuffer );

            byteBuffer = (BYTE *)realloc(byteBuffer, bytesRead + bytesReadInSegment);
            memcpy( byteBuffer + bytesRead, tmpByteBuffer, bytesReadInSegment );
            bytesRead += bytesReadInSegment;
        }

        IBufferFactory_Create( bufferFactory, bytesRead, &readBuffer );

        status = IBuffer_QueryInterface( readBuffer, &IID_IBufferByteAccess, (void **)&readBufferByteAccess );
        if( FAILED( status ) ) goto _FAIL;

        IBufferByteAccess_Buffer( readBufferByteAccess, &tmpByteBuffer );
        memcpy( tmpByteBuffer, byteBuffer, bytesRead );

        IBuffer_put_Length( readBuffer, bytesRead );
    }

    status = IOutputStream_WriteAsync( destination, readBuffer, (IAsyncOperationWithProgress_UINT32_UINT32 **)operation );

    _FAIL:
        IBuffer_Release( buffer );
        IBuffer_Release( readBuffer );
        IBuffer_Release( segmentBuffer );
        CoTaskMemFree( tmpByteBuffer );
        CoTaskMemFree( byteBuffer );
        CHECK_LAST_RESTRICTED_ERROR();
        return status;
}