/* WinRT Windows.Storage.PathIO Implementation
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

#include "PathIOInternal.h"

_ENABLE_DEBUGGING_

HRESULT WINAPI path_io_statics_ReadText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;
    HANDLE fileHandle = INVALID_HANDLE_VALUE;    
    LPWSTR fileBufferWChar = NULL;
    LPWSTR outputBuffer = NULL;
    LPSTR fileBufferChar = NULL;
    DWORD fileSize;
    DWORD bytesRead;
    ULONG i;
    BOOL readResult;

    IStorageItem *item = NULL;

    struct path_io_read_text_options *read_text_options = (struct path_io_read_text_options *)param;

    //Parameters    
    UnicodeEncoding unicodeEncoding = read_text_options->encoding;
    WindowsDuplicateString( read_text_options->absolutePath, &filePath );

    TRACE( "iface %p, value %p\n", invoker, result );

    fileHandle = CreateFileW( WindowsGetStringRawBuffer( filePath, NULL ), GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    fileSize = GetFileSize( fileHandle, NULL );

    if ( fileSize == INVALID_FILE_SIZE )
    {
        status = E_INVALIDARG;
        goto _CLEANUP;
    }

    outputBuffer = (LPWSTR)malloc( fileSize );

    if ( !outputBuffer )
    {
        status = E_OUTOFMEMORY;
        goto _CLEANUP;
    }

    if ( unicodeEncoding == UnicodeEncoding_Utf8 )
    {
        fileBufferChar = (LPSTR)malloc( fileSize );
        if ( !fileBufferChar )
        {
            status = E_OUTOFMEMORY;
            goto _CLEANUP;
        }
        readResult = ReadFile( fileHandle, (LPVOID)fileBufferChar, fileSize, &bytesRead, NULL );
    } else
    {
        fileBufferWChar = (LPWSTR)malloc( fileSize );
        if ( !fileBufferWChar )
        {
            status = E_OUTOFMEMORY;
            goto _CLEANUP;
        }
        readResult = ReadFile( fileHandle, (LPVOID)fileBufferWChar, fileSize, &bytesRead, NULL );
    }

    if ( !readResult || bytesRead != fileSize )
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

    if ( unicodeEncoding != UnicodeEncoding_Utf8 )
    {
        if ( fileSize % 2 != 0 )
        {
            status = E_FAIL;
            goto _CLEANUP;
        }
    }

    switch ( unicodeEncoding )
    {
        case UnicodeEncoding_Utf8:
            MultiByteToWideChar( CP_UTF8, 0, fileBufferChar, -1, outputBuffer, fileSize );
            if ( !outputBuffer )
                status = E_OUTOFMEMORY;
            break;

        case UnicodeEncoding_Utf16LE:
            wcscpy( outputBuffer, fileBufferWChar );
            break;

        case UnicodeEncoding_Utf16BE:
            for ( i = 0; i < fileSize / sizeof( WCHAR ); i++ )
                fileBufferWChar[i] = ( fileBufferWChar[i] >> 8 ) | ( fileBufferWChar[i] << 8 );

            wcscpy( outputBuffer, fileBufferWChar );
            break;

        default:
            status = E_INVALIDARG;
    }

    outputBuffer[ fileSize ] = '\0';

    if ( SUCCEEDED ( status ) )
    {
        result->vt = VT_LPWSTR;
        result->pwszVal = outputBuffer;
    }

_CLEANUP:
    if ( fileHandle != INVALID_HANDLE_VALUE )
        CloseHandle( fileHandle );
    if ( fileBufferChar )
        free( fileBufferChar );
    if ( fileBufferWChar )
        free( fileBufferWChar );
    if ( FAILED(status) && outputBuffer )
        free( outputBuffer );
    if ( item )
        IStorageItem_Release(item);

    return status;
}

HRESULT WINAPI path_io_statics_WriteText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;
    HSTRING contents;
    HANDLE fileHandle;
    LPSTR writeBufferChar = NULL;
    LPWSTR contentsBE = NULL;
    DWORD bytesWritten;
    ULONG i;
    BYTE UTF16LEBOM[] = { 0xFF, 0xFE };
    BYTE UTF16BEBOM[] = { 0xFE, 0xFF };

    IStorageItem *item = NULL;

    struct path_io_write_text_options *write_text_options = (struct path_io_write_text_options *)param;

    //Parameters    
    UnicodeEncoding unicodeEncoding = write_text_options->encoding;
    WindowsDuplicateString( write_text_options->absolutePath, &filePath );
    WindowsDuplicateString( write_text_options->contents, &contents );

    TRACE( "iface %p, value %p\n", invoker, result );

    fileHandle = CreateFileW( WindowsGetStringRawBuffer( filePath, NULL ), GENERIC_WRITE, 0 , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );    
    //Clear the file
    if ( SetFilePointer( fileHandle, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    if ( !SetEndOfFile( fileHandle ) ) 
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

    switch ( unicodeEncoding )
    {
        case UnicodeEncoding_Utf8:
            writeBufferChar = (LPSTR)malloc( wcslen( WindowsGetStringRawBuffer( contents, NULL ) ) + 1 );
            if ( !writeBufferChar )
            {
                status = E_OUTOFMEMORY;
                goto _CLEANUP;
            }

            WideCharToMultiByte( CP_UTF8, 0, WindowsGetStringRawBuffer( contents, NULL ), -1, writeBufferChar, wcslen( WindowsGetStringRawBuffer( contents, NULL ) ), NULL, NULL );
            
            if ( !WriteFile( fileHandle, (LPCVOID)writeBufferChar, wcslen( WindowsGetStringRawBuffer( contents, NULL ) ), &bytesWritten, NULL ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case UnicodeEncoding_Utf16LE:
            if ( !WriteFile( fileHandle, (LPCVOID)UTF16LEBOM, sizeof( UTF16LEBOM ), &bytesWritten, NULL ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }

            if ( !WriteFile( fileHandle, (LPCVOID)WindowsGetStringRawBuffer( contents, NULL ), 
                        ( wcslen( WindowsGetStringRawBuffer( contents, NULL ) ) + 1 ) * sizeof( WCHAR ), &bytesWritten, NULL) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case UnicodeEncoding_Utf16BE:
            contentsBE = (LPWSTR)malloc( wcslen( WindowsGetStringRawBuffer( contents, NULL ) ) + 1 );
            if ( !contentsBE )
            {
                status = E_OUTOFMEMORY;
                goto _CLEANUP;
            }

            if ( !WriteFile( fileHandle, (LPCVOID)UTF16BEBOM, sizeof( UTF16BEBOM ), &bytesWritten, NULL ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }

            for ( i = 0; i < wcslen( WindowsGetStringRawBuffer( contents, NULL ) ); i++ ) 
                contentsBE[i] = ( WindowsGetStringRawBuffer( contents, NULL )[i] >> 8) | ( WindowsGetStringRawBuffer( contents, NULL )[i] << 8 );

            if ( !WriteFile( fileHandle, (LPCVOID)contentsBE, 
                        ( wcslen( contentsBE ) + 1 ) * sizeof( WCHAR ), &bytesWritten, NULL) ) {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        default:
            status = E_INVALIDARG;
    }

_CLEANUP:
    if ( fileHandle != INVALID_HANDLE_VALUE )
        CloseHandle( fileHandle );
    if ( writeBufferChar )
        free( writeBufferChar );
    if ( contentsBE )
        free( contentsBE );
    if ( item )
        IStorageItem_Release( item );
    WindowsDeleteString( contents );

    return status;
}

HRESULT WINAPI path_io_statics_AppendText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;
    HSTRING contents;
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    LPSTR writeBufferChar = NULL;
    LPWSTR contentsBE = NULL;
    DWORD bytesWritten;
    ULONG i;
    BYTE UTF16LEBOM[] = { 0xFF, 0xFE };
    BYTE UTF16BEBOM[] = { 0xFE, 0xFF };
    
    IStorageItem *item = NULL;

    struct path_io_write_text_options *write_text_options = (struct path_io_write_text_options *)param;

    //Parameters   
    UnicodeEncoding unicodeEncoding = write_text_options->encoding;
    WindowsDuplicateString( write_text_options->absolutePath, &filePath );
    WindowsDuplicateString( write_text_options->contents, &contents );

    TRACE( "iface %p, value %p\n", invoker, result );

    fileHandle = CreateFileW( WindowsGetStringRawBuffer( filePath, NULL ), FILE_APPEND_DATA, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );    

    switch ( unicodeEncoding )
    {
        case UnicodeEncoding_Utf8:
            writeBufferChar = (LPSTR)malloc( wcslen( WindowsGetStringRawBuffer( contents, NULL ) ) + 1 );
            if ( !writeBufferChar )
            {
                status = E_OUTOFMEMORY;
                goto _CLEANUP;
            }

            WideCharToMultiByte( CP_UTF8, 0, WindowsGetStringRawBuffer( contents, NULL ), -1, writeBufferChar, wcslen( WindowsGetStringRawBuffer( contents, NULL ) ), NULL, NULL );
            
            if ( !WriteFile( fileHandle, (LPCVOID)writeBufferChar, wcslen( WindowsGetStringRawBuffer( contents, NULL ) ), &bytesWritten, NULL ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case UnicodeEncoding_Utf16LE:
            if ( !WriteFile( fileHandle, (LPCVOID)UTF16LEBOM, sizeof( UTF16LEBOM ), &bytesWritten, NULL ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }

            if ( !WriteFile( fileHandle, (LPCVOID)WindowsGetStringRawBuffer( contents, NULL ), 
                        ( wcslen( WindowsGetStringRawBuffer( contents, NULL ) ) + 1 ) * sizeof( WCHAR ), &bytesWritten, NULL) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case UnicodeEncoding_Utf16BE:
            contentsBE = (LPWSTR)malloc( wcslen( WindowsGetStringRawBuffer( contents, NULL ) ) + 1 );
            if ( !contentsBE )
            {
                status = E_OUTOFMEMORY;
                goto _CLEANUP;
            }

            if ( !WriteFile( fileHandle, (LPCVOID)UTF16BEBOM, sizeof( UTF16BEBOM ), &bytesWritten, NULL ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }

            for ( i = 0; i < wcslen( WindowsGetStringRawBuffer( contents, NULL ) ); i++ ) 
                contentsBE[i] = ( WindowsGetStringRawBuffer( contents, NULL )[i] >> 8) | ( WindowsGetStringRawBuffer( contents, NULL )[i] << 8 );

            if ( !WriteFile( fileHandle, (LPCVOID)contentsBE, 
                        ( wcslen( contentsBE ) + 1 ) * sizeof( WCHAR ), &bytesWritten, NULL) ) {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        default:
            status = E_INVALIDARG;
            goto _CLEANUP;
    }

_CLEANUP:
    if ( fileHandle != INVALID_HANDLE_VALUE )
        CloseHandle( fileHandle );
    if ( writeBufferChar )
        free( writeBufferChar );
    if ( contentsBE )
        free( contentsBE );
    if ( item )
        IStorageItem_Release( item );
    WindowsDeleteString( contents );

    return status;
}

//I only managed to hit my head against the wall twice while writing this function

HRESULT WINAPI path_io_statics_ReadLines( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;
    HSTRING vectorElement;
    HANDLE fileHandle = INVALID_HANDLE_VALUE;    
    LPWSTR fileBufferWChar = NULL;
    LPWSTR outputBuffer = NULL;
    LPWSTR tmpBuffer = NULL;
    LPSTR fileBufferChar = NULL;
    DWORD fileSize;
    DWORD bytesRead;
    ULONG i;
    ULONG currChar;
    BOOL readResult;
    size_t INITIAL_BUFFER_SIZE = 100;    
    
    IVector_HSTRING *vector = NULL;
    IStorageItem *item = NULL;

    struct path_io_read_text_options *read_text_options = (struct path_io_read_text_options *)param;

    //Parameters    
    UnicodeEncoding unicodeEncoding = read_text_options->encoding;
    WindowsDuplicateString( read_text_options->absolutePath, &filePath );

    TRACE( "iface %p, value %p\n", invoker, result );

    status = hstring_vector_create( &vector );
    if ( FAILED( status ) ) goto _CLEANUP;

    fileHandle = CreateFileW( WindowsGetStringRawBuffer( filePath, NULL ), GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    fileSize = GetFileSize( fileHandle, NULL );

    if ( fileSize == INVALID_FILE_SIZE )
    {
        status = E_INVALIDARG;
        goto _CLEANUP;
    }

    outputBuffer = (LPWSTR)malloc( fileSize );

    if ( !outputBuffer )
    {
        status = E_OUTOFMEMORY;
        goto _CLEANUP;
    }

    tmpBuffer = (LPWSTR)malloc( sizeof(wchar_t) * INITIAL_BUFFER_SIZE );

    if ( !tmpBuffer )
    {
        status = E_OUTOFMEMORY;
        goto _CLEANUP;
    }

    if ( unicodeEncoding == UnicodeEncoding_Utf8 )
    {
        fileBufferChar = (LPSTR)malloc( fileSize );
        if ( !fileBufferChar )
        {
            status = E_OUTOFMEMORY;
            goto _CLEANUP;
        }
        readResult = ReadFile( fileHandle, (LPVOID)fileBufferChar, fileSize, &bytesRead, NULL );
    } else
    {
        fileBufferWChar = (LPWSTR)malloc( fileSize );
        if ( !fileBufferWChar )
        {
            status = E_OUTOFMEMORY;
            goto _CLEANUP;
        }
        readResult = ReadFile( fileHandle, (LPVOID)fileBufferWChar, fileSize, &bytesRead, NULL );
    }

    if ( !readResult || bytesRead != fileSize )
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

    if ( unicodeEncoding != UnicodeEncoding_Utf8 )
    {
        if ( fileSize % 2 != 0 )
        {
            status = E_INVALIDARG;
            goto _CLEANUP;
        }
    }

    switch ( unicodeEncoding )
    {
        case UnicodeEncoding_Utf8:
            MultiByteToWideChar( CP_UTF8, 0, fileBufferChar, -1, outputBuffer, fileSize );
            if ( !outputBuffer )
            {
                status = E_OUTOFMEMORY;
                goto _CLEANUP;
            }
            break;

        case UnicodeEncoding_Utf16LE:
            wcscpy( outputBuffer, fileBufferWChar );
            break;

        case UnicodeEncoding_Utf16BE:
            for ( i = 0; i < fileSize / sizeof( WCHAR ); i++ )
                fileBufferWChar[i] = ( fileBufferWChar[i] >> 8 ) | ( fileBufferWChar[i] << 8 );

            wcscpy( outputBuffer, fileBufferWChar );
            break;

        default:
            status = E_INVALIDARG;
            goto _CLEANUP;
    }

    outputBuffer[ fileSize ] = L'\0';
    tmpBuffer[0] = L'\0';

    for ( currChar = 0; currChar < wcslen( outputBuffer ); currChar++ )
    {
        if ( outputBuffer[ currChar ] == L'\n' )
        {
            WindowsCreateString( tmpBuffer, wcslen( tmpBuffer ), &vectorElement );
            IVector_HSTRING_Append( vector, vectorElement );
            tmpBuffer = (LPWSTR)malloc(sizeof(wchar_t) * INITIAL_BUFFER_SIZE);
        } else
        {
            //Append the current character to tmpBuffer. We'll use tmpBuffer once control reaches line break.
            size_t tmpBufferLen = wcslen(tmpBuffer);
            if (tmpBufferLen + 1 >= INITIAL_BUFFER_SIZE) {
                // Grow the buffer if necessary.
                INITIAL_BUFFER_SIZE *= 2;
                tmpBuffer = (LPWSTR)realloc(tmpBuffer, sizeof(wchar_t) * INITIAL_BUFFER_SIZE);
            }
            tmpBuffer[ tmpBufferLen ] = outputBuffer[ currChar ];
            tmpBuffer[ tmpBufferLen + 1 ] = L'\0';
        }
    }

    if ( tmpBuffer )
    {
        WindowsCreateString( tmpBuffer, wcslen( tmpBuffer ), &vectorElement );
        IVector_HSTRING_Append( vector, vectorElement );
    }

    if ( SUCCEEDED ( status ) )
    {
        IVector_HSTRING_AddRef( vector );
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)vector;
    }

_CLEANUP:
    if ( fileHandle != INVALID_HANDLE_VALUE )
        CloseHandle( fileHandle );
    if ( fileBufferChar )
        free( fileBufferChar );
    if ( fileBufferWChar )
        free( fileBufferWChar );
    if ( outputBuffer )
        free( outputBuffer );
    if ( tmpBuffer )
        free( tmpBuffer );
    if ( item )
        IStorageItem_Release( item );
    if ( FAILED( status ) && vector )
        IVector_HSTRING_Release( vector );

    return status;
}

HRESULT WINAPI path_io_statics_ReadBuffer( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    BYTE *outputBuffer = NULL;
    DWORD fileSize;
    DWORD bytesRead;
    BOOL readResult;    
    
    IBufferByteAccess *buffer_byte_access = NULL;
    IBufferFactory *buffer_factory = NULL;
    IStorageItem *item = NULL;
    IBuffer *buffer = NULL;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_Buffer, buffer_factory, IID_IBufferFactory );

    //Parameters
    WindowsDuplicateString( (HSTRING)param, &filePath );

    TRACE( "iface %p, value %p\n", invoker, result );

    fileHandle = CreateFileW( WindowsGetStringRawBuffer( filePath, NULL ), GENERIC_READ, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    fileSize = GetFileSize( fileHandle, NULL );

    if ( fileSize == INVALID_FILE_SIZE )
    {
        status = E_INVALIDARG;
        goto _CLEANUP;
    }

    status = IBufferFactory_Create( buffer_factory, fileSize, &buffer );
    if ( FAILED( status ) )
        goto _CLEANUP;

    status = IBuffer_QueryInterface( buffer, &IID_IBufferByteAccess, (void **)&buffer_byte_access );
    if ( FAILED( status ) )
        goto _CLEANUP;

    status = IBufferByteAccess_Buffer( buffer_byte_access, &outputBuffer );
    if ( FAILED( status ) )
        goto _CLEANUP;
    
    readResult = ReadFile( fileHandle, (LPVOID)outputBuffer, fileSize, &bytesRead, NULL );

    if ( !readResult || bytesRead != fileSize )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    if ( SUCCEEDED ( status ) )
    {
        IBuffer_put_Length( buffer, fileSize );
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)buffer;
    }

_CLEANUP:
    if ( fileHandle != INVALID_HANDLE_VALUE )
        CloseHandle( fileHandle );
    if ( outputBuffer && FAILED(status) )
        free( outputBuffer );
    if ( item )
        IStorageItem_Release( item );
    if ( buffer && FAILED(status) )
        IBuffer_Release( buffer );

    return status;
}

HRESULT WINAPI path_io_statics_WriteBuffer( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;
    UINT32 bufferLength;
    BYTE *contents;
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    DWORD bytesWritten;    
    
    IBufferByteAccess *bufferByteAccess = NULL;
    IStorageItem *item = NULL;

    struct path_io_write_buffer_options *write_buffer_options = (struct path_io_write_buffer_options *)param;

    //Parameters
    WindowsDuplicateString( write_buffer_options->absolutePath, &filePath );

    TRACE( "iface %p, value %p\n", invoker, result );

    IBuffer_QueryInterface( write_buffer_options->buffer, &IID_IBufferByteAccess, (void **)&bufferByteAccess );
    IBufferByteAccess_Buffer( bufferByteAccess, &contents );

    fileHandle = CreateFileW( WindowsGetStringRawBuffer( filePath, NULL ), GENERIC_WRITE, 0 , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );    
    //Clear the file
    if ( SetFilePointer( fileHandle, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    if ( !SetEndOfFile( fileHandle ) ) 
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

    IBuffer_get_Length( write_buffer_options->buffer, &bufferLength );

    if ( !WriteFile( fileHandle, (LPCVOID)contents, bufferLength, &bytesWritten, NULL ) )
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

_CLEANUP:
    if ( fileHandle != INVALID_HANDLE_VALUE )
        CloseHandle( fileHandle );
    if ( item )
        IStorageItem_Release( item );
    if ( bufferByteAccess )
        IBufferByteAccess_Release( bufferByteAccess );

    return status;
}

HRESULT WINAPI path_io_statics_WriteBytes( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;    
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    DWORD bytesWritten;
    BYTE *contents;
    
    IStorageItem *item = NULL;

    struct path_io_write_bytes_options *write_bytes_options = (struct path_io_write_bytes_options *)param;

    //Parameters
    WindowsDuplicateString( write_bytes_options->absolutePath, &filePath );
    
    TRACE( "iface %p, value %p\n", invoker, result );

    contents = write_bytes_options->buffer;

    fileHandle = CreateFileW( WindowsGetStringRawBuffer( filePath, NULL ), GENERIC_WRITE, 0 , NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );    
    //Clear the file
    if ( SetFilePointer( fileHandle, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    if ( !SetEndOfFile( fileHandle ) ) 
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

    if ( !WriteFile( fileHandle, (LPCVOID)contents, write_bytes_options->bufferSize, &bytesWritten, NULL ) )
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

_CLEANUP:
    if ( fileHandle != INVALID_HANDLE_VALUE )
        CloseHandle( fileHandle );
    if ( item )
        IStorageItem_Release( item );

    return status;
}