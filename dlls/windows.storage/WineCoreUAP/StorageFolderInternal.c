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

#include "util.h"
#include "StorageFolderInternal.h"
#include "StorageItemInternal.h"
#include "VectorView/StorageItemVectorView.h"
#include "VectorView/StorageFolderVectorView.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

HRESULT WINAPI storage_folder_AssignFolder ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT status;
    BOOLEAN isFolder;

    struct storage_folder *folder;

    TRACE( "iface %p, value %p\n", invoker, result );
    
    if (!result) return E_INVALIDARG;
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    status = storage_item_Internal_CreateNew( (HSTRING)param, &folder->IStorageItem_iface );

    IStorageItem_IsOfType( &folder->IStorageItem_iface, StorageItemTypes_Folder, &isFolder );
    if ( !isFolder )
        status = E_INVALIDARG;

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&folder->IStorageFolder_iface;
    }

    return status;
}


HRESULT WINAPI storage_folder_CreateFolder( IStorageFolder* folder, CreationCollisionOption collisionOption, HSTRING Name, HSTRING *OutPath )
{
    HRESULT status = S_OK;
    HSTRING Path;
    DWORD attrib;
    BOOL Exists = FALSE;
    BOOL Replace = FALSE;
    CHAR fullPath[MAX_PATH];
    CHAR uuidName[MAX_PATH];

    struct storage_folder *invokerFolder;

    TRACE( "iface %p, value %p\n", folder, OutPath );

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)folder );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

    strcpy( fullPath, HStringToLPCSTR( Path ) );

    switch ( collisionOption )
    {
        case CreationCollisionOption_FailIfExists:
            PathAppendA( fullPath, HStringToLPCSTR( Name ));
            attrib = GetFileAttributesA(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                status = E_INVALIDARG;
            else 
                status = S_OK;
            break;

        case CreationCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof(uuidName) );
            PathAppendA( fullPath, uuidName );

            status = S_OK;
            break;
        
        case CreationCollisionOption_OpenIfExists:
            PathAppendA( fullPath, HStringToLPCSTR( Name ));
            attrib = GetFileAttributesA(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Exists = TRUE;
            
            status = S_OK;
            break;
        
        case CreationCollisionOption_ReplaceExisting:
            PathAppendA( fullPath, HStringToLPCSTR( Name ));
            attrib = GetFileAttributesA(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Replace = TRUE;

            status = S_OK;
            break;
    }
    if ( SUCCEEDED( status ) )
    {
        if ( Replace )
        {
            if ( !RemoveDirectoryA( fullPath ) )
            {
                return E_ABORT;
            }
        }
        if ( !Exists )
        {
            if ( !CreateDirectoryA( fullPath, NULL ) )
            {
                return E_ABORT;
            }
            status = S_OK;
        }
        status = WindowsCreateString( CharToLPCWSTR( fullPath ), wcslen( CharToLPCWSTR( fullPath ) ), OutPath );
    }
    return status;
}

HRESULT WINAPI storage_folder_FetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    CHAR fullPath[MAX_PATH];
    HRESULT status;
    HSTRING Path;
    HSTRING itemPath;

    struct storage_folder * folder;
    struct storage_item * item;

    TRACE( "iface %p, value %p\n", invoker, result );

    folder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &folder->IStorageItem_iface )->Path;

    if (!result) return E_INVALIDARG;
    if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;

    item->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    item->ref = 1;

    PathAppendA( fullPath, HStringToLPCSTR( Path ) );
    PathAppendA( fullPath, HStringToLPCSTR( (HSTRING)param ) );
    WindowsCreateString( CharToLPCWSTR( fullPath ), wcslen( CharToLPCWSTR( fullPath ) ), &itemPath );

    status = storage_item_Internal_CreateNew( itemPath, &item->IStorageItem_iface );
    
    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&item->IStorageItem_iface;
    }


    return status;
}


