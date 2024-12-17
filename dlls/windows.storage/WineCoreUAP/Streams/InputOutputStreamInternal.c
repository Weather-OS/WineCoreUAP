/* WinRT InputOutputStream Implementation
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

#include "InputOutputStreamInternal.h"

DEFINE_ASYNC_COMPLETED_HANDLER( currentOperation_async, IAsyncOperationWithProgressCompletedHandler_UINT32_UINT32, IAsyncOperationWithProgress_UINT32_UINT32 )

HRESULT WINAPI input_stream_Read( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress )
{
    HRESULT status = S_OK;    
    UINT32 totalBytesRead = 0;
    UINT32 bufferCapacity;
    DWORD bytesRead;    
    BYTE *buffer;
    BYTE tmpBuffer[BUFFER_SIZE];
    BOOL readResult;

    IBufferByteAccess *bufferByteAccess;

    struct input_stream *stream = impl_from_IInputStream( (IInputStream *)invoker );

    /**
     * Paramteres
     */
    struct input_stream_options *options = (struct input_stream_options *)result;

    if ( stream->streamSize <= 0 )
        return E_BOUNDS;

    if ( options->count >= stream->streamSize )
        return E_BOUNDS;

    IBuffer_get_Capacity( options->buffer, &bufferCapacity );

    if ( bufferCapacity <= options->count )
        return E_BOUNDS;
    
    status = IBuffer_QueryInterface( options->buffer, &IID_IBufferByteAccess, (void **)&bufferByteAccess );
    if ( FAILED( status ) ) return status;

    IBufferByteAccess_get_Buffer( bufferByteAccess, &buffer );

    while ( totalBytesRead < stream->streamSize )
    {
        readResult = ReadFile( stream->stream, tmpBuffer, BUFFER_SIZE, &bytesRead, NULL );

        if ( !readResult )
            return E_ABORT;

        if ( bytesRead == 0 ) break;

        memcpy( buffer + totalBytesRead, tmpBuffer, bytesRead );

        totalBytesRead += bytesRead;

        status = IWineAsyncOperationProgressHandler_Invoke( progress, (IInspectable *)invoker, totalBytesRead );
        if ( FAILED( status ) ) return status;
    }

    IBuffer_put_Length( options->buffer, totalBytesRead );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)options->buffer;
    }

    return status;
}

HRESULT WINAPI output_stream_Write( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress )
{
    HRESULT status = S_OK;
    UINT32 totalBytesWritten = 0;
    UINT32 totalBytesToWrite = 0;
    DWORD bytesWritten;
    BYTE *buffer;
    BOOL writeResult;

    IBuffer *bufferToWrite = (IBuffer *)param;
    IBufferByteAccess *bufferByteAccess;

    struct output_stream *stream = impl_from_IOutputStream( (IOutputStream *)invoker );

    status = IBuffer_QueryInterface( bufferToWrite, &IID_IBufferByteAccess, (void **)&bufferByteAccess );
    if ( FAILED( status ) ) return status;

    IBufferByteAccess_get_Buffer( bufferByteAccess, &buffer );

    IBuffer_get_Length( bufferToWrite, &totalBytesToWrite );

    while ( totalBytesWritten < totalBytesToWrite )
    {
        writeResult = WriteFile( stream->stream, buffer + totalBytesWritten, BUFFER_SIZE, &bytesWritten, NULL );

        if ( !writeResult )
            return E_ABORT;

        if ( bytesWritten == 0 ) break;

        totalBytesWritten += bytesWritten;

        status = IWineAsyncOperationProgressHandler_Invoke( progress, (IInspectable *)invoker, totalBytesWritten );
        if ( FAILED( status ) ) return status;
    }

    stream->streamSize = totalBytesWritten;

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UI4;
        result->ulVal = totalBytesWritten;
    }

    return status;
}

HRESULT WINAPI output_stream_Flush( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    DWORD async_result;
    IAsyncInfo *info;
    AsyncStatus status;

    struct output_stream *stream = impl_from_IOutputStream( (IOutputStream *)invoker );

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