/* WinRT InputOutputStream Implementation
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

#include "InputOutputStreamInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( currentOperation_async, IAsyncOperationWithProgressCompletedHandler_UINT32_UINT32, IAsyncOperationWithProgress_UINT32_UINT32 )

HRESULT WINAPI input_stream_Read( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress )
{    
    LARGE_INTEGER li;
    STATSTG stat;
    HRESULT status = S_OK;    
    UINT32 totalBytesRead = 0;
    UINT32 bufferCapacity;
    UINT64 streamSize;
    ULONG bytesRead;    
    BYTE *buffer;
    BYTE tmpBuffer[BUFFER_SIZE];

    IBufferByteAccess *bufferByteAccess;

    struct input_stream *stream = impl_from_IInputStream( (IInputStream *)invoker );

    /**
     * Parameters
     */
    struct input_stream_options *options = (struct input_stream_options *)param;

    TRACE( "invoker %p, result %p\n", invoker, result );

    status = IStream_Stat( stream->stream, &stat, STATFLAG_NONAME );
    if ( FAILED( status ) ) return status;

    streamSize = stat.cbSize.QuadPart;

    if ( streamSize <= 0 )
        return E_BOUNDS;

    //Update stream position (Workaround for when the client uses both streams at the same time)
    li.QuadPart = stream->updatePos;
    IStream_Seek( stream->stream, li, STREAM_SEEK_SET, NULL );
    
    IBuffer_get_Capacity( options->buffer, &bufferCapacity );

    if ( bufferCapacity < options->count )
        return E_BOUNDS;

    status = IBuffer_QueryInterface( options->buffer, &IID_IBufferByteAccess, (void **)&bufferByteAccess );
    if ( FAILED( status ) ) return status;

    IBufferByteAccess_Buffer( bufferByteAccess, &buffer );

    while ( totalBytesRead < streamSize )
    {
        //IStream_Read is supposed to break after "tmpBuffer" reaches null,
        //but apparently this isn't implemented in wine.
        if ( (streamSize - totalBytesRead) < BUFFER_SIZE )
        {
            status = IStream_Read( stream->stream, (LPVOID)tmpBuffer, streamSize - totalBytesRead, &bytesRead );
            if ( FAILED( status ) ) return status;
        } else {
            status = IStream_Read( stream->stream, (LPVOID)tmpBuffer, BUFFER_SIZE, &bytesRead );
            if ( FAILED( status ) ) return status;
        }

        if ( bytesRead == 0 ) break;

        memcpy( buffer + totalBytesRead, tmpBuffer, bytesRead );

        totalBytesRead += bytesRead;

        //Progress handlers are optional.
        if ( progress )
        {
            status = IWineAsyncOperationProgressHandler_Invoke( progress, (IInspectable *)invoker, totalBytesRead );
            if ( FAILED( status ) ) return status;
        }
    }

    IBuffer_put_Length( options->buffer, totalBytesRead );

    if ( stream->headPosition )
        *stream->headPosition += totalBytesRead;

    stream->updatePos += totalBytesRead;

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)options->buffer;
    }

    return status;
}

HRESULT WINAPI output_stream_Write( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress )
{
    LARGE_INTEGER li;
    HRESULT status = S_OK;
    UINT64 totalBytesWritten = 0;
    UINT32 totalBytesToWrite = 0;
    ULONG bytesWritten;
    BYTE *buffer;

    IBuffer *bufferToWrite = (IBuffer *)param;
    IBufferByteAccess *bufferByteAccess;

    struct output_stream *stream = impl_from_IOutputStream( (IOutputStream *)invoker );

    TRACE( "invoker %p, result %p\n", invoker, result );

    status = IBuffer_QueryInterface( bufferToWrite, &IID_IBufferByteAccess, (void **)&bufferByteAccess );
    if ( FAILED( status ) ) return status;

    IBufferByteAccess_Buffer( bufferByteAccess, &buffer );

    IBuffer_get_Length( bufferToWrite, &totalBytesToWrite );

    //Update stream position (Workaround for when the client uses both streams at the same time)
    li.QuadPart = stream->updatePos;
    IStream_Seek( stream->stream, li, STREAM_SEEK_SET, NULL );

    while ( totalBytesWritten < totalBytesToWrite )
    {
        //IStream_Write is supposed to break after "tmpBuffer" reaches null,
        //but apparently this isn't implemented in wine.
        if ( (totalBytesToWrite - totalBytesWritten) < BUFFER_SIZE )
        {
            status = IStream_Write( stream->stream, (LPCVOID)(buffer + totalBytesWritten), totalBytesToWrite - totalBytesWritten, &bytesWritten );
            if ( FAILED( status ) ) return status;
        } else {
            status = IStream_Write( stream->stream, (LPCVOID)(buffer + totalBytesWritten), BUFFER_SIZE, &bytesWritten );
            if ( FAILED( status ) ) return status;
        }

        if ( bytesWritten == 0 ) break;

        totalBytesWritten += bytesWritten;

        //Progress handlers are optional.
        if ( progress )
        {
            status = IWineAsyncOperationProgressHandler_Invoke( progress, (IInspectable *)invoker, totalBytesWritten );
            if ( FAILED( status ) ) return status;
        }
    }

    if ( stream->headPosition )
        *stream->headPosition += totalBytesWritten;

    stream->updatePos += totalBytesWritten;

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UI4;
        result->ulVal = totalBytesWritten;
    }

    stream->currentOperation = NULL;

    return status;
}

HRESULT WINAPI output_stream_Flush( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    DWORD async_result;
    IAsyncInfo *info;
    AsyncStatus status;

    struct output_stream *stream = impl_from_IOutputStream( (IOutputStream *)invoker );

    TRACE( "invoker %p, result %p\n", invoker, result );

    if ( stream->currentOperation )
    {
        IAsyncOperationWithProgress_UINT32_UINT32_QueryInterface( stream->currentOperation, &IID_IAsyncInfo, (void **)&info );
        IAsyncInfo_get_Status( info, &status );

        if ( status == Started )
        {
            async_result = await_IAsyncOperationWithProgress_UINT32_UINT32( stream->currentOperation, INFINITE );
            if ( !async_result )
            {
                result->vt = VT_BOOL;
                result->boolVal = FALSE;
                return E_UNEXPECTED;
            }
        }
    }

    result->vt = VT_BOOL;
    result->boolVal = TRUE;

    return S_OK;
}