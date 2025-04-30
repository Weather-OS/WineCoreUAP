/* WinRT Windows.Storage.Search.IStorageFolderQueryOperations Implementation
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

#include "StorageFolderQueryOperationsInternal.h"

_ENABLE_DEBUGGING_

HRESULT WINAPI storage_folder_query_operations_GetIndexedState( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    //Unix has no concept of "Indexed States"
    FIXME( "iface %p, param %p stub!\n", invoker, param );
    result->vt = VT_UI4;
    result->ulVal = (ULONG)IndexedState_Unknown;
    return S_OK;
}

HRESULT WINAPI storage_folder_query_operations_FetchFileQuery( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageFileQueryResult **value )
{
    struct storage_folder *inheritedFolder = CONTAINING_RECORD( iface, struct storage_folder, IStorageFolderQueryOperations_iface );
    struct file_query_result *fileQueryResult;

    TRACE( "iface %p, queryOptions %p\n", iface, queryOptions );

    if (!(fileQueryResult = calloc( 1, sizeof(*fileQueryResult) ))) return E_OUTOFMEMORY;

    fileQueryResult->IStorageFileQueryResult_iface.lpVtbl = &file_query_result_vtbl;
    fileQueryResult->IStorageQueryResultBase_iface.lpVtbl = &query_result_base_vtbl;
    fileQueryResult->searchType = StorageItemTypes_File;
    if ( inheritedFolder->IStorageFolder_iface.lpVtbl != &storage_folder_vtbl )
    {
        free( fileQueryResult );
        return E_ABORT;
    } 
    fileQueryResult->Folder = &inheritedFolder->IStorageFolder_iface;
    fileQueryResult->storageQueryResultBaseRef = 1;
    fileQueryResult->Options = queryOptions;
    fileQueryResult->ref = 1;

    *value = &fileQueryResult->IStorageFileQueryResult_iface;

    return S_OK;
}

HRESULT WINAPI storage_folder_query_operations_FetchFolderQuery( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageFolderQueryResult **value )
{
    struct storage_folder *inheritedFolder = CONTAINING_RECORD( iface, struct storage_folder, IStorageFolderQueryOperations_iface );
    struct folder_query_result *folderQueryResult;

    TRACE( "iface %p, queryOptions %p\n", iface, queryOptions );

    if (!(folderQueryResult = calloc( 1, sizeof(*folderQueryResult) ))) return E_OUTOFMEMORY;

    folderQueryResult->IStorageFolderQueryResult_iface.lpVtbl = &folder_query_result_vtbl;
    folderQueryResult->IStorageQueryResultBase_iface.lpVtbl = &query_result_base_vtbl;
    folderQueryResult->searchType = StorageItemTypes_Folder;
    if ( inheritedFolder->IStorageFolder_iface.lpVtbl != &storage_folder_vtbl )
    {
        free( folderQueryResult );
        return E_ABORT;
    } 
    folderQueryResult->Folder = &inheritedFolder->IStorageFolder_iface;
    folderQueryResult->storageQueryResultBaseRef = 1;
    folderQueryResult->Options = queryOptions;
    folderQueryResult->ref = 1;

    *value = &folderQueryResult->IStorageFolderQueryResult_iface;

    return S_OK;
}

HRESULT WINAPI storage_folder_query_operations_FetchItemQuery( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageItemQueryResult **value )
{
    struct storage_folder *inheritedFolder = CONTAINING_RECORD( iface, struct storage_folder, IStorageFolderQueryOperations_iface );
    struct item_query_result *itemQueryResult;

    TRACE( "iface %p, queryOptions %p\n", iface, queryOptions );

    if (!(itemQueryResult = calloc( 1, sizeof(*itemQueryResult) ))) return E_OUTOFMEMORY;

    itemQueryResult->IStorageItemQueryResult_iface.lpVtbl = &item_query_result_vtbl;
    itemQueryResult->IStorageQueryResultBase_iface.lpVtbl = &query_result_base_vtbl;
    itemQueryResult->searchType = StorageItemTypes_None;
    if ( inheritedFolder->IStorageFolder_iface.lpVtbl != &storage_folder_vtbl )
    {
        free( itemQueryResult );
        return E_ABORT;
    } 
    itemQueryResult->Folder = &inheritedFolder->IStorageFolder_iface;
    itemQueryResult->storageQueryResultBaseRef = 1;
    itemQueryResult->Options = queryOptions;
    itemQueryResult->ref = 1;

    *value = &itemQueryResult->IStorageItemQueryResult_iface;

    return S_OK;
}