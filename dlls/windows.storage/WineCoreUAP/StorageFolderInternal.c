/* WinRT Windows.Storage.StorageFolder Implementation
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

#include "StorageFolderInternal.h"

_ENABLE_DEBUGGING_

static VOID DeleteDirectoryRecursively( LPCWSTR directoryPath )
{
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind;
    WCHAR searchPath[MAX_PATH];
    WCHAR fullPath[MAX_PATH];

    swprintf( searchPath, sizeof(searchPath), L"%s\\*.*", directoryPath );

    hFind = FindFirstFileW( searchPath, &findFileData );
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        return;
    }

    do 
    {
        if ( wcscmp( findFileData.cFileName, L"." ) != 0 && wcscmp( findFileData.cFileName, L".." ) != 0 ) 
        {
            swprintf( fullPath, sizeof(fullPath), L"%s\\%s", directoryPath, findFileData.cFileName );

            if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
            {
                DeleteDirectoryRecursively( fullPath );

                RemoveDirectoryW( fullPath );
            } 
            else 
            {
                if ( !DeleteFileW( fullPath ) ) 
                {
                    return;
                }
            }
        }
    } 
    while ( FindNextFileW( hFind, &findFileData ) != 0 );

    FindClose( hFind );

    if ( !RemoveDirectoryW( directoryPath ) ) 
    {
        return;
    }
}

static VOID GenerateUniqueFileName( LPWSTR buffer, SIZE_T bufferSize ) {
    UUID uuid;
    LPWSTR str;

    UuidCreate( &uuid );
    UuidToStringW( &uuid, (RPC_WSTR*)&str );
    swprintf( buffer, bufferSize, L"%s", str );

    RpcStringFreeW( (RPC_WSTR*)&str );
}

HRESULT WINAPI storage_folder_AssignFolder ( HSTRING path, IStorageFolder **value )
{
    //Must be called from an asynchronous context!
    HRESULT status;
    BOOLEAN isFolder = FALSE;

    IStorageItem *folderItem = NULL;
    IStorageItemProperties *folderItemProperties = NULL;

    struct storage_folder *folder;

    TRACE( "path %p, value %p\n", path, value );

    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageFolder2_iface.lpVtbl = &storage_folder2_vtbl;
    folder->IStorageFolderQueryOperations_iface.lpVtbl = &storage_folder_query_operations_vtbl;
    folder->ref = 1;

    status = storage_item_Internal_CreateNew( path, &folderItem );
    if ( FAILED( status ) )
    {
        free( folder );
        return status;
    }     
    // This copies each corresponding field of storage_item to their respective values in storage_folder.
    // Any changes in either of the structs needs to be reflected on the other.
    memcpy( &folder->IStorageItem_iface, folderItem, sizeof(struct storage_item) );

    IStorageItem_IsOfType( &folder->IStorageItem_iface, StorageItemTypes_Folder, &isFolder );

    if ( !isFolder )
    {
        free( folder );
        status = E_INVALIDARG;
        if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_ISNOTFOLDER ) ) ) )
            return E_UNEXPECTED;
        return status;
    }

    status = storage_item_properties_AssignProperties( folderItem, &folderItemProperties );
    if ( FAILED( status ) )
    {
        free( folder );
        return status;
    } 
    memcpy( &folder->IStorageItemProperties_iface, folderItemProperties, sizeof(struct storage_item_properties) );

    *value = &folder->IStorageFolder_iface;

    IStorageItem_Release( folderItem );
    IStorageItemProperties_Release( folderItemProperties );

    return status;
}

HRESULT WINAPI storage_folder_AssignFolderAsync ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT status;

    IUnknown *unknown = NULL;
    IStorageFolder *folder = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = storage_folder_AssignFolder( (HSTRING)param, &folder );

    IStorageFolder_QueryInterface( folder, &IID_IUnknown, (void **)&unknown );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)folder;
    }

    return status;
}

HRESULT WINAPI storage_folder_CreateFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING Path;
    DWORD attrib;
    WCHAR fullPath[MAX_PATH];
    WCHAR uuidName[MAX_PATH];
    BOOL Exists = FALSE;
    BOOL Replace = FALSE;

    IStorageFolder *resultFolder = NULL;
    IStorageItem *invokerFolderItem = NULL;

    struct storage_folder_creation_options *creation_options = (struct storage_folder_creation_options *)param;

    //Parameters
    CreationCollisionOption collisionOption = creation_options->option;
    HSTRING Name = creation_options->name;

    TRACE( "iface %p, value %p\n", invoker, result );

    if ( WindowsIsStringEmpty( Name ) )
    {
        status = E_INVALIDARG;
        if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_NAMEISSHORT ) ) ) )
            return E_UNEXPECTED;
        return status;
    }

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    IStorageItem_get_Path( invokerFolderItem, &Path );

    wcscpy( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );

    switch ( collisionOption )
    {
        case CreationCollisionOption_FailIfExists:
            PathAppendW( fullPath, WindowsGetStringRawBuffer( Name, NULL ));
            attrib = GetFileAttributesW(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                status = E_INVALIDARG;
            else 
                status = S_OK;
            break;

        case CreationCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof(uuidName) );
            PathAppendW( fullPath, uuidName );

            status = S_OK;
            break;
        
        case CreationCollisionOption_OpenIfExists:
            PathAppendW( fullPath, WindowsGetStringRawBuffer( Name, NULL ));
            attrib = GetFileAttributesW(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Exists = TRUE;
            
            status = S_OK;
            break;
        
        case CreationCollisionOption_ReplaceExisting:
            PathAppendW( fullPath, WindowsGetStringRawBuffer( Name, NULL ));
            attrib = GetFileAttributesW(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Replace = TRUE;

            status = S_OK;
            break;
    }

    if ( SUCCEEDED( status ) )
    {
        if ( Replace )
            DeleteDirectoryRecursively( fullPath );
        if ( !Exists )
        {
            if ( !CreateDirectoryW( fullPath, NULL ) )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                switch( GetLastError() )
                {
                    case ERROR_INVALID_NAME:
                    case ERROR_BAD_PATHNAME:
                        if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_INVALIDNAME ), Name ) ) )
                            return E_UNEXPECTED;
                        return status;

                    case ERROR_FILENAME_EXCED_RANGE:
                        if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_NAMETOOLONG ), Name ) ) )
                            return E_UNEXPECTED;
                        return status;
                    
                    case ERROR_ACCESS_DENIED:
                        if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_SUBNOTALLOWED ) ) ) )
                            return E_UNEXPECTED;
                        return status;

                    default: 
                        return status;
                }
            }
        } 

        WindowsCreateString( fullPath, wcslen( fullPath ), &Path );
        status = storage_folder_AssignFolder( Path, &resultFolder );
    }

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)resultFolder;
    } else {
        IStorageFolder_Release( resultFolder );
    }

    return status;
}

HRESULT WINAPI storage_folder_CreateFile( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING Path;
    HANDLE createdFileHandle;
    DWORD attrib;
    WCHAR fullPath[MAX_PATH];
    WCHAR uuidName[MAX_PATH];
    BOOL Exists = FALSE;
    BOOL Replace = FALSE;

    IStorageFile *resultFile = NULL;

    struct storage_folder_creation_options *creation_options = (struct storage_folder_creation_options *)param;

    //Parameters
    CreationCollisionOption collisionOption = creation_options->option;
    HSTRING Name = creation_options->name;

    TRACE( "iface %p, value %p\n", invoker, result );

    if ( WindowsIsStringEmpty( Name ) )
    {
        status = E_INVALIDARG;
        if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_NAMEISSHORT ) ) ) )
            return E_UNEXPECTED;
        return status;
    }

    WindowsDuplicateString( impl_from_IStorageFolder( (IStorageFolder *)invoker )->Path, &Path );

    wcscpy( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );

    switch ( collisionOption )
    {
        case CreationCollisionOption_FailIfExists:
            PathAppendW( fullPath, WindowsGetStringRawBuffer( Name, NULL ));
            attrib = GetFileAttributesW(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                status = E_INVALIDARG;
            else 
                status = S_OK;
            break;

        case CreationCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof(uuidName) );
            PathAppendW( fullPath, uuidName );

            status = S_OK;
            break;
        
        case CreationCollisionOption_OpenIfExists:
            PathAppendW( fullPath, WindowsGetStringRawBuffer( Name, NULL ));
            attrib = GetFileAttributesW(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Exists = TRUE;
            
            status = S_OK;
            break;
        
        case CreationCollisionOption_ReplaceExisting:
            PathAppendW( fullPath, WindowsGetStringRawBuffer( Name, NULL ));
            attrib = GetFileAttributesW(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Replace = TRUE;

            status = S_OK;
            break;
    }
    if ( SUCCEEDED( status ) )
    {
        if ( Replace )
        {
            if ( !DeleteFileW( fullPath ) )
            {
                return HRESULT_FROM_WIN32( GetLastError() );
            }
        }
        if ( !Exists )
        {
            createdFileHandle = CreateFileW( fullPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            if ( createdFileHandle == INVALID_HANDLE_VALUE )
            {
                status = HRESULT_FROM_WIN32( GetLastError() );
                switch( GetLastError() )
                {
                    case ERROR_INVALID_NAME:
                    case ERROR_BAD_PATHNAME:
                        if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_INVALIDNAME ), Name ) ) )
                            return E_UNEXPECTED;
                        return status;

                    case ERROR_FILENAME_EXCED_RANGE:
                        if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_NAMETOOLONG ), Name ) ) )
                            return E_UNEXPECTED;
                        return status;

                    case ERROR_ACCESS_DENIED:
                        if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_FILENOTALLOWED ) ) ) )
                            return E_UNEXPECTED;
                        return status;

                    default: 
                        return status;
                }
            } else
            {
                CloseHandle( createdFileHandle );
            }
        }
        WindowsCreateString( fullPath, wcslen( fullPath ), &Path );
        status = storage_file_AssignFile( Path, &resultFile );
    }

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)resultFile;
    }

    return status;
}

HRESULT WINAPI storage_folder_FetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT status;
    HSTRING Path;
    HSTRING itemPath;
    BOOLEAN isFolder = FALSE;
    WCHAR fullPath[MAX_PATH];

    IStorageFolder *newFolder = NULL;
    IStorageFile *newFile = NULL;
    IStorageItem *newFileItem = NULL;
    IStorageItem *invokerFolderItem = NULL;
    IStorageItem *newFolderItem = NULL;
    IStorageItem *item = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    if ( FAILED( status ) ) 
        return status;

    IStorageItem_get_Path( invokerFolderItem, &Path );

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &itemPath );

    status = storage_item_Internal_CreateNew( itemPath, &item );
    
    if ( SUCCEEDED( status ) )
    {
        IStorageItem_IsOfType( item, StorageItemTypes_Folder, &isFolder );
        if ( isFolder )
        {
            storage_folder_AssignFolder( itemPath, &newFolder );
            IStorageFolder_QueryInterface( newFolder, &IID_IStorageItem, (void **)&newFolderItem );
            result->vt = VT_UNKNOWN;
            result->punkVal = (IUnknown *)newFolderItem;
        } else
        {
            storage_file_AssignFile( itemPath, &newFile );
            IStorageFile_QueryInterface( newFile, &IID_IStorageItem, (void **)&newFileItem );
            result->vt = VT_UNKNOWN;
            result->punkVal = (IUnknown *)newFileItem;
        }
    } else 
    {
        if ( status == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
        {
            WindowsDeleteString( itemPath );
            if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_NAMENOTFOUND ), (HSTRING)param ) ) )
                return E_UNEXPECTED;
            return status;
        }
    }
        
    if ( item )
        IStorageItem_Release( item );
    WindowsDeleteString( itemPath );

    return status;
}

HRESULT WINAPI storage_folder_FetchItemsAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAW findFileData;
    BOOLEAN isFolder = FALSE;
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING itemPath;    
    HANDLE hFind = INVALID_HANDLE_VALUE;    
    WCHAR searchPath[MAX_PATH]; 
    WCHAR fullItemPath[MAX_PATH];

    IStorageFolder *newFolder = NULL;
    IStorageFile *newFile = NULL;
    IStorageItem *newFolderItem = NULL;
    IStorageItem *newFileItem = NULL;
    IStorageItem *invokerFolderItem = NULL;
    IStorageItem *item = NULL;
    IVector_IStorageItem *vector = NULL;
    IVectorView_IStorageItem *vectorView = NULL;

    //non-runtime class require redefinition
    #undef IStorageItem
    DEFINE_VECTOR_IIDS( IStorageItem );
    #define IStorageItem __x_ABI_CWindows_CStorage_CIStorageItem

    status = vector_create( &IStorageItem_iids, (void **)&vector );
    if ( FAILED( status ) ) return status;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    if ( FAILED( status ) ) 
    {
        IVector_IStorageItem_Release( vector );
        return status;
    }
    IStorageItem_get_Path( invokerFolderItem, &Path );

    swprintf( searchPath, MAX_PATH, L"%s\\*.*", WindowsGetStringRawBuffer( Path, NULL ) );

    hFind = FindFirstFileW( searchPath, &findFileData );

    if ( hFind == INVALID_HANDLE_VALUE ) 
    {
        WindowsDeleteString( Path );
        IVector_IStorageItem_Release( vector );
        return E_ABORT;
    } 

    // This is quite weird. FindNextFileW on Windows first iterates through 
    // Folders and then Files. but somehow on Wine, it's the exact inverse.
    // For conformance sake, this loop had to be written twice.
    do
    {
        if ( wcscmp( findFileData.cFileName, L"." ) != 0 
          && wcscmp( findFileData.cFileName, L".." ) != 0 ) 
        {
            PathAppendW( fullItemPath, WindowsGetStringRawBuffer( Path, NULL ) );
            PathAppendW( fullItemPath, findFileData.cFileName );
            WindowsCreateString( fullItemPath, wcslen( fullItemPath ), &itemPath);

            status = storage_item_Internal_CreateNew( itemPath, &item );
            if ( SUCCEEDED( status ) )
            {
                IStorageItem_IsOfType( item, StorageItemTypes_Folder, &isFolder );
                if ( isFolder )
                {
                    storage_folder_AssignFolder( itemPath, &newFolder );
                    IStorageFolder_QueryInterface( newFolder, &IID_IStorageItem, (void **)&newFolderItem );
                    IVector_IStorageItem_Append( vector, newFolderItem );
                }
            } else
                break;

            WindowsDeleteString( itemPath );
            if ( item )
                IStorageItem_Release( item );
            SecureZeroMemory( fullItemPath, sizeof( fullItemPath ) );
        }
    } while ( FindNextFileW( hFind, &findFileData ) != 0 );

    hFind = FindFirstFileW( searchPath, &findFileData );

    do
    {
        if ( wcscmp( findFileData.cFileName, L"." ) != 0 
          && wcscmp( findFileData.cFileName, L".." ) != 0 ) 
        {
            PathAppendW( fullItemPath, WindowsGetStringRawBuffer( Path, NULL ) );
            PathAppendW( fullItemPath, findFileData.cFileName );
            WindowsCreateString( fullItemPath, wcslen( fullItemPath ), &itemPath);

            status = storage_item_Internal_CreateNew( itemPath, &item );
            if ( SUCCEEDED( status ) )
            {
                IStorageItem_IsOfType( item, StorageItemTypes_Folder, &isFolder );
                if ( !isFolder )
                {
                    storage_file_AssignFile( itemPath, &newFile );
                    IStorageFile_QueryInterface( newFile, &IID_IStorageItem, (void **)&newFileItem );
                    IVector_IStorageItem_Append( vector, newFileItem );
                }
            } else
                break;

            WindowsDeleteString( itemPath );
            if ( item )
                IStorageItem_Release( item );
            SecureZeroMemory( fullItemPath, sizeof( fullItemPath ) );
        }
    } while ( FindNextFileW( hFind, &findFileData ) != 0 );

    FindClose( hFind );

    if ( SUCCEEDED( status ) )
    {    
        IVector_IStorageItem_GetView( vector, &vectorView );
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)vectorView;
    } else 
        IVector_IStorageItem_Release( vector );

    return status;
}

HRESULT WINAPI storage_folder_FetchFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status;
    HSTRING Path;
    HSTRING folderPath;
    WCHAR fullPath[MAX_PATH];

    IStorageFolder *folderToFetch = NULL;
    IStorageItem *invokerFolderItem = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    if ( FAILED( status ) ) return status;
    IStorageItem_get_Path( invokerFolderItem, &Path );

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &folderPath );

    status = storage_folder_AssignFolder( folderPath, &folderToFetch );
    if ( FAILED( status ) )
    {
        WindowsDeleteString( folderPath );
        if ( status == HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) )
            if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_NAMENOTFOUND ), (HSTRING)param ) ) )
                return E_UNEXPECTED;
        return status;
    }
    
    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)folderToFetch;
    } else {
        IStorageFolder_Release( folderToFetch );
    }

    WindowsDeleteString( folderPath );

    return status;
}

HRESULT WINAPI storage_folder_FetchFoldersAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAW findFolderData;
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING folderPath;    
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WCHAR searchPath[MAX_PATH];
    WCHAR fullFolderPath[MAX_PATH];

    IStorageFolder *folder = NULL;
    IStorageItem *invokerFolderItem = NULL;
    IVector_StorageFolder *vector = NULL;
    IVectorView_StorageFolder *vectorView = NULL;

    DEFINE_VECTOR_IIDS( StorageFolder );
    status = vector_create( &StorageFolder_iids, (void **)&vector );
    if ( FAILED( status ) ) return status;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    if ( FAILED( status ) )
    {
        IVector_StorageFolder_Release( vector );
        return status;
    }
    IStorageItem_get_Path( invokerFolderItem, &Path );

    swprintf( searchPath, MAX_PATH, L"%s\\*.*", WindowsGetStringRawBuffer( Path, NULL ) );

    hFind = FindFirstFileW( searchPath, &findFolderData );

    if ( hFind == INVALID_HANDLE_VALUE ) 
    {
        WindowsDeleteString( Path );
        IVector_StorageFolder_Release( vector );
        return E_ABORT;
    } 

    do {
        if ( wcscmp( findFolderData.cFileName, L"." ) != 0 
          && wcscmp( findFolderData.cFileName, L".." ) != 0 
          && findFolderData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
        {
            PathAppendW( fullFolderPath, WindowsGetStringRawBuffer( Path, NULL ) );
            PathAppendW( fullFolderPath, findFolderData.cFileName );
            WindowsCreateString( fullFolderPath, wcslen( fullFolderPath ), &folderPath );

            status = storage_folder_AssignFolder( folderPath, &folder );

            if ( FAILED( status ) )
                break;
                    
            IVector_StorageFolder_Append( vector, folder );

            WindowsDeleteString( folderPath );
            SecureZeroMemory( fullFolderPath, sizeof( fullFolderPath ) );
        }
    } while ( FindNextFileW( hFind, &findFolderData ) != 0 );

    if ( SUCCEEDED( status ) )
    {
        IVector_StorageFolder_GetView( vector, &vectorView );
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)vectorView;
    } else 
        IVector_StorageFolder_Release( vector );

    return status;
}

HRESULT WINAPI storage_folder_FetchFile( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WCHAR fullPath[MAX_PATH];
    HRESULT status;
    HSTRING Path;
    HSTRING folderPath;
    BOOLEAN isFile = FALSE;
    
    IStorageFile *fileToFetch = NULL;
    IStorageItem *fileItemToFetch = NULL;
    IStorageItem *invokerFolderItem = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    if ( FAILED( status ) ) return status;
    IStorageItem_get_Path( invokerFolderItem, &Path );

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &folderPath );

    status = storage_file_AssignFile( folderPath, &fileToFetch );
    if ( FAILED( status ) )
    {
        WindowsDeleteString( folderPath );
        return status;
    }
    IStorageFile_QueryInterface( fileToFetch, &IID_IStorageItem, (void **)&fileItemToFetch );
    IStorageItem_IsOfType( fileItemToFetch, StorageItemTypes_File, &isFile );

    if ( !isFile )
        status = E_INVALIDARG;
    
    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)fileToFetch;
    } else {
        IStorageFile_Release( fileToFetch );
    }

    return status;
}

HRESULT WINAPI storage_folder_FetchFilesAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAW findFileData;    
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING filePath;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WCHAR searchPath[MAX_PATH]; 
    WCHAR fullFilePath[MAX_PATH];

    IStorageFile *file = NULL;
    IStorageItem *invokerFolderItem = NULL;
    IVector_StorageFile *vector = NULL;
    IVectorView_StorageFile *vectorView = NULL;

    DEFINE_VECTOR_IIDS( StorageFile );
    status = vector_create( &StorageFile_iids, (void **)&vector );
    if ( FAILED( status ) ) return status;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    if ( FAILED( status ) )
    {
        IVector_StorageFile_Release( vector );
        return status;
    }
    IStorageItem_get_Path( invokerFolderItem, &Path );

    swprintf( searchPath, MAX_PATH, L"%s\\*.*", WindowsGetStringRawBuffer( Path, NULL ) );

    hFind = FindFirstFileW( searchPath, &findFileData );

    if ( hFind == INVALID_HANDLE_VALUE ) 
    {
        IVector_StorageFile_Release( vector );
        WindowsDeleteString( Path );
        return E_ABORT;
    } 
    
    do {
        if ( wcscmp( findFileData.cFileName, L"." ) != 0 
          && wcscmp( findFileData.cFileName, L".." ) != 0
          && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
        {
            PathAppendW( fullFilePath, WindowsGetStringRawBuffer( Path, NULL ) );
            PathAppendW( fullFilePath, findFileData.cFileName );
            WindowsCreateString( fullFilePath, wcslen( fullFilePath ), &filePath );

            status = storage_file_AssignFile( filePath, &file );

            if ( FAILED( status ) )
                break;

            IVector_StorageFile_Append( vector, file );
    
            WindowsDeleteString( filePath );
            SecureZeroMemory( fullFilePath, sizeof( fullFilePath ) );
        }
    } while ( FindNextFileW( hFind, &findFileData ) != 0 );

    if ( SUCCEEDED( status ) )
    {    
        IVector_StorageFile_GetView( vector, &vectorView );
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)vectorView;
    } else
        IVector_StorageFile_Release( vector );

    if ( FAILED( status ) )
    {
        IVector_StorageFile_Release( vector );
    }

    return status;
}

HRESULT WINAPI storage_folder2_TryFetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT status;
    HSTRING Path;
    HSTRING itemPath;
    BOOLEAN isFolder = FALSE;
    WCHAR fullPath[MAX_PATH];

    IStorageFolder *newFolder = NULL;
    IStorageFile *newFile = NULL;
    IStorageItem *newFileItem = NULL;
    IStorageItem *invokerFolderItem = NULL;
    IStorageItem *newFolderItem = NULL;
    IStorageItem *item = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );

    status = IStorageFolder_QueryInterface( (IStorageFolder *)invoker, &IID_IStorageItem, (void **)&invokerFolderItem );
    if ( FAILED( status ) ) 
    {
        free( item );
        return status;    
    }
    IStorageItem_get_Path( invokerFolderItem, &Path );

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &itemPath );

    status = storage_item_Internal_CreateNew( itemPath, &item );
    
    if ( SUCCEEDED( status ) )
    {
        IStorageItem_IsOfType( item, StorageItemTypes_Folder, &isFolder );
        if ( isFolder )
        {
            storage_folder_AssignFolder( itemPath, &newFolder );
            IStorageFolder_QueryInterface( newFolder, &IID_IStorageItem, (void **)&newFolderItem );
            result->vt = VT_UNKNOWN;
            result->punkVal = (IUnknown *)newFolderItem;
        } else
        {
            storage_file_AssignFile( itemPath, &newFile );
            IStorageFile_QueryInterface( newFile, &IID_IStorageItem, (void **)&newFileItem );
            result->vt = VT_UNKNOWN;
            result->punkVal = (IUnknown *)newFileItem;
        }
    } else {
        result->vt = VT_NULL;
    }

    if ( item )
        IStorageItem_Release( item );
    WindowsDeleteString( itemPath );

    return S_OK;
}
