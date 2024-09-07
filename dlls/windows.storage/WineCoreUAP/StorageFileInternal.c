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

#include "util.h"
#include "StorageFileInternal.h"

#define BUFFER_SIZE 512

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

HRESULT WINAPI storage_file_Internal_AssignFile ( HSTRING filePath, IStorageFile * result )
{
    HRESULT status;
    HSTRING path;
    FILE *fileData;
    BYTE buffer[BUFFER_SIZE];
    DWORD bytesRead;
    LPWSTR pwsMimeOut;
    BOOLEAN isFile;
    LPSTR fileExtension;

    struct storage_file *file;

    bytesRead = 0;
    pwsMimeOut = NULL;
    
    if (!result) return E_INVALIDARG;
    if (!(file = calloc( 1, sizeof(*file) ))) return E_OUTOFMEMORY;

    file = impl_from_IStorageFile( result );

    file->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    file->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    file->ref = 1;

    WindowsDuplicateString( filePath, &path );

    status = storage_item_Internal_CreateNew( path, &file->IStorageItem_iface );

    IStorageItem_IsOfType( &file->IStorageItem_iface, StorageItemTypes_File, &isFile );
    if ( !isFile )
        status = E_INVALIDARG;

    if ( SUCCEEDED( status ) )
    {
        fileExtension = strrchr( HStringToLPCSTR( path ), '.' );
        if ( fileExtension && fileExtension != HStringToLPCSTR( path ) )
        {
            WindowsCreateString( CharToLPCWSTR( fileExtension ), wcslen( CharToLPCWSTR( fileExtension ) ), &file->FileType );
        }
        fileData = fopen( HStringToLPCSTR( path ), "rb" );
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

    if (!(file = calloc( 1, sizeof(*file) ))) return E_OUTOFMEMORY;

    file->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    file->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    file->ref = 1;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = storage_file_Internal_AssignFile( (HSTRING)param, &file->IStorageFile_iface );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&file->IStorageFile_iface;
    }

    return status;
}

HRESULT WINAPI storage_file_Copy ( IStorageFile *invoker, IStorageFolder *folder, HSTRING name, NameCollisionOption option, HSTRING *destPath )
{
    HRESULT status = S_OK;
    HSTRING folderPath;
    HSTRING filePath;
    CHAR filePathStr[MAX_PATH];
    CHAR folderPathStr[MAX_PATH];
    CHAR uuidName[MAX_PATH];

    struct storage_folder *destFolder;
    struct storage_item *destFolderItem;
    struct storage_item *invokerFileItem;
    struct storage_file *invokerFile;


    destFolder = impl_from_IStorageFolder( folder );
    destFolderItem = impl_from_IStorageItem( &destFolder->IStorageItem_iface );
    WindowsDuplicateString( destFolderItem->Path, &folderPath );
    strcpy( folderPathStr, HStringToLPCSTR( folderPath ) );
    
    invokerFile = impl_from_IStorageFile( invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    WindowsDuplicateString( invokerFileItem->Path, &filePath );
    strcpy( filePathStr, HStringToLPCSTR( filePath ) );


    switch ( option )
    {
        case NameCollisionOption_FailIfExists:
            PathAppendA( folderPathStr, HStringToLPCSTR( name ) );

            if ( !CopyFileA( filePathStr, folderPathStr, TRUE ) )
                return E_ABORT;
            break;

        case NameCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof( uuidName ) );
            PathAppendA( folderPathStr, uuidName );

            //Assume FailIfExists by default.
            if ( !CopyFileA( filePathStr, folderPathStr, TRUE ) )
                return E_ABORT;
            break;

        case NameCollisionOption_ReplaceExisting:
            PathAppendA( folderPathStr, HStringToLPCSTR( name ) );

            if ( !CopyFileA( filePathStr, folderPathStr, FALSE ) )
                return E_ABORT;
            break;

        default:
            status = E_INVALIDARG;
    }

    if ( SUCCEEDED( status ) )
    {
        WindowsCreateString( CharToLPCWSTR( folderPathStr ), wcslen( CharToLPCWSTR( folderPathStr ) ), destPath );
    }

    return status;
}

