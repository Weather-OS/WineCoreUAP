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

#ifndef STORAGE_ITEM_CONTENT_PROPERTIES_INTERNAL_H
#define STORAGE_ITEM_CONTENT_PROPERTIES_INTERNAL_H

#include "../../private.h"
#include "wine/debug.h"

#include "QueryOptionsInternal.h"
#include "StorageQueryResultsInternal.h"
#include "../StorageFolderInternal.h"

struct storage_folder_query_operations
{
    //Derivatives
    IStorageFolderQueryOperations IStorageFolderQueryOperations_iface;

    LONG ref;
};

HRESULT WINAPI storage_folder_query_operations_GetIndexedState( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_query_operations_FetchFileQuery( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageFileQueryResult **value );
HRESULT WINAPI storage_folder_query_operations_FetchFolderQuery( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageFolderQueryResult **value );
HRESULT WINAPI storage_folder_query_operations_FetchItemQuery( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageItemQueryResult **value );

#endif