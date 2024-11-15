/* WinRT Windows.Storage.StorageFile Implementation
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

HRESULT WINAPI storage_file_AssignFile ( HSTRING filePath, IStorageFile * result )
{
    HRESULT status;
    HSTRING path;
    FILE *fileData;
    BYTE buffer[BUFFER_SIZE];
    DWORD bytesRead;
    LPWSTR pwsMimeOut;
    BOOLEAN isFile;
    LPWSTR fileExtension;

    struct storage_file *file;

    TRACE( "filePath %p, value %p\n", filePath, result );

    bytesRead = 0;
    pwsMimeOut = NULL;
    
    if (!result) return E_INVALIDARG;
    if (!(file = calloc( 1, sizeof(*file) ))) return E_OUTOFMEMORY;

    file = impl_from_IStorageFile( result );

    file->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    file->ref = 1;

    WindowsDuplicateString( filePath, &path );

    status = storage_item_Internal_CreateNew( path, &file->IStorageItem_iface );

    file->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    file->IStorageFilePropertiesWithAvailability_iface.lpVtbl = &storage_file_properties_with_availability_vtbl;

    if ( SUCCEEDED(status) )
        status = storage_item_properties_AssignProperties( &file->IStorageItem_iface, &file->IStorageItemProperties_iface );

    IStorageItem_IsOfType( &file->IStorageItem_iface, StorageItemTypes_File, &isFile );
    if ( !isFile )
        status = E_INVALIDARG;

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

        FindMimeFromData ( NULL, WindowsGetStringRawBuffer( path, NULL ), buffer, bytesRead, NULL, 0, &pwsMimeOut, 0 );
        if (pwsMimeOut != NULL)
            WindowsCreateString( pwsMimeOut, wcslen( pwsMimeOut ), &file->ContentType );

        CoTaskMemFree( pwsMimeOut );
    }

    return status;
}

HRESULT WINAPI storage_file_AssignFileAsync ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;

    struct storage_file *file;

    TRACE( "iface %p, value %p\n", invoker, result );

    if (!(file = calloc( 1, sizeof(*file) ))) return E_OUTOFMEMORY;

    file->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    file->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    file->IStorageFilePropertiesWithAvailability_iface.lpVtbl = &storage_file_properties_with_availability_vtbl;
    file->ref = 1;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = storage_file_AssignFile( (HSTRING)param, &file->IStorageFile_iface );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)&file->IStorageFile_iface;
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

    struct storage_file * newFile;
    struct storage_file_copy_options *copy_options = (struct storage_file_copy_options *)param;

    //Parameters
    IStorageFolder *folder = copy_options->folder;
    NameCollisionOption option = copy_options->option;
    HSTRING name = copy_options->name;

    struct storage_folder *destFolder;
    struct storage_item *destFolderItem;
    struct storage_item *invokerFileItem;
    struct storage_file *invokerFile;
    
    TRACE( "iface %p, value %p\n", invoker, result );

    if (!(newFile = calloc( 1, sizeof(*newFile) ))) return E_OUTOFMEMORY;

    newFile->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    newFile->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    newFile->IStorageFilePropertiesWithAvailability_iface.lpVtbl = &storage_file_properties_with_availability_vtbl;
    newFile->ref = 1;

    destFolder = impl_from_IStorageFolder( folder );
    destFolderItem = impl_from_IStorageItem( &destFolder->IStorageItem_iface );
    WindowsDuplicateString( destFolderItem->Path, &folderPath );
    wcscpy( folderPathStr, WindowsGetStringRawBuffer( folderPath, NULL ) );
    
    invokerFile = impl_from_IStorageFile( (IStorageFile *)invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    WindowsDuplicateString( invokerFileItem->Path, &filePath );
    wcscpy( filePathStr, WindowsGetStringRawBuffer( filePath, NULL ) );


    switch ( option )
    {
        case NameCollisionOption_FailIfExists:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );

            if ( !CopyFileW( filePathStr, folderPathStr, TRUE ) )
                return E_ABORT;
            break;

        case NameCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof( uuidName ) );
            PathAppendW( folderPathStr, uuidName );

            //Assume FailIfExists by default.
            if ( !CopyFileW( filePathStr, folderPathStr, TRUE ) )
                return E_ABORT;
            break;

        case NameCollisionOption_ReplaceExisting:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );

            if ( !CopyFileW( filePathStr, folderPathStr, FALSE ) )
                return E_ABORT;
            break;

        default:
            status = E_INVALIDARG;
    }

    WindowsCreateString( folderPathStr, wcslen( folderPathStr ), &filePath );

    storage_file_AssignFile( filePath, &newFile->IStorageFile_iface );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)&newFile->IStorageFile_iface;
    }

    return status;
}

HRESULT WINAPI storage_file_CopyAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    LPCWSTR invokerPath;
    LPCWSTR targetPath;
    struct storage_file *invokerFile;
    struct storage_file *targetFile;
    struct storage_item *invokerFileItem;
    struct storage_item *targetFileItem;
    
    TRACE( "iface %p, value %p\n", invoker, result );

    invokerFile = impl_from_IStorageFile( (IStorageFile *)invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    invokerPath = WindowsGetStringRawBuffer( invokerFileItem->Path, NULL );

    targetFile = impl_from_IStorageFile( (IStorageFile *)param );
    targetFileItem = impl_from_IStorageItem( &targetFile->IStorageItem_iface );
    targetPath = WindowsGetStringRawBuffer( targetFileItem->Path, NULL );

    if ( !CopyFileW( invokerPath, targetPath, FALSE ) )
        return E_ABORT;

    *targetFile = *invokerFile;

    status = storage_file_AssignFile( targetFileItem->Path, &targetFile->IStorageFile_iface );

    return status;
}

HRESULT WINAPI storage_file_Move ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING folderPath;
    HSTRING filePath;
    HSTRING destPath;
    WCHAR filePathStr[MAX_PATH];
    WCHAR folderPathStr[MAX_PATH];
    WCHAR uuidName[MAX_PATH];

    struct storage_file_move_options *move_options = (struct storage_file_move_options *)param;

    //Parameters
    IStorageFolder *folder = move_options->folder;
    NameCollisionOption option = move_options->option;
    HSTRING name = move_options->name;

    struct storage_folder *destFolder;
    struct storage_item *destFolderItem;
    struct storage_item *invokerFileItem;
    struct storage_file *invokerFile;

    TRACE( "iface %p, value %p\n", invoker, result );

    destFolder = impl_from_IStorageFolder( folder );
    destFolderItem = impl_from_IStorageItem( &destFolder->IStorageItem_iface );
    WindowsDuplicateString( destFolderItem->Path, &folderPath );
    wcscpy( folderPathStr, WindowsGetStringRawBuffer( folderPath, NULL ) );
    
    invokerFile = impl_from_IStorageFile( (IStorageFile *)invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    WindowsDuplicateString( invokerFileItem->Path, &filePath );
    wcscpy( filePathStr, WindowsGetStringRawBuffer( filePath, NULL ) );


    switch ( option )
    {
        case NameCollisionOption_FailIfExists:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );

            if ( !MoveFileW( filePathStr, folderPathStr ) )
                return E_ABORT;
            break;

        case NameCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof( uuidName ) );
            PathAppendW( folderPathStr, uuidName );

            //Assume FailIfExists by default.
            if ( !MoveFileW( filePathStr, folderPathStr ) )
                return E_ABORT;
            break;

        case NameCollisionOption_ReplaceExisting:
            PathAppendW( folderPathStr, WindowsGetStringRawBuffer( name, NULL ) );

            if ( !MoveFileW( filePathStr, folderPathStr ) )
                return E_ABORT;
            break;

        default:
            status = E_INVALIDARG;
    }

    if ( SUCCEEDED( status ) )
    {
        WindowsCreateString( folderPathStr, wcslen( folderPathStr ), &destPath );
        storage_file_AssignFile( destPath, &invokerFile->IStorageFile_iface );
    }

    return status;
}

HRESULT WINAPI storage_file_MoveAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    LPCWSTR invokerPath;
    LPCWSTR targetPath;
    struct storage_file *invokerFile;
    struct storage_file *targetFile;
    struct storage_item *invokerFileItem;
    struct storage_item *targetFileItem;

    TRACE( "iface %p, value %p\n", invoker, result );

    invokerFile = impl_from_IStorageFile( (IStorageFile *)invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    invokerPath = WindowsGetStringRawBuffer( invokerFileItem->Path, NULL );

    targetFile = impl_from_IStorageFile( (IStorageFile *)param );
    targetFileItem = impl_from_IStorageItem( &targetFile->IStorageItem_iface );
    targetPath = WindowsGetStringRawBuffer( targetFileItem->Path, NULL );

    if ( !MoveFileW( invokerPath, targetPath ) )
        return E_ABORT;

    *targetFile = *invokerFile;

    status = storage_file_AssignFile( targetFileItem->Path, &targetFile->IStorageFile_iface );

    return status;
}

HRESULT WINAPI storage_file_properties_with_availability_IsAvailable ( IStorageItem *fileItem, boolean *value )
{
    if ( GetFileAttributesW( WindowsGetStringRawBuffer( impl_from_IStorageItem( fileItem )->Path, NULL ) ) == INVALID_FILE_ATTRIBUTES )
    {
        *value = FALSE;
    } else
    {
        *value = TRUE;
    }

    return S_OK;
}