HRESULT WINAPI storage_file_CopyAndReplace ( IStorageFile *invoker, IStorageFile *target )
{
    HRESULT status = S_OK;
    LPCSTR invokerPath;
    LPCSTR targetPath;
    struct storage_file *invokerFile;
    struct storage_file *targetFile;
    struct storage_item *invokerFileItem;
    struct storage_item *targetFileItem;

    invokerFile = impl_from_IStorageFile( invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    invokerPath = HStringToLPCSTR( invokerFileItem->Path );

    targetFile = impl_from_IStorageFile( target );
    targetFileItem = impl_from_IStorageItem( &targetFile->IStorageItem_iface );
    targetPath = HStringToLPCSTR( targetFileItem->Path );

    if ( !CopyFileA( invokerPath, targetPath, FALSE ) )
        return E_ABORT;

    *targetFile = *invokerFile;

    status = storage_item_Internal_CreateNew( targetFileItem->Path, &targetFile->IStorageItem_iface );

    return status;
}

HRESULT WINAPI storage_file_Move ( IStorageFile *invoker, IStorageFolder *folder, HSTRING name, NameCollisionOption option )
{
    HRESULT status = S_OK;
    HSTRING folderPath;
    HSTRING filePath;
    HSTRING destPath;
    CHAR filePathStr[MAX_PATH];
    CHAR folderPathStr[MAX_PATH];
    CHAR uuidName[MAX_PATH];

    struct storage_folder *destFolder;
    struct storage_item *destFolderItem;
    struct storage_item *invokerFileItem;
    struct storage_file *invokerFile;


    destFolder = impl_from_IStorageFolder( folder );
    destFolderItem = impl_from_IStorageItem( &destFolder->IStorageItem_iface );
    WindowsDuplicateString( destFolderItem->Path, &folderPath );
    strcpy( folderPathStr, HStringToLPCSTR( folderPath ) );
    
    invokerFile = impl_from_IStorageFile( invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    WindowsDuplicateString( invokerFileItem->Path, &filePath );
    strcpy( filePathStr, HStringToLPCSTR( filePath ) );


    switch ( option )
    {
        case NameCollisionOption_FailIfExists:
            PathAppendA( folderPathStr, HStringToLPCSTR( name ) );

            if ( !MoveFileA( filePathStr, folderPathStr ) )
                return E_ABORT;
            break;

        case NameCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof( uuidName ) );
            PathAppendA( folderPathStr, uuidName );

            //Assume FailIfExists by default.
            if ( !MoveFileA( filePathStr, folderPathStr ) )
                return E_ABORT;
            break;

        case NameCollisionOption_ReplaceExisting:
            PathAppendA( folderPathStr, HStringToLPCSTR( name ) );

            if ( !MoveFileA( filePathStr, folderPathStr ) )
                return E_ABORT;
            break;

        default:
            status = E_INVALIDARG;
    }

    if ( SUCCEEDED( status ) )
    {
        WindowsCreateString( CharToLPCWSTR( folderPathStr ), wcslen( CharToLPCWSTR( folderPathStr ) ), &destPath );
        storage_item_Internal_CreateNew( destPath, &invokerFile->IStorageItem_iface );
    }

    return status;
}

HRESULT WINAPI storage_file_MoveAndReplace ( IStorageFile *invoker, IStorageFile *target )
{
    HRESULT status = S_OK;
    LPCSTR invokerPath;
    LPCSTR targetPath;
    struct storage_file *invokerFile;
    struct storage_file *targetFile;
    struct storage_item *invokerFileItem;
    struct storage_item *targetFileItem;

    invokerFile = impl_from_IStorageFile( invoker );
    invokerFileItem = impl_from_IStorageItem( &invokerFile->IStorageItem_iface );
    invokerPath = HStringToLPCSTR( invokerFileItem->Path );

    targetFile = impl_from_IStorageFile( target );
    targetFileItem = impl_from_IStorageItem( &targetFile->IStorageItem_iface );
    targetPath = HStringToLPCSTR( targetFileItem->Path );

    if ( !MoveFileA( invokerPath, targetPath ) )
        return E_ABORT;

    *targetFile = *invokerFile;

    status = storage_item_Internal_CreateNew( targetFileItem->Path, &targetFile->IStorageItem_iface );

    return status;
}