/* WinRT Windows.Storage.FileIO Implementation
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

#include "FileIOInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( async_random_access_stream_handler, IAsyncOperationCompletedHandler_IRandomAccessStream, IAsyncOperation_IRandomAccessStream )
DEFINE_ASYNC_COMPLETED_HANDLER( async_uint32_handler, IAsyncOperationCompletedHandler_UINT32, IAsyncOperation_UINT32 )
DEFINE_ASYNC_COMPLETED_HANDLER( async_boolean_handler, IAsyncOperationCompletedHandler_boolean, IAsyncOperation_boolean )
DEFINE_ASYNC_COMPLETED_HANDLER( async_hstring_handler, IAsyncOperationCompletedHandler_HSTRING, IAsyncOperation_HSTRING )

static HRESULT WINAPI
AutoDetectEncoding(
    IN IStorageFile *file,
    OUT UnicodeEncoding *encoding
) {
    HRESULT status = S_OK;
    BOOLEAN canRead;
    UINT64 fileSize;
    DWORD asyncRes;
    BYTE byteOrder[3];
    BYTE UTF16LEBOM[] = { 0xFF, 0xFE };
    BYTE UTF16BEBOM[] = { 0xFE, 0xFF };
    BYTE UTF8BOM[] = { 0xEF, 0xBB, 0xBF };

    IAsyncOperation_IRandomAccessStream *readStreamOperation = NULL;
    IAsyncOperation_UINT32 *loadOperation = NULL;
    IRandomAccessStream *readStream = NULL;
    IDataReaderFactory *dataReaderFactory = NULL;
    IInputStream *inputStream = NULL;
    IDataReader *dataReader = NULL;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataReader, dataReaderFactory, IID_IDataReaderFactory );
    
    TRACE( "file %p, encoding %p\n", file, encoding );

    status = IStorageFile_OpenAsync( file, FileAccessMode_Read, &readStreamOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_IRandomAccessStream( readStreamOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_IRandomAccessStream_GetResults( readStreamOperation, &readStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    IRandomAccessStream_get_CanRead( readStream, &canRead );
    if ( !canRead )
    {
        status = E_ACCESSDENIED;
        goto _CLEANUP;
    }

    IRandomAccessStream_get_Size( readStream, &fileSize );
    if ( fileSize < 2 )
    {
        status = E_FAIL;
        goto _CLEANUP;
    }

    status = IRandomAccessStream_GetInputStreamAt( readStream, 0, &inputStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataReaderFactory_CreateDataReader( dataReaderFactory, inputStream, &dataReader );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataReader_LoadAsync( dataReader, 3, &loadOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_UINT32( loadOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    if ( fileSize >= 3 )
    {
        status = IDataReader_ReadBytes( dataReader, 3, byteOrder );
        if ( FAILED( status ) ) goto _CLEANUP;
    } else
    {
        status = IDataReader_ReadBytes( dataReader, 2, byteOrder );
        if ( FAILED( status ) ) goto _CLEANUP;
    }

    if ( memcmp( byteOrder, UTF16LEBOM, 2 ) == 0 )
        *encoding = UnicodeEncoding_Utf16LE;
    else if ( memcmp( byteOrder, UTF16BEBOM, 2 ) == 0 )
        *encoding = UnicodeEncoding_Utf16BE;
    else if ( fileSize >= 3 && memcmp( byteOrder, UTF8BOM, 3 ) == 0 )
        *encoding = UnicodeEncoding_Utf8;
    else
        status = E_FAIL;

_CLEANUP:
    if ( readStream )
        IRandomAccessStream_Release( readStream );
    if ( readStreamOperation )
        IAsyncOperation_IRandomAccessStream_Release( readStreamOperation );
    if ( loadOperation )
        IAsyncOperation_UINT32_Release( loadOperation );
    if ( inputStream )
        IInputStream_Release( inputStream );
    if ( dataReader )
        IDataReader_Release( dataReader );
    if ( dataReaderFactory )
        IDataReaderFactory_Release( dataReaderFactory );

    return status;
}

HRESULT WINAPI 
file_io_statics_ReadText( 
    IUnknown *invoker, 
    IUnknown *param, 
    PROPVARIANT *result 
) {
    HRESULT status = S_OK;
    BOOLEAN canRead;
    HSTRING readData = NULL;
    LPCWSTR readDataStr = NULL;
    UINT32 readDataStrLen;
    UINT64 fileSize;
    DWORD asyncRes;

    IAsyncOperation_IRandomAccessStream *readStreamOperation = NULL;
    IAsyncOperation_UINT32 *loadOperation = NULL;
    IRandomAccessStream *readStream = NULL;
    IDataReaderFactory *dataReaderFactory = NULL;
    IInputStream *inputStream = NULL;
    IDataReader *dataReader = NULL;

    struct file_io_read_text_options *read_text_options = (struct file_io_read_text_options *)param;

    //Parameters    
    UnicodeEncoding unicodeEncoding = read_text_options->encoding;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataReader, dataReaderFactory, IID_IDataReaderFactory );

    TRACE( "iface %p, value %p\n", invoker, result );

    // Windows prioritizes integrity over client choice.
    // UnicodeEncoding is overwritten if a different encoding is detected.
    AutoDetectEncoding( read_text_options->file, &unicodeEncoding );

    status = IStorageFile_OpenAsync( read_text_options->file, FileAccessMode_Read, &readStreamOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_IRandomAccessStream( readStreamOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_IRandomAccessStream_GetResults( readStreamOperation, &readStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    IRandomAccessStream_get_CanRead( readStream, &canRead );
    if ( !canRead )
    {
        status = E_ACCESSDENIED;
        goto _CLEANUP;
    }

    IRandomAccessStream_get_Size( readStream, &fileSize );
    if ( fileSize > __UINT32_MAX__ /* ~4 Gigabytes */)
    {
        ERR( "fileSize for stream %p exceeds the uint32 limit, which is outside the range of an HSTRING.\n", readStream );
        status = E_BOUNDS;
        goto _CLEANUP;
    }

    status = IRandomAccessStream_GetInputStreamAt( readStream, 0, &inputStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataReaderFactory_CreateDataReader( dataReaderFactory, inputStream, &dataReader );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataReader_put_UnicodeEncoding( dataReader, unicodeEncoding );
    if ( FAILED( status ) ) goto _CLEANUP;

    // We have verified that fileSize doesn't exceed the uint32 limit.
    status = IDataReader_LoadAsync( dataReader, (UINT32)fileSize, &loadOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_UINT32( loadOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IDataReader_ReadString( dataReader, (UINT32)(fileSize / sizeof(WCHAR)), &readData );
    if ( FAILED( status ) ) goto _CLEANUP;

    if ( SUCCEEDED( status ) )
    {
        readDataStr = WindowsGetStringRawBuffer( readData, &readDataStrLen );

        result->pwszVal = CoTaskMemAlloc( (readDataStrLen + 1) * sizeof(WCHAR) );
        if ( !result->pwszVal )
        {
            status = E_OUTOFMEMORY;
            goto _CLEANUP;
        }
        
        result->vt = VT_LPWSTR;
        CopyMemory( result->pwszVal, readDataStr, readDataStrLen * sizeof(WCHAR) );
        result->pwszVal[readDataStrLen] = L'\0';
    }

_CLEANUP:
    if ( readData )
        WindowsDeleteString( readData );
    if ( readStream )
        IRandomAccessStream_Release( readStream );
    if ( readStreamOperation )
        IAsyncOperation_IRandomAccessStream_Release( readStreamOperation );
    if ( loadOperation )
        IAsyncOperation_UINT32_Release( loadOperation );
    if ( inputStream )
        IInputStream_Release( inputStream );
    if ( dataReader )
        IDataReader_Release( dataReader );
    if ( dataReaderFactory )
        IDataReaderFactory_Release( dataReaderFactory );
    free( read_text_options );

    return status;
}

HRESULT WINAPI 
file_io_statics_WriteText( 
    IUnknown *invoker, 
    IUnknown *param, 
    PROPVARIANT *result 
) {
    HRESULT status = S_OK;
    BOOLEAN canWrite;
    UINT32 bytesWritten;
    UINT32 stringWriteLen;
    DWORD asyncRes;
    BYTE UTF16LEBOM[] = { 0xFF, 0xFE };
    BYTE UTF16BEBOM[] = { 0xFE, 0xFF };
    BYTE UTF8BOM[] = { 0xEF, 0xBB, 0xBB };

    IAsyncOperation_IRandomAccessStream *writeStreamOperation = NULL;
    IAsyncOperation_boolean *flushOperation = NULL;
    IAsyncOperation_UINT32 *storeOperation = NULL;
    IRandomAccessStream *writeStream = NULL;
    IDataWriterFactory *dataWriterFactory = NULL;
    IOutputStream *outputStream = NULL;
    IDataWriter *dataWriter = NULL;

    struct file_io_write_text_options *write_text_options = (struct file_io_write_text_options *)param;

    //Parameters    
    UnicodeEncoding unicodeEncoding = write_text_options->encoding;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataWriter, dataWriterFactory, IID_IDataWriterFactory );

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFile_OpenAsync( write_text_options->file, FileAccessMode_ReadWrite, &writeStreamOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_IRandomAccessStream( writeStreamOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_IRandomAccessStream_GetResults( writeStreamOperation, &writeStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    IRandomAccessStream_get_CanWrite( writeStream, &canWrite );
    if ( !canWrite )
    {
        status = E_ACCESSDENIED;
        goto _CLEANUP;
    }

    // Clear out the file
    status = IRandomAccessStream_put_Size( writeStream, 0 );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IRandomAccessStream_GetOutputStreamAt( writeStream, 0, &outputStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriterFactory_CreateDataWriter( dataWriterFactory, outputStream, &dataWriter );
    if ( FAILED( status ) ) goto _CLEANUP;

    // Writing the BOM headers
    if ( unicodeEncoding == UnicodeEncoding_Utf16LE )
    {
        status = IDataWriter_WriteBytes( dataWriter, 2, UTF16LEBOM );
        if ( FAILED( status ) ) goto _CLEANUP;
    } else if ( unicodeEncoding == UnicodeEncoding_Utf16BE )
    {
        status = IDataWriter_WriteBytes( dataWriter, 2, UTF16BEBOM );
        if ( FAILED( status ) ) goto _CLEANUP;
    } else if ( write_text_options->withEncoding )
    {
        // A UTF-8 BOM header is forcefully appended when using the routine `FileIO::WriteTextWithEncoding`
        //  when using `UnicodeEncoding::Utf8`
        status = IDataWriter_WriteBytes( dataWriter, 3, UTF8BOM );
        if ( FAILED( status ) ) goto _CLEANUP;
    }

    status = IDataWriter_put_UnicodeEncoding( dataWriter, unicodeEncoding );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_WriteString( dataWriter, write_text_options->contents, &stringWriteLen );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_StoreAsync( dataWriter, &storeOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_UINT32( storeOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_UINT32_GetResults( storeOperation, &bytesWritten );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_FlushAsync( dataWriter, &flushOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_boolean( flushOperation, INFINITE );
    if ( asyncRes ) status = E_UNEXPECTED;

_CLEANUP:
    if ( writeStream )
        IRandomAccessStream_Release( writeStream );
    if ( writeStreamOperation )
        IAsyncOperation_IRandomAccessStream_Release( writeStreamOperation );
    if ( storeOperation )
        IAsyncOperation_UINT32_Release( storeOperation );
    if ( flushOperation )
        IAsyncOperation_boolean_Release( flushOperation );
    if ( outputStream )
        IOutputStream_Release( outputStream );
    if ( dataWriter )
        IDataWriter_Release( dataWriter );
    if ( dataWriterFactory )
        IDataWriterFactory_Release( dataWriterFactory );
    free( write_text_options );

    return status;
}

HRESULT WINAPI 
file_io_statics_AppendText( 
    IUnknown *invoker, 
    IUnknown *param, 
    PROPVARIANT *result 
) {
    HRESULT status = S_OK;
    BOOLEAN canWrite;
    BOOLEAN flushResult;
    UINT64 fileSize;
    UINT32 bytesWritten;
    UINT32 stringWriteLen;
    DWORD asyncRes;

    IAsyncOperation_IRandomAccessStream *writeStreamOperation = NULL;
    IAsyncOperation_boolean *flushOperation = NULL;
    IAsyncOperation_UINT32 *storeOperation = NULL;
    IRandomAccessStream *writeStream = NULL;
    IDataWriterFactory *dataWriterFactory = NULL;
    IOutputStream *outputStream = NULL;
    IDataWriter *dataWriter = NULL;

    struct file_io_write_text_options *write_text_options = (struct file_io_write_text_options *)param;

    //Parameters    
    UnicodeEncoding unicodeEncoding = write_text_options->encoding;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataWriter, dataWriterFactory, IID_IDataWriterFactory);

    TRACE( "iface %p, value %p\n", invoker, result );

    // Unlike read related functions, the file's encoding doesn't take priority here.
    if ( !write_text_options->withEncoding )
        AutoDetectEncoding( write_text_options->file, &unicodeEncoding );


    status = IStorageFile_OpenAsync( write_text_options->file, FileAccessMode_ReadWrite, &writeStreamOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_IRandomAccessStream( writeStreamOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_IRandomAccessStream_GetResults( writeStreamOperation, &writeStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    IRandomAccessStream_get_CanWrite( writeStream, &canWrite );
    if ( !canWrite )
    {
        status = E_ACCESSDENIED;
        goto _CLEANUP;
    }

    status = IRandomAccessStream_get_Size( writeStream, &fileSize);
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IRandomAccessStream_GetOutputStreamAt( writeStream, fileSize, &outputStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriterFactory_CreateDataWriter( dataWriterFactory, outputStream, &dataWriter );
    if ( FAILED( status ) ) goto _CLEANUP;

    // Notes: `FileIO::AppendTextAsync` does not append BOM headers on an empty file.

    status = IDataWriter_put_UnicodeEncoding( dataWriter, unicodeEncoding );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_WriteString( dataWriter, write_text_options->contents, &stringWriteLen );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_StoreAsync( dataWriter, &storeOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_UINT32( storeOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_UINT32_GetResults( storeOperation, &bytesWritten );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_FlushAsync( dataWriter, &flushOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_boolean( flushOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_boolean_GetResults( flushOperation, &flushResult );
    if ( FAILED( status ) ) goto _CLEANUP;
    if ( !flushResult ) status = E_FAIL;

_CLEANUP:    
    if ( dataWriterFactory )
        IDataWriterFactory_Release( dataWriterFactory );
    if ( dataWriter )
        IDataWriter_Release( dataWriter );
    if ( outputStream )
        IOutputStream_Release( outputStream );    
    if ( writeStream )
        IRandomAccessStream_Release( writeStream );    
    if ( writeStreamOperation )
        IAsyncOperation_IRandomAccessStream_Release( writeStreamOperation );
    if ( storeOperation )
        IAsyncOperation_UINT32_Release( storeOperation );
    if ( flushOperation )
        IAsyncOperation_boolean_Release( flushOperation );
    free( write_text_options );

    return status;
}

HRESULT WINAPI 
file_io_statics_ReadLines( 
    IUnknown *invoker, 
    IUnknown *param, 
    PROPVARIANT *result 
) {
    HRESULT status = S_OK;
    HSTRING fileContents;
    HSTRING currentLine;
    LPCWSTR fileContentsBuffer;
    UINT32 fileContentsBufferSize;
    UINT32 iterator;
    UINT32 lineIndexStart;
    UINT32 lineIndexEnd;
    DWORD asyncRes;
    WCHAR currentWChar;
    
    IAsyncOperation_HSTRING *textReadOperation = NULL;
    IVector_HSTRING *vector = NULL;

    struct file_io_read_text_options *read_text_options = (struct file_io_read_text_options *)param;

    //Parameters    
    UnicodeEncoding unicodeEncoding = read_text_options->encoding;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = hstring_vector_create( &vector );
    if ( FAILED( status ) ) goto _CLEANUP;

    //Read the file first, then we'll split the packed HSTRING into individual lines.
    status = IFileIOStatics_ReadTextWithEncodingAsync( (IFileIOStatics *)invoker, read_text_options->file, unicodeEncoding, &textReadOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_HSTRING( textReadOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_HSTRING_GetResults( textReadOperation, &fileContents );
    if ( FAILED( status ) ) goto _CLEANUP;

    fileContentsBuffer = WindowsGetStringRawBuffer( fileContents, &fileContentsBufferSize );

    lineIndexStart = 0;

    if ( fileContentsBufferSize != 0 )
    {
        for ( iterator = 0; iterator <= fileContentsBufferSize; iterator++ )
        {
            if ( iterator < fileContentsBufferSize ) 
                currentWChar = fileContentsBuffer[iterator];
            else
                currentWChar = L'\0';

            if ( currentWChar == L'\r' || currentWChar == L'\n' || currentWChar == L'\0' )
            {
                lineIndexEnd = iterator;
                if ( lineIndexEnd > lineIndexStart )
                {
                    status = WindowsCreateString( 
                        fileContentsBuffer + lineIndexStart, 
                        lineIndexEnd - lineIndexStart,
                        &currentLine
                    );
                    if ( FAILED( status ) ) goto _CLEANUP;
                } else 
                    WindowsCreateString( NULL, 0, &currentLine );

                status = IVector_HSTRING_Append( vector, currentLine );
                if ( FAILED( status ) ) goto _CLEANUP;

                WindowsDeleteString( currentLine );

                //skip crlf
                if ( currentWChar == L'\r' && ( iterator + 1 ) < fileContentsBufferSize && fileContentsBuffer[ iterator + 1 ] == L'\n' )
                    iterator++; 

                lineIndexStart = iterator + 1;
            }
        }
    }

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)vector;
    }

_CLEANUP:
    if ( FAILED( status ) )
        if ( vector )
            IVector_HSTRING_Release( vector );
    if ( textReadOperation )
        IAsyncOperation_HSTRING_Release( textReadOperation );
    WindowsDeleteString( currentLine );
    free( read_text_options );

    return status;
}

HRESULT WINAPI file_io_statics_ReadBuffer( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    BOOLEAN canRead;
    UINT64 fileSize;
    DWORD asyncRes;

    IAsyncOperation_IRandomAccessStream *readStreamOperation = NULL;
    IAsyncOperation_UINT32 *loadOperation = NULL;
    IRandomAccessStream *readStream = NULL;
    IDataReaderFactory *dataReaderFactory = NULL;
    IInputStream *inputStream = NULL;
    IDataReader *dataReader = NULL;
    IBuffer *buffer = NULL;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataReader, dataReaderFactory, IID_IDataReaderFactory );

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFile_OpenAsync( (IStorageFile *)param, FileAccessMode_Read, &readStreamOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_IRandomAccessStream( readStreamOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_IRandomAccessStream_GetResults( readStreamOperation, &readStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    IRandomAccessStream_get_CanRead( readStream, &canRead );
    if ( !canRead )
    {
        status = E_ACCESSDENIED;
        goto _CLEANUP;
    }

    IRandomAccessStream_get_Size( readStream, &fileSize );
    if ( fileSize > __UINT32_MAX__ /* ~4 Gigabytes */)
    {
        ERR( "fileSize for stream %p exceeds the uint32 limit, which is outside the range of an IBuffer.\n", readStream );
        status = E_BOUNDS;
        goto _CLEANUP;
    }

    status = IRandomAccessStream_GetInputStreamAt( readStream, 0, &inputStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataReaderFactory_CreateDataReader( dataReaderFactory, inputStream, &dataReader );
    if ( FAILED( status ) ) goto _CLEANUP;

    // We have verified that fileSize doesn't exceed the uint32 limit.
    status = IDataReader_LoadAsync( dataReader, (UINT32)fileSize, &loadOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_UINT32( loadOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IDataReader_ReadBuffer( dataReader, (UINT32)fileSize, &buffer );
    if ( FAILED( status ) ) goto _CLEANUP;

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)buffer;
    }

_CLEANUP:
    if ( FAILED( status ) )
        if ( buffer )
            IBuffer_Release( buffer );
    if ( readStream )
        IRandomAccessStream_Release( readStream );
    if ( readStreamOperation )
        IAsyncOperation_IRandomAccessStream_Release( readStreamOperation );
    if ( loadOperation )
        IAsyncOperation_UINT32_Release( loadOperation );
    if ( inputStream )
        IInputStream_Release( inputStream );
    if ( dataReader )
        IDataReader_Release( dataReader );
    if ( dataReaderFactory )
        IDataReaderFactory_Release( dataReaderFactory );

    return status;
}

HRESULT WINAPI file_io_statics_WriteBuffer( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    BOOLEAN canWrite;
    UINT32 bytesWritten;
    UINT32 bufferLen;
    DWORD asyncRes;

    IAsyncOperation_IRandomAccessStream *writeStreamOperation = NULL;
    IAsyncOperation_boolean *flushOperation = NULL;
    IAsyncOperation_UINT32 *storeOperation = NULL;
    IRandomAccessStream *writeStream = NULL;
    IDataWriterFactory *dataWriterFactory = NULL;
    IOutputStream *outputStream = NULL;
    IDataWriter *dataWriter = NULL;

    struct file_io_write_buffer_options *write_buffer_options = (struct file_io_write_buffer_options *)param;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataWriter, dataWriterFactory, IID_IDataWriterFactory );

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFile_OpenAsync( write_buffer_options->file, FileAccessMode_ReadWrite, &writeStreamOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_IRandomAccessStream( writeStreamOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_IRandomAccessStream_GetResults( writeStreamOperation, &writeStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    IRandomAccessStream_get_CanWrite( writeStream, &canWrite );
    if ( !canWrite )
    {
        status = E_ACCESSDENIED;
        goto _CLEANUP;
    }

    // Clear out the file
    status = IRandomAccessStream_put_Size( writeStream, 0 );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IRandomAccessStream_GetOutputStreamAt( writeStream, 0, &outputStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriterFactory_CreateDataWriter( dataWriterFactory, outputStream, &dataWriter );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_WriteBuffer( dataWriter, write_buffer_options->buffer );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_StoreAsync( dataWriter, &storeOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_UINT32( storeOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_UINT32_GetResults( storeOperation, &bytesWritten );
    if ( FAILED( status ) ) goto _CLEANUP;

    IBuffer_get_Length( write_buffer_options->buffer, &bufferLen );

    if ( bytesWritten != bufferLen )
    {
        status = E_FAIL;
        goto _CLEANUP;
    }

    status = IDataWriter_FlushAsync( dataWriter, &flushOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_boolean( flushOperation, INFINITE );
    if ( asyncRes ) status = E_UNEXPECTED;

_CLEANUP:
    if ( writeStream )
        IRandomAccessStream_Release( writeStream );
    if ( writeStreamOperation )
        IAsyncOperation_IRandomAccessStream_Release( writeStreamOperation );
    if ( storeOperation )
        IAsyncOperation_UINT32_Release( storeOperation );
    if ( flushOperation )
        IAsyncOperation_boolean_Release( flushOperation );
    if ( outputStream )
        IOutputStream_Release( outputStream );
    if ( dataWriter )
        IDataWriter_Release( dataWriter );
    if ( dataWriterFactory )
        IDataWriterFactory_Release( dataWriterFactory );
    free( write_buffer_options );

    return status;
}

HRESULT WINAPI file_io_statics_WriteBytes( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT status = S_OK;
    BOOLEAN canWrite;
    UINT32 bytesWritten;
    DWORD asyncRes;

    IAsyncOperation_IRandomAccessStream *writeStreamOperation = NULL;
    IAsyncOperation_boolean *flushOperation = NULL;
    IAsyncOperation_UINT32 *storeOperation = NULL;
    IRandomAccessStream *writeStream = NULL;
    IDataWriterFactory *dataWriterFactory = NULL;
    IOutputStream *outputStream = NULL;
    IDataWriter *dataWriter = NULL;

    struct file_io_write_bytes_options *write_bytes_options = (struct file_io_write_bytes_options *)param;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataWriter, dataWriterFactory, IID_IDataWriterFactory );

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFile_OpenAsync( write_bytes_options->file, FileAccessMode_ReadWrite, &writeStreamOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_IRandomAccessStream( writeStreamOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_IRandomAccessStream_GetResults( writeStreamOperation, &writeStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    IRandomAccessStream_get_CanWrite( writeStream, &canWrite );
    if ( !canWrite )
    {
        status = E_ACCESSDENIED;
        goto _CLEANUP;
    }

    // Clear out the file
    status = IRandomAccessStream_put_Size( writeStream, 0 );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IRandomAccessStream_GetOutputStreamAt( writeStream, 0, &outputStream );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriterFactory_CreateDataWriter( dataWriterFactory, outputStream, &dataWriter );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_WriteBytes( dataWriter, write_bytes_options->bufferSize, write_bytes_options->buffer );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = IDataWriter_StoreAsync( dataWriter, &storeOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_UINT32( storeOperation, INFINITE );
    if ( asyncRes )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_UINT32_GetResults( storeOperation, &bytesWritten );
    if ( FAILED( status ) ) goto _CLEANUP;

    if ( bytesWritten != write_bytes_options->bufferSize )
    {
        status = E_FAIL;
        goto _CLEANUP;
    }

    status = IDataWriter_FlushAsync( dataWriter, &flushOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_boolean( flushOperation, INFINITE );
    if ( asyncRes ) status = E_UNEXPECTED;

_CLEANUP:
    if ( writeStream )
        IRandomAccessStream_Release( writeStream );
    if ( writeStreamOperation )
        IAsyncOperation_IRandomAccessStream_Release( writeStreamOperation );
    if ( storeOperation )
        IAsyncOperation_UINT32_Release( storeOperation );
    if ( flushOperation )
        IAsyncOperation_boolean_Release( flushOperation );
    if ( outputStream )
        IOutputStream_Release( outputStream );
    if ( dataWriter )
        IDataWriter_Release( dataWriter );
    if ( dataWriterFactory )
        IDataWriterFactory_Release( dataWriterFactory );
    free( write_bytes_options );

    return status;
}