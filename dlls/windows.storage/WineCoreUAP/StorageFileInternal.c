/* WinRT Windows.Storage.StorageFile Implementation
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

#include "StorageFileInternal.h"

_ENABLE_DEBUGGING_

static VOID GenerateUniqueFileName( LPWSTR buffer, SIZE_T bufferSize ) {
    UUID uuid;
    LPWSTR str;

    UuidCreate( &uuid );
    UuidToStringW( &uuid, (RPC_WSTR*)&str );
    swprintf( buffer, bufferSize, L"%s", str );

    RpcStringFreeW( (RPC_WSTR*)&str );
}

HRESULT WINAPI storage_file_AssignFile ( HSTRING filePath, IStorageFile ** result )
{
    //Must be called from an asynchronous context!
    HRESULT status;
    HSTRING path;
    BOOLEAN isFile = FALSE;
    LPWSTR pwsMimeOut = NULL;
    LPWSTR fileExtension = NULL;     
    DWORD bytesRead = 0;
    BYTE buffer[BUFFER_SIZE];
    FILE *fileData;

    IStorageItem *fileItem = NULL;
    IStorageItemProperties *fileItemProperties = NULL;

    struct storage_file *file;

    TRACE( "filePath %p, value %p\n", filePath, result );

    if (!(file = calloc( 1, sizeof(*file) ))) return E_OUTOFMEMORY;

    file->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    file->ref = 1;

    WindowsDuplicateString( filePath, &path );

    status = storage_item_Internal_CreateNew( path, &fileItem );
    if ( FAILED( status ) )
    {
        WindowsDeleteString( path );
        free( file );
        return status;
    }

    // This copies each corresponding field of storage_item to their respective values in storage_file.
    // Any changes in either of the structs needs to be reflected on the other.
    memcpy( &file->IStorageItem_iface, fileItem, sizeof(struct storage_item) );

    file->IStorageItem2_iface.lpVtbl = &storage_item2_vtbl;
    file->IStorageFilePropertiesWithAvailability_iface.lpVtbl = &storage_file_properties_with_availability_vtbl;

    IStorageItem_IsOfType( &file->IStorageItem_iface, StorageItemTypes_File, &isFile );
    if ( !isFile )
    {
        status = E_INVALIDARG;
        if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_ISNOTFILE ) ) ) )
            return E_UNEXPECTED;
        return status;
    }

    if ( SUCCEEDED(status) )
        status = storage_item_properties_AssignProperties( &file->IStorageItem_iface, &fileItemProperties );

    if ( SUCCEEDED( status ) )
    {
        memcpy( &file->IStorageItemProperties_iface, fileItemProperties, sizeof(struct storage_item_properties) );
        status = CreateRandomAccessStreamOnFile( WindowsGetStringRawBuffer( path, NULL ), FileAccessMode_Read, &IID_IRandomAccessStreamReference, (void **)&file->IRandomAccessStreamReference_iface );
    }
    
    if ( SUCCEEDED( status ) )
    {
        fileExtension = wcsrchr( WindowsGetStringRawBuffer( path, NULL ), '.' );
        if ( fileExtension && fileExtension != WindowsGetStringRawBuffer( path, NULL ) )
        {
            WindowsCreateString( fileExtension, wcslen( fileExtension ), &file->FileType );
        }
        fileData = _wfopen( WindowsGetStringRawBuffer( path, NULL ), L"rb" );
        bytesRead = (DWORD)fread( buffer, 1, BUFFER_SIZE, fileData );
        fclose( fileData );

        FindMimeFromData( NULL, WindowsGetStringRawBuffer( path, NULL ), buffer, bytesRead, NULL, 0, &pwsMimeOut, 0 );
        if (pwsMimeOut != NULL)
        {
            WindowsCreateString( pwsMimeOut, wcslen( pwsMimeOut ), &file->ContentType );
        }

        CoTaskMemFree( pwsMimeOut );

        *result = &file->IStorageFile_iface;
    } else {
        WindowsDeleteString( path );        
        free( file );
    }

    IStorageItem_Release( fileItem );
    IStorageItemProperties_Release( fileItemProperties );

    return status;
}

HRESULT WINAPI storage_file_AssignFileAsync ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;

    IStorageFile *file = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = storage_file_AssignFile( (HSTRING)param, &file );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)file;
    }

    return status;
}

HRESULT WINAPI storage_file_Copy ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING folderPath;
    HSTRING filePath;
    WCHAR filePathStr[MAX_PATH];
    WCHAR folderPathStr[MAX_PATH];
    WCHAR uuidName[MAX_PATH];

    IStorageFile *newFile = NULL;
    IStorageItem *invokerFileItem = NULL;
    IStorageItem *destFolderItem = NULL;

    struct storage_file_copy_options *copy_options = (struct storage_file_copy_options *)param;

    //Parameters
    NameCollisionOption option = copy_options->option;
    HSTRING name = copy_options->name;
    
    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( copy_options->folder, &IID_IStorageItem, (void **)&destFolderItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( destFolderItem, &folderPath );
    wcscpy( folderPathStr, WindowsGetStringRawBuffer( folderPath, NULL ) );
    
    status = IStorageFile_QueryInterface( (IStorageFile *)invoker, &IID_IStorageItem, (void **)&invokerFileItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( invokerFileItem, &filePath );
    wcscpy( filePathStr, WindowsGetStringRawBuffer( filePath, NULL ) );

    switch ( option )
    {
        case NameCollisionOption_FailIfExists:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );

            if ( !CopyFileW( filePathStr, folderPathStr, TRUE ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case NameCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof( uuidName ) );
            PathAppendW( folderPathStr, uuidName );

            //Assume FailIfExists by default.
            if ( !CopyFileW( filePathStr, folderPathStr, TRUE ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case NameCollisionOption_ReplaceExisting:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );

            if ( !CopyFileW( filePathStr, folderPathStr, FALSE ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        default:
            status = E_INVALIDARG;
    }
    
    if ( SUCCEEDED( status ) )
    {
        WindowsCreateString( folderPathStr, wcslen( folderPathStr ), &filePath );
        storage_file_AssignFile( filePath, &newFile );
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)newFile;
    }

_CLEANUP:
    WindowsDeleteString( name );
    WindowsDeleteString( filePath );
    WindowsDeleteString( folderPath );
    if ( newFile )
        IStorageFile_Release( newFile );
    if ( invokerFileItem )
        IStorageItem_Release( invokerFileItem );
    if ( destFolderItem )
        IStorageItem_Release( destFolderItem );

    return status;
}

HRESULT WINAPI storage_file_CopyAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING targetPath;
    HSTRING invokerPath;

    IStorageItem *invokerFileItem = NULL;
    IStorageItem *targetFileItem = NULL;
    
    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFile_QueryInterface( (IStorageFile *)invoker, &IID_IStorageItem, (void **)&invokerFileItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( invokerFileItem, &invokerPath );

    status = IStorageFile_QueryInterface( (IStorageFile *)param, &IID_IStorageItem, (void **)&targetFileItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( targetFileItem, &targetPath );

    if ( !CopyFileW( WindowsGetStringRawBuffer( invokerPath, NULL ), WindowsGetStringRawBuffer( targetPath, NULL ), FALSE ) )
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

_CLEANUP:
    WindowsDeleteString( invokerPath );
    WindowsDeleteString( targetPath );
    if ( invokerFileItem )
        IStorageItem_Release( invokerFileItem );
    if ( targetFileItem )
        IStorageItem_Release( targetFileItem );

    return status;
}

HRESULT WINAPI storage_file_Move ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING folderPath;
    HSTRING filePath;
    WCHAR filePathStr[MAX_PATH];
    WCHAR folderPathStr[MAX_PATH];
    WCHAR uuidName[MAX_PATH];

    IStorageItem *invokerFileItem = NULL;
    IStorageItem *destFolderItem = NULL;
    IStorageFile *movedFile = NULL;

    struct storage_file_move_options *move_options = (struct storage_file_move_options *)param;

    //Parameters
    NameCollisionOption option = move_options->option;
    HSTRING name = move_options->name;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( move_options->folder, &IID_IStorageItem, (void **)&destFolderItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( destFolderItem, &folderPath );
    wcscpy( folderPathStr, WindowsGetStringRawBuffer( folderPath, NULL ) );
    
    status = IStorageFile_QueryInterface( (IStorageFile *)invoker, &IID_IStorageItem, (void **)&invokerFileItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( invokerFileItem, &filePath );
    wcscpy( filePathStr, WindowsGetStringRawBuffer( filePath, NULL ) );

    switch ( option )
    {
        case NameCollisionOption_FailIfExists:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );

            if ( !MoveFileW( filePathStr, folderPathStr ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case NameCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof( uuidName ) );
            PathAppendW( folderPathStr, uuidName );

            //Assume FailIfExists by default.
            if ( !MoveFileW( filePathStr, folderPathStr ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        case NameCollisionOption_ReplaceExisting:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );
            if ( PathFileExistsW( folderPathStr ) )
            {
                if ( !DeleteFileW( folderPathStr ) )
                {
                    status = HRESULT_FROM_WIN32( GetLastError() );
                    goto _CLEANUP;
                }
            }

            if ( !MoveFileW( filePathStr, folderPathStr ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                goto _CLEANUP;
            }
            break;

        default:
            status = E_INVALIDARG;
    }

    WindowsDeleteString( filePath );
    WindowsCreateString( folderPathStr, wcslen( folderPathStr ), &filePath );

    status = storage_file_AssignFile( filePath, (IStorageFile **)&movedFile );
    memcpy( (IStorageFile *)invoker, movedFile, sizeof(struct storage_file) );

_CLEANUP:
    WindowsDeleteString( name );
    WindowsDeleteString( folderPath );
    WindowsDeleteString( filePath );
    if ( invokerFileItem )
        IStorageItem_Release( invokerFileItem );
    if ( destFolderItem )
        IStorageItem_Release( destFolderItem );
    if ( movedFile )
        IStorageFile_Release( movedFile );

    return status;
}

HRESULT WINAPI storage_file_MoveAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING invokerPath;
    HSTRING targetPath;

    IStorageItem *targetFileItem = NULL;
    IStorageItem *invokerFileItem = NULL;
    IStorageFile *movedFile = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFile_QueryInterface( (IStorageFile *)invoker, &IID_IStorageItem, (void **)&invokerFileItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( invokerFileItem, &invokerPath );

    status = IStorageFile_QueryInterface( (IStorageFile *)param, &IID_IStorageItem, (void **)&targetFileItem );
    if ( FAILED( status ) ) goto _CLEANUP;
    IStorageItem_get_Path( targetFileItem, &targetPath );

    if ( PathFileExistsW( WindowsGetStringRawBuffer( targetPath, NULL ) ) )
    {
        if ( !DeleteFileW( WindowsGetStringRawBuffer( targetPath, NULL ) ) )
        {
            status = HRESULT_FROM_WIN32( GetLastError() );
            goto _CLEANUP;
        }
    }

    if ( !MoveFileW( WindowsGetStringRawBuffer( invokerPath, NULL ), WindowsGetStringRawBuffer( targetPath, NULL ) ) )
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

    status = storage_file_AssignFile( targetPath, (IStorageFile **)&movedFile );
    memcpy( (IStorageFile *)invoker, movedFile, sizeof(struct storage_file) );

_CLEANUP:
    WindowsDeleteString( invokerPath );
    WindowsDeleteString( targetPath );
    if ( invokerFileItem )
        IStorageItem_Release( invokerFileItem );
    if ( targetFileItem )
        IStorageItem_Release( targetFileItem );

    return status;
}

HRESULT WINAPI storage_file_Open ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING filePath;
    
    IStorageItem *fileItem = NULL;
    IRandomAccessStream *fileStream = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFile_QueryInterface( (IStorageFile *)invoker, &IID_IStorageItem, (void **)&fileItem );
    if ( FAILED( status ) ) return status;

    IStorageItem_get_Path( fileItem, &filePath );

    status = CreateRandomAccessStreamOnFile( WindowsGetStringRawBuffer( filePath, NULL ), FileAccessMode_ReadWrite, &IID_IRandomAccessStream, (void **)&fileStream );
    if ( FAILED( status ) ) return status;

    CHECK_LAST_RESTRICTED_ERROR();

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)fileStream;
    }

    return status;
}

HRESULT WINAPI storage_file_properties_with_availability_IsAvailable ( IStorageItem *fileItem, boolean *value )
{
    HSTRING filePath;

    IStorageItem_get_Path( fileItem, &filePath );

    if ( GetFileAttributesW( WindowsGetStringRawBuffer( filePath, NULL ) ) == INVALID_FILE_ATTRIBUTES )
    {
        *value = FALSE;
    } else
    {
        *value = TRUE;
    }

    return S_OK;
}