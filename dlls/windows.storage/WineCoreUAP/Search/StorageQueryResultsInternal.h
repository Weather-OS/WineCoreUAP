/* WinRT Windows.Storage.Search.IStorageQueryResults Implementation
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

#ifndef STORAGE_QUERY_RESULTS_INTERNAL_H
#define STORAGE_QUERY_RESULTS_INTERNAL_H

#include "../../private.h"
#include "wine/debug.h"

#include "../StorageFolderInternal.h"
#include "../StorageFileInternal.h"

extern const struct IStorageQueryResultBaseVtbl query_result_base_vtbl;
extern const struct IStorageFileQueryResultVtbl file_query_result_vtbl;
extern const struct IStorageFolderQueryResultVtbl folder_query_result_vtbl;
extern const struct IStorageItemQueryResultVtbl item_query_result_vtbl;

struct options_changed_event_handler
{
    ITypedEventHandler_IStorageQueryResultBase_IInspectable *contentEventHandler;
    EventRegistrationToken token;
};

struct query_result_base
{
    //Derivatives
    IStorageQueryResultBase IStorageQueryResultBase_iface;    
    StorageItemTypes searchType;
    IStorageFolder *Folder;
    IQueryOptions *Options;

    struct options_changed_event_handler **optionsChangedEventHandlers;
    UINT32 handlerSize;
    UINT32 handlerCapacity;

    LONG ref;
};

struct folder_query_result
{
    //Derivatives
    IStorageFolderQueryResult IStorageFolderQueryResult_iface;
    
    //IStorageQueryResultBase Derivatives
    IStorageQueryResultBase IStorageQueryResultBase_iface;    
        StorageItemTypes searchType;
        IStorageFolder *Folder;
        IQueryOptions *Options;
        struct options_changed_event_handler **optionsChangedEventHandlers;
        UINT32 handlerSize;
        UINT32 handlerCapacity;
        LONG storageQueryResultBaseRef;

    LONG ref;
};

struct file_query_result
{
    //Derivatives
    IStorageFileQueryResult IStorageFileQueryResult_iface;
    
    //IStorageQueryResultBase Derivatives
    IStorageQueryResultBase IStorageQueryResultBase_iface;    
        StorageItemTypes searchType;
        IStorageFolder *Folder;
        IQueryOptions *Options;
        struct options_changed_event_handler **optionsChangedEventHandlers;
        UINT32 handlerSize;
        UINT32 handlerCapacity;
        LONG storageQueryResultBaseRef;

    LONG ref;
};

struct item_query_result
{
    //Derivatives
    IStorageItemQueryResult IStorageItemQueryResult_iface;
    
    //IStorageQueryResultBase Derivatives
    IStorageQueryResultBase IStorageQueryResultBase_iface;    
        StorageItemTypes searchType;
        IStorageFolder *Folder;
        IQueryOptions *Options;
        struct options_changed_event_handler **optionsChangedEventHandlers;
        UINT32 handlerSize;
        UINT32 handlerCapacity;
        LONG storageQueryResultBaseRef;

    LONG ref;
};

/**
 * Parametizred structs
 */
struct storage_query_options
{
    UINT32 startIndex;
    UINT32 maxNumberOfItems;
};

struct query_result_base *impl_from_IStorageQueryResultBase( IStorageQueryResultBase *iface );
struct folder_query_result *impl_from_IStorageFolderQueryResult( IStorageFolderQueryResult *iface );
struct file_query_result *impl_from_IStorageFileQueryResult( IStorageFileQueryResult *iface );
struct item_query_result *impl_from_IStorageItemQueryResult( IStorageItemQueryResult *iface );

HRESULT WINAPI query_result_base_SearchCountAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI query_result_base_FindFirstAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI query_result_base_FetchFoldersAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI query_result_base_FetchFilesAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI query_result_base_FetchItemsAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif