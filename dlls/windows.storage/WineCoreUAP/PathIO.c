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

DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_file_handler, IAsyncOperationCompletedHandler_StorageFile, IAsyncOperation_StorageFile )

// Path Input Output Operations

DEFINE_IACTIVATIONFACTORY( struct path_io_statics, IPathIOStatics_iface, IID_IPathIOStatics )

DEFINE_IINSPECTABLE( path_io_statics, IPathIOStatics, struct path_io_statics, IActivationFactory_iface )

static HRESULT WINAPI 
ObtainStorageFileFromPath( 
    IN HSTRING absolutePath, 
    OUT IStorageFile **targetFile 
) {
    HRESULT hr;
    DWORD asyncRes;

    IAsyncOperation_StorageFile *storageFileOperation = NULL;
    IStorageFileStatics *storageFileStatics = NULL; 

    TRACE( "absolutePath %s, targetFile %p\n", debugstr_hstring( absolutePath ), targetFile );

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_StorageFile, storageFileStatics, IID_IStorageFileStatics );

    hr = IStorageFileStatics_GetFileFromPathAsync( storageFileStatics, absolutePath, &storageFileOperation );
    if ( FAILED( hr ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_StorageFile( storageFileOperation, INFINITE );
    if ( asyncRes )
    {
        hr = E_UNEXPECTED;
        goto _CLEANUP;
    }

    hr = IAsyncOperation_StorageFile_GetResults( storageFileOperation, targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

_CLEANUP:
    if ( storageFileStatics )
        IStorageFileStatics_Release( storageFileStatics );
    if ( storageFileOperation )
        IAsyncOperation_StorageFile_Release( storageFileOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<HSTRING> IPathIOStatics::ReadTextAsync
 *      Description: Reads text from an IStorageFile*, obtained from the given path then returns 
 *      all the contents of the file within a packed HSTRING.
 */
static HRESULT WINAPI 
path_io_statics_ReadTextAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IAsyncOperation_HSTRING **textOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, operation %p\n", iface, debugstr_hstring( absolutePath ), textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_ReadTextAsync( fileIOStatics, targetFile, textOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<HSTRING> IPathIOStatics::ReadTextWithEncodingAsync
 *      Description: Reads text from an IStorageFile*, obtained from the given path then returns 
 *      all the contents of the file within a packed HSTRING with the specified encoding.
 */
static HRESULT WINAPI 
path_io_statics_ReadTextWithEncodingAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    UnicodeEncoding encoding, 
    IAsyncOperation_HSTRING **textOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, encoding %d, operation %p\n", iface, debugstr_hstring( absolutePath ), encoding, textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_ReadTextWithEncodingAsync( fileIOStatics, targetFile, encoding, textOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::WriteTextAsync
 *      Description: Overwrites the contents of an IStorageFile*, obtained from the given path
 *      with the given packed HSTRING.
 */
static HRESULT WINAPI 
path_io_statics_WriteTextAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    HSTRING contents, 
    IAsyncAction **textOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, contents %p, operation %p\n", iface, debugstr_hstring( absolutePath ), &contents, textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_WriteTextAsync( fileIOStatics, targetFile, contents, textOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::WriteTextWithEncodingAsync
 *      Description: Overwrites the contents of an IStorageFile*, obtained from the given path
 *      with the given packed HSTRING with the specified encoding.
 */
static HRESULT WINAPI 
path_io_statics_WriteTextWithEncodingAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    HSTRING contents, 
    UnicodeEncoding encoding, 
    IAsyncAction **textOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, contents %p, encoding %d, operation %p\n", iface, debugstr_hstring( absolutePath ), &contents, encoding, textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_WriteTextWithEncodingAsync( fileIOStatics, targetFile, contents, encoding, textOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::AppendTextAsync
 *      Description: Appends a packed HSTRING to the contents of an IStorageFile*, obtained
 *      from the given path.
 */
static HRESULT WINAPI 
path_io_statics_AppendTextAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    HSTRING contents, 
    IAsyncAction **textOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, contents %p, operation %p\n", iface, debugstr_hstring( absolutePath ), &contents, textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_AppendTextAsync( fileIOStatics, targetFile, contents, textOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::AppendTextAsync
 *      Description: Appends a packed HSTRING to the contents of an IStorageFile*, obtained
 *      from the given path, with the specified encoding.
 */
static HRESULT WINAPI 
path_io_statics_AppendTextWithEncodingAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    HSTRING contents, 
    UnicodeEncoding encoding, 
    IAsyncAction **textOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, contents %p, encoding %d, operation %p\n", iface, debugstr_hstring( absolutePath ), &contents, encoding, textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_AppendTextWithEncodingAsync( fileIOStatics, targetFile, contents, encoding, textOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<IVector <HSTRING>*> IPathIOStatics::ReadLinesAsync
 *      Description: Reads the contents of an IStorageFile*, obtained from the given path, 
 *      line by line and appends it to an HSTRING Vector.
 */
static HRESULT WINAPI 
path_io_statics_ReadLinesAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IAsyncOperation_IVector_HSTRING **linesOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, operation %p\n", iface, debugstr_hstring( absolutePath ), linesOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !linesOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_ReadLinesAsync( fileIOStatics, targetFile, linesOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<IVector <HSTRING>*> IPathIOStatics::ReadLinesAsync
 *      Description: Reads the contents of an IStorageFile*, obtained from the given path,
 *      line by line and appends it to an HSTRING Vector with the specified encoding.
 */
static HRESULT WINAPI 
path_io_statics_ReadLinesWithEncodingAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    UnicodeEncoding encoding, 
    IAsyncOperation_IVector_HSTRING **linesOperation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, encoding %d, operation %p\n", iface, debugstr_hstring( absolutePath ), encoding, linesOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !linesOperation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_ReadLinesWithEncodingAsync( fileIOStatics, targetFile, encoding, linesOperation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::WriteLinesAsync
 *      Description: Overwrites the contents of an IStorageFile*, obtained from the given path
 *      with the given HSTRING vector.
 */
static HRESULT WINAPI 
path_io_statics_WriteLinesAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IIterable_HSTRING *lines, 
    IAsyncAction **operation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, lines %p, operation %p\n", iface, debugstr_hstring( absolutePath ), lines, operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_WriteLinesAsync( fileIOStatics, targetFile, lines, operation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::WriteLinesWithEncodingAsync
 *      Description: Overwrites the contents of an IStorageFile*, obtained from the given path
 *      with the given HSTRING vector with the specified encoding.
 */
static HRESULT WINAPI 
path_io_statics_WriteLinesWithEncodingAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IIterable_HSTRING *lines, 
    UnicodeEncoding encoding, 
    IAsyncAction **operation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, lines %p, operation %p\n", iface, debugstr_hstring( absolutePath ), lines, operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_WriteLinesWithEncodingAsync( fileIOStatics, targetFile, lines, encoding, operation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::AppendLinesAsync
 *      Description: Appends an HSTRING vector to the contents of an IStorageFile*,
 *      obtained from the given path.
 */
static HRESULT WINAPI 
path_io_statics_AppendLinesAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IIterable_HSTRING *lines, 
    IAsyncAction **operation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, lines %p, operation %p\n", iface, debugstr_hstring( absolutePath ), lines, operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_AppendLinesAsync( fileIOStatics, targetFile, lines, operation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::AppendLinesWithEncodingAsync
 *      Description: Appends an HSTRING vector to the contents of an IStorageFile*,
 *      obtained from the given path, with the specified encoding.
 */
static HRESULT WINAPI 
path_io_statics_AppendLinesWithEncodingAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IIterable_HSTRING *lines, 
    UnicodeEncoding encoding, 
    IAsyncAction **operation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, lines %p, encoding %d, operation %p\n", iface, debugstr_hstring( absolutePath ), lines, encoding, operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_AppendLinesWithEncodingAsync( fileIOStatics, targetFile, lines, encoding, operation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<IBuffer *> IPathIOStatics::ReadBufferAsync
 *      Description: Reads the contents of an IStorageFile*, obtained from the given path, 
 *      and appends it to an IBuffer.
 */
static HRESULT WINAPI 
path_io_statics_ReadBufferAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IAsyncOperation_IBuffer **operation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, operation %p\n", iface, debugstr_hstring( absolutePath ), operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_ReadBufferAsync( fileIOStatics, targetFile, operation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::WriteBufferAsync
 *      Description: Overwrites the contents of an IStorageFile*, obtained from the given path,
 *      with the given IBuffer.
 */
static HRESULT WINAPI 
path_io_statics_WriteBufferAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    IBuffer* buffer, 
    IAsyncAction **operation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, buffer %p, operation %p\n", iface, debugstr_hstring( absolutePath ), buffer, operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_WriteBufferAsync( fileIOStatics, targetFile, buffer, operation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IPathIOStatics::WriteBytesAsync
 *      Description: Overwrites the contents of an IStorageFile*, obtained from the given path,
 *      with the given byte buffer.
 */
static HRESULT WINAPI 
path_io_statics_WriteBytesAsync( 
    IPathIOStatics *iface, 
    HSTRING absolutePath, 
    UINT32 __bufferSize, 
    BYTE *buffer, 
    IAsyncAction **operation 
) {
    HRESULT hr;

    IFileIOStatics *fileIOStatics = NULL;
    IStorageFile *targetFile = NULL;

    TRACE( "iface %p, absolutePath %s, __bufferSize %d, buffer %p, operation %p\n", iface, debugstr_hstring( absolutePath ), __bufferSize, &buffer, operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_FileIO, fileIOStatics, IID_IFileIOStatics );

    hr = ObtainStorageFileFromPath( absolutePath, &targetFile );
    if ( FAILED( hr ) ) goto _CLEANUP;

    hr = IFileIOStatics_WriteBytesAsync( fileIOStatics, targetFile, __bufferSize, buffer, operation );

_CLEANUP:
    if ( fileIOStatics )
        IFileIOStatics_Release( fileIOStatics );
    if ( targetFile )
        IStorageFile_Release( targetFile );

    return hr;
}

static const struct IPathIOStaticsVtbl path_io_statics_vtbl =
{
    path_io_statics_QueryInterface,
    path_io_statics_AddRef,
    path_io_statics_Release,
    /* IInspectable methods */
    path_io_statics_GetIids,
    path_io_statics_GetRuntimeClassName,
    path_io_statics_GetTrustLevel,
    /* IPathIOStatics methods */
    path_io_statics_ReadTextAsync,
    path_io_statics_ReadTextWithEncodingAsync,
    path_io_statics_WriteTextAsync,
    path_io_statics_WriteTextWithEncodingAsync,
    path_io_statics_AppendTextAsync,
    path_io_statics_AppendTextWithEncodingAsync,
    path_io_statics_ReadLinesAsync,
    path_io_statics_ReadLinesWithEncodingAsync,
    path_io_statics_WriteLinesAsync,
    path_io_statics_WriteLinesWithEncodingAsync,
    path_io_statics_AppendLinesAsync,
    path_io_statics_AppendLinesWithEncodingAsync,
    path_io_statics_ReadBufferAsync,
    path_io_statics_WriteBufferAsync,
    path_io_statics_WriteBytesAsync
};

static struct path_io_statics path_io_statics =
{
    {&factory_vtbl},
    {&path_io_statics_vtbl},
    1,
};

IActivationFactory *path_io_factory = &path_io_statics.IActivationFactory_iface;
