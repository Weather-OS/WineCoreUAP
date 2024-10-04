/* WinRT Windows.Storage.StorageFolder Implementation
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

#include "StorageFolderInternal.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

HRESULT WINAPI storage_folder_AssignFolder ( HSTRING path, IStorageFolder *value )
{
    //MUST BE CALLED FROM AN ASYNCHRONOUS CONTEXT!
    HRESULT status;
    BOOLEAN isFolder;

    struct storage_folder *folder;

    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder = impl_from_IStorageFolder( value );

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageFolder2_iface.lpVtbl = &storage_folder2_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    status = storage_item_Internal_CreateNew( path, &folder->IStorageItem_iface );

    IStorageItem_IsOfType( &folder->IStorageItem_iface, StorageItemTypes_Folder, &isFolder );

    if ( !isFolder )
        status = E_INVALIDARG;

    return status;
}

HRESULT WINAPI storage_folder_AssignFolderAsync ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT status;
    
    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    TRACE( "iface %p, value %p\n", invoker, result );
    if (!result) return E_INVALIDARG;

    status = storage_folder_AssignFolder( (HSTRING)param, &folder->IStorageFolder_iface );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&folder->IStorageFolder_iface;
    }

    return status;
}

HRESULT WINAPI storage_folder_CreateFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING Path;
    DWORD attrib;
    BOOL Exists = FALSE;
    BOOL Replace = FALSE;
    WCHAR fullPath[MAX_PATH];
    WCHAR uuidName[MAX_PATH];

    struct storage_folder_creation_options *creation_options = (struct storage_folder_creation_options *)param;
    struct storage_folder *invokerFolder;
    struct storage_folder *resultFolder;

    //Parameters
    CreationCollisionOption collisionOption = creation_options->option;
    HSTRING Name = creation_options->name;

    if (!(resultFolder = calloc( 1, sizeof(*resultFolder) ))) return E_OUTOFMEMORY;

    resultFolder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    resultFolder->IStorageFolder2_iface.lpVtbl = &storage_folder2_vtbl;
    resultFolder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    resultFolder->ref = 1;

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

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
            DeleteDirectoryRecursively( fullPath );
        }
        if ( !Exists )
        {
            if ( !CreateDirectoryW( fullPath, NULL ) )
            {
                return E_ABORT;
            }
            status = S_OK;
        }
    }

    WindowsCreateString( fullPath, wcslen( fullPath ), &Path );

    status = storage_folder_AssignFolder( Path, &resultFolder->IStorageFolder_iface );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&resultFolder->IStorageFolder_iface;
    }

    return status;
}

HRESULT WINAPI storage_folder_CreateFile( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING Path;
    DWORD attrib;
    BOOL Exists = FALSE;
    BOOL Replace = FALSE;
    WCHAR fullPath[MAX_PATH];
    WCHAR uuidName[MAX_PATH];

    struct storage_folder_creation_options *creation_options = (struct storage_folder_creation_options *)param;
    struct storage_folder *invokerFolder;
    struct storage_file *resultFile;

    //Parameters
    CreationCollisionOption collisionOption = creation_options->option;
    HSTRING Name = creation_options->name;

    if (!(resultFile = calloc( 1, sizeof(*resultFile) ))) return E_OUTOFMEMORY;

    resultFile->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    resultFile->IStorageFilePropertiesWithAvailability_iface.lpVtbl = &storage_file_properties_with_availability_vtbl;
    resultFile->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    resultFile->ref = 1;

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

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
                return E_ABORT;
            }
        }
        if ( !Exists )
        {
            CloseHandle( CreateFileW( fullPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) );
            status = S_OK;
        }
    }

    WindowsCreateString( fullPath, wcslen( fullPath ), &Path );
    status = storage_file_AssignFile( Path, &resultFile->IStorageFile_iface );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&resultFile->IStorageFile_iface;
    }

    return status;
}

HRESULT WINAPI storage_folder_FetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    WCHAR fullPath[MAX_PATH];
    HRESULT status;
    HSTRING Path;
    HSTRING itemPath;
    boolean isFolder;

    struct storage_folder * folder;
    struct storage_item * item;

    struct storage_folder *newFolder;
    struct storage_file *newFile;

    TRACE( "iface %p, value %p\n", invoker, result );

    folder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &folder->IStorageItem_iface )->Path;

    if (!result) return E_INVALIDARG;
    if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;
    item->IStorageItem_iface.lpVtbl = &storage_item_vtbl;

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &itemPath );

    status = storage_item_Internal_CreateNew( itemPath, &item->IStorageItem_iface );
    
    if ( SUCCEEDED( status ) )
    {
        IStorageItem_IsOfType( &item->IStorageItem_iface, StorageItemTypes_Folder, &isFolder );
        if ( isFolder )
        {
            if (!(newFolder = calloc( 1, sizeof(*newFolder) ))) return E_OUTOFMEMORY;
            storage_folder_AssignFolder( itemPath, &newFolder->IStorageFolder_iface );
            result->vt = VT_UNKNOWN;
            result->ppunkVal = (IUnknown **)&newFolder->IStorageItem_iface;
        } else
        {
            if (!(newFile = calloc( 1, sizeof(*newFile) ))) return E_OUTOFMEMORY;
            storage_file_AssignFile( itemPath, &newFile->IStorageFile_iface );
            result->vt = VT_UNKNOWN;
            result->ppunkVal = (IUnknown **)&newFile->IStorageItem_iface;
        }
    }

    return status;
}


HRESULT WINAPI storage_folder_FetchItemsAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING itemPath;
    WCHAR searchPath[MAX_PATH]; 
    WCHAR fullItemPath[MAX_PATH];
    boolean isFolder;

    struct storage_folder *invokerFolder;
    struct storage_item_vector_view *itemVector;

    struct storage_folder *newFolder;
    struct storage_file *newFile;

    TRACE( "iface %p, value %p\n", invoker, result );

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

    if (!(itemVector = calloc( 1, sizeof(*itemVector) ))) return E_OUTOFMEMORY;
    
    itemVector->IVectorView_IStorageItem_iface.lpVtbl = &storage_item_vector_view_vtbl;
    itemVector->ref = 1;
    itemVector->size = 0;

    swprintf( searchPath, MAX_PATH, L"%s\\*.*", WindowsGetStringRawBuffer( Path, NULL ) );

    if ( param == NULL )
    {
        hFind = FindFirstFileW( searchPath, &findFileData );

        if ( hFind == INVALID_HANDLE_VALUE ) 
        {
            free(itemVector);
            return E_ABORT;
        } 

        while ( FindNextFileW( hFind, &findFileData ) != 0 ) 
        {
            if ( wcscmp( findFileData.cFileName, L"." ) != 0 
              && wcscmp( findFileData.cFileName, L".." ) != 0 ) 
            {
                if (!(itemVector->elements[itemVector->size] = calloc( 1, sizeof(*itemVector->elements[itemVector->size]) ))) 
                    return E_OUTOFMEMORY;
                itemVector->elements[itemVector->size]->lpVtbl = &storage_item_vtbl;
                
                PathAppendW( fullItemPath, WindowsGetStringRawBuffer( Path, NULL ) );
                PathAppendW( fullItemPath, findFileData.cFileName );
                WindowsCreateString( fullItemPath, wcslen( fullItemPath ), &itemPath);

                status = storage_item_Internal_CreateNew( itemPath, itemVector->elements[itemVector->size] );

                if ( SUCCEEDED( status ) )
                {
                    IStorageItem_IsOfType( itemVector->elements[itemVector->size], StorageItemTypes_Folder, &isFolder );
                    if ( isFolder )
                    {
                        if (!(newFolder = calloc( 1, sizeof(*newFolder) ))) return E_OUTOFMEMORY;
                        storage_folder_AssignFolder( itemPath, &newFolder->IStorageFolder_iface );
                        itemVector->elements[itemVector->size] = &newFolder->IStorageItem_iface;
                    } else
                    {
                        if (!(newFile = calloc( 1, sizeof(*newFile) ))) return E_OUTOFMEMORY;
                        storage_file_AssignFile( itemPath, &newFile->IStorageFile_iface );
                        itemVector->elements[itemVector->size] = &newFile->IStorageItem_iface;
                    }
                }

                itemVector->size++;

                SecureZeroMemory( fullItemPath, sizeof( fullItemPath ) );
            }
        }
    }


    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&itemVector->IVectorView_IStorageItem_iface;
    }

    return status;
}

HRESULT WINAPI storage_folder_FetchFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WCHAR fullPath[MAX_PATH];
    HRESULT status;
    HSTRING Path;
    HSTRING folderPath;
    boolean isFolder;
    
    struct storage_folder * folder;
    struct storage_folder * folderToFetch;

    TRACE( "iface %p, value %p\n", invoker, result );

    folder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &folder->IStorageItem_iface )->Path;

    if (!result) return E_INVALIDARG;
    if (!(folderToFetch = calloc( 1, sizeof(*folderToFetch) ))) return E_OUTOFMEMORY;

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &folderPath );

    status = storage_folder_AssignFolder( folderPath, &folderToFetch->IStorageFolder_iface );
    IStorageItem_IsOfType( &folderToFetch->IStorageItem_iface, StorageItemTypes_Folder, &isFolder );

    if ( !isFolder )
        status = E_INVALIDARG;
    
    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&folderToFetch->IStorageFolder_iface;
    }


    return status;
}

HRESULT WINAPI storage_folder_FetchFoldersAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAW findFolderData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING folderPath;
    WCHAR searchPath[MAX_PATH];
    WCHAR fullFolderPath[MAX_PATH];

    struct storage_folder_vector_view *folderVector;
    struct storage_folder *invokerFolder;

    TRACE( "iface %p, value %p\n", invoker, result );

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

    if (!(folderVector = calloc( 1, sizeof(*folderVector) ))) return E_OUTOFMEMORY;
    
    folderVector->IVectorView_StorageFolder_iface.lpVtbl = &storage_folder_vector_view_vtbl;
    folderVector->ref = 1;
    folderVector->size = 0;

    swprintf( searchPath, MAX_PATH, L"%s\\*.*", WindowsGetStringRawBuffer( Path, NULL ) );

    if ( param == NULL )
    {
        hFind = FindFirstFileW( searchPath, &findFolderData );

        if ( hFind == INVALID_HANDLE_VALUE ) 
        {
            return E_ABORT;
        } 

        while ( FindNextFileW( hFind, &findFolderData ) != 0 ) 
        {
            if ( wcscmp( findFolderData.cFileName, L"." ) != 0 
              && wcscmp( findFolderData.cFileName, L".." ) != 0 
              && findFolderData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
            {
                if (!(folderVector->elements[folderVector->size] = calloc( 1, sizeof(*folderVector->elements[folderVector->size]) ))) 
                    return E_OUTOFMEMORY;
                folderVector->elements[folderVector->size]->lpVtbl = &storage_folder_vtbl;
                impl_from_IStorageFolder( folderVector->elements[folderVector->size] )->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
                impl_from_IStorageFolder( folderVector->elements[folderVector->size] )->IStorageFolder2_iface.lpVtbl = &storage_folder2_vtbl;

                PathAppendW( fullFolderPath, WindowsGetStringRawBuffer( Path, NULL ) );
                PathAppendW( fullFolderPath, findFolderData.cFileName );
                WindowsCreateString( fullFolderPath, wcslen( fullFolderPath ), &folderPath);

                status = storage_folder_AssignFolder( folderPath, &impl_from_IStorageFolder( folderVector->elements[folderVector->size] )->IStorageFolder_iface );

                if ( FAILED( status ) )
                    break;

                SecureZeroMemory( fullFolderPath, sizeof( fullFolderPath ) );

                folderVector->size++;
            }
        }
    }


    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&folderVector->IVectorView_StorageFolder_iface;
    }

    return status;
}

HRESULT WINAPI storage_folder_FetchFile( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WCHAR fullPath[MAX_PATH];
    HRESULT status;
    HSTRING Path;
    HSTRING folderPath;
    boolean isFile;
    
    struct storage_folder * folder;
    struct storage_file * fileToFetch;

    TRACE( "iface %p, value %p\n", invoker, result );

    folder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &folder->IStorageItem_iface )->Path;

    if (!result) return E_INVALIDARG;
    if (!(fileToFetch = calloc( 1, sizeof(*fileToFetch) ))) return E_OUTOFMEMORY;

    fileToFetch->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    fileToFetch->IStorageFilePropertiesWithAvailability_iface.lpVtbl = &storage_file_properties_with_availability_vtbl;
    fileToFetch->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    fileToFetch->ref = 1;

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &folderPath );

    status = storage_file_AssignFile( folderPath, &fileToFetch->IStorageFile_iface );
        IStorageItem_IsOfType( &fileToFetch->IStorageItem_iface, StorageItemTypes_File, &isFile );

    if ( !isFile )
        status = E_INVALIDARG;
    
    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&fileToFetch->IStorageFile_iface;
    }


    return status;
}

HRESULT WINAPI storage_folder_FetchFilesAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING filePath;
    WCHAR searchPath[MAX_PATH]; 
    WCHAR fullFilePath[MAX_PATH];

    struct storage_folder *invokerFolder;
    struct storage_file_vector_view *fileVector;

    TRACE( "iface %p, value %p\n", invoker, result );

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

    if (!(fileVector = calloc( 1, sizeof(*fileVector) ))) return E_OUTOFMEMORY;
    
    fileVector->IVectorView_StorageFile_iface.lpVtbl = &storage_file_vector_view_vtbl;
    fileVector->ref = 1;
    fileVector->size = 0;

    swprintf( searchPath, MAX_PATH, L"%s\\*.*", WindowsGetStringRawBuffer( Path, NULL ) );

    if ( param == NULL )
    {
        hFind = FindFirstFileW( searchPath, &findFileData );

        if ( hFind == INVALID_HANDLE_VALUE ) 
        {
            return E_ABORT;
        } 

        while ( FindNextFileW( hFind, &findFileData ) != 0 ) 
        {
            if ( wcscmp( findFileData.cFileName, L"." ) != 0 
              && wcscmp( findFileData.cFileName, L".." ) != 0
              && !(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
            {
                if (!(fileVector->elements[fileVector->size] = calloc( 1, sizeof(*fileVector->elements[fileVector->size]) ))) 
                    return E_OUTOFMEMORY;
                fileVector->elements[fileVector->size]->lpVtbl = &storage_file_vtbl;
                
                PathAppendW( fullFilePath, WindowsGetStringRawBuffer( Path, NULL ) );
                PathAppendW( fullFilePath, findFileData.cFileName );
                WindowsCreateString( fullFilePath, wcslen( fullFilePath ), &filePath);

                status = storage_file_AssignFile( filePath, fileVector->elements[fileVector->size] );

                if ( FAILED( status ) )
                    break;

                fileVector->size++;

                SecureZeroMemory( fullFilePath, sizeof( fullFilePath ) );
            }
        }
    }


    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&fileVector->IVectorView_StorageFile_iface;
    }

    return status;
}

HRESULT WINAPI storage_folder2_TryFetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    WCHAR fullPath[MAX_PATH];
    HRESULT status;
    HSTRING Path;
    HSTRING itemPath;
    boolean isFolder;

    struct storage_folder * folder;
    struct storage_item * item;

    struct storage_folder *newFolder;
    struct storage_file *newFile;

    TRACE( "iface %p, value %p\n", invoker, result );

    folder = impl_from_IStorageFolder2( (IStorageFolder2 *)invoker );
    Path = impl_from_IStorageItem( &folder->IStorageItem_iface )->Path;

    if (!result) return E_INVALIDARG;
    if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;
    item->IStorageItem_iface.lpVtbl = &storage_item_vtbl;

    PathAppendW( fullPath, WindowsGetStringRawBuffer( Path, NULL ) );
    PathAppendW( fullPath, WindowsGetStringRawBuffer( (HSTRING)param, NULL ) );
    WindowsCreateString( fullPath, wcslen( fullPath ), &itemPath );

    status = storage_item_Internal_CreateNew( itemPath, &item->IStorageItem_iface );
    
    if ( SUCCEEDED( status ) )
    {
        IStorageItem_IsOfType( &item->IStorageItem_iface, StorageItemTypes_Folder, &isFolder );
        if ( isFolder )
        {
            if (!(newFolder = calloc( 1, sizeof(*newFolder) ))) return E_OUTOFMEMORY;
            storage_folder_AssignFolder( itemPath, &newFolder->IStorageFolder_iface );
            result->vt = VT_UNKNOWN;
            result->ppunkVal = (IUnknown **)&newFolder->IStorageItem_iface;
        } else
        {
            if (!(newFile = calloc( 1, sizeof(*newFile) ))) return E_OUTOFMEMORY;
            storage_file_AssignFile( itemPath, &newFile->IStorageFile_iface );
            result->vt = VT_UNKNOWN;
            result->ppunkVal = (IUnknown **)&newFile->IStorageItem_iface;
        }
    } else 
    {
        result->vt = VT_NULL;
        result->ppunkVal = (IUnknown **)NULL;
    }

    return S_OK;
}