HRESULT WINAPI storage_folder_FetchItemsAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING itemPath;
    CHAR searchPath[MAX_PATH]; 
    CHAR fullItemPath[MAX_PATH];

    struct storage_folder *invokerFolder;
    struct storage_item_vector_view *itemVector;

    TRACE( "iface %p, value %p\n", invoker, result );

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

    if (!(itemVector = calloc( 1, sizeof(*itemVector) ))) return E_OUTOFMEMORY;
    
    itemVector->IVectorView_IStorageItem_iface.lpVtbl = &storage_item_vector_view_vtbl;
    itemVector->ref = 1;
    itemVector->size = 0;

    snprintf( searchPath, MAX_PATH, "%s\\*.*", HStringToLPCSTR( Path ) );

    if ( param == NULL )
    {
        hFind = FindFirstFileA( searchPath, &findFileData );

        if ( hFind == INVALID_HANDLE_VALUE ) 
        {
            return E_ABORT;
        } 

        while ( FindNextFileA( hFind, &findFileData ) != 0 ) 
        {
            if ( strcmp( findFileData.cFileName, "." ) != 0 
              && strcmp( findFileData.cFileName, ".." ) != 0 ) 
            {
                if (!(itemVector->elements[itemVector->size] = calloc( 1, sizeof(*itemVector->elements[itemVector->size]) ))) 
                    return E_OUTOFMEMORY;
                itemVector->elements[itemVector->size]->lpVtbl = &storage_item_vtbl;
                
                PathAppendA( fullItemPath, HStringToLPCSTR( Path ) );
                PathAppendA( fullItemPath, findFileData.cFileName );
                WindowsCreateString( CharToLPCWSTR( fullItemPath ), wcslen( CharToLPCWSTR( fullItemPath ) ), &itemPath);

                status = storage_item_Internal_CreateNew( itemPath, itemVector->elements[itemVector->size] );

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
    CHAR fullPath[MAX_PATH];
    HRESULT status;
    HSTRING Path;
    HSTRING folderPath;
    
    struct storage_folder * folder;
    struct storage_folder * folderToFetch;

    TRACE( "iface %p, value %p\n", invoker, result );

    folder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &folder->IStorageItem_iface )->Path;

    if (!result) return E_INVALIDARG;
    if (!(folderToFetch = calloc( 1, sizeof(*folderToFetch) ))) return E_OUTOFMEMORY;

    folderToFetch->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folderToFetch->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folderToFetch->ref = 1;

    PathAppendA( fullPath, HStringToLPCSTR( Path ) );
    PathAppendA( fullPath, HStringToLPCSTR( (HSTRING)param ) );
    WindowsCreateString( CharToLPCWSTR( fullPath ), wcslen( CharToLPCWSTR( fullPath ) ), &folderPath );

    status = storage_item_Internal_CreateNew( folderPath, &folderToFetch->IStorageItem_iface );
    
    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&folderToFetch->IStorageFolder_iface;
    }


    return status;
}

HRESULT WINAPI storage_folder_FetchFoldersAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAA findFolderData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    HRESULT status = S_OK;
    HSTRING Path;
    HSTRING folderPath;
    CHAR searchPath[MAX_PATH];
    CHAR fullFolderPath[MAX_PATH];

    struct storage_folder_vector_view *folderVector;
    struct storage_folder *invokerFolder;

    TRACE( "iface %p, value %p\n", invoker, result );

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem( &invokerFolder->IStorageItem_iface )->Path;

    if (!(folderVector = calloc( 1, sizeof(*folderVector) ))) return E_OUTOFMEMORY;
    
    folderVector->IVectorView_StorageFolder_iface.lpVtbl = &storage_folder_vector_view_vtbl;
    folderVector->ref = 1;
    folderVector->size = 0;

    snprintf( searchPath, MAX_PATH, "%s\\*.*", HStringToLPCSTR( Path ) );

    if ( param == NULL )
    {
        hFind = FindFirstFileA( searchPath, &findFolderData );

        if ( hFind == INVALID_HANDLE_VALUE ) 
        {
            return E_ABORT;
        } 

        while ( FindNextFileA( hFind, &findFolderData ) != 0 ) 
        {
            if ( strcmp( findFolderData.cFileName, "." ) != 0 
              && strcmp( findFolderData.cFileName, ".." ) != 0 
              && findFolderData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
            {
                if (!(folderVector->elements[folderVector->size] = calloc( 1, sizeof(*folderVector->elements[folderVector->size]) ))) 
                    return E_OUTOFMEMORY;
                folderVector->elements[folderVector->size]->lpVtbl = &storage_folder_vtbl;
                impl_from_IStorageFolder( folderVector->elements[folderVector->size] )->IStorageItem_iface.lpVtbl = &storage_item_vtbl;

                PathAppendA( fullFolderPath, HStringToLPCSTR( Path ) );
                PathAppendA( fullFolderPath, findFolderData.cFileName );
                WindowsCreateString( CharToLPCWSTR( fullFolderPath ), wcslen( CharToLPCWSTR( fullFolderPath ) ), &folderPath);

                status = storage_item_Internal_CreateNew( folderPath, &impl_from_IStorageFolder( folderVector->elements[folderVector->size] )->IStorageItem_iface );

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