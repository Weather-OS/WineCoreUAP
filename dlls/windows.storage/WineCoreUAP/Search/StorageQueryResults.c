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

#include "StorageQueryResultsInternal.h"

_ENABLE_DEBUGGING_

// Storage Query Results

struct query_result_base *impl_from_IStorageQueryResultBase( IStorageQueryResultBase *iface )
{
    return CONTAINING_RECORD( iface, struct query_result_base, IStorageQueryResultBase_iface );
}

static HRESULT WINAPI query_result_base_QueryInterface( IStorageQueryResultBase *iface, REFIID iid, void **out )
{
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageQueryResultBase ))
    {
        *out = &impl->IStorageQueryResultBase_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI query_result_base_AddRef( IStorageQueryResultBase *iface )
{
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI query_result_base_Release( IStorageQueryResultBase *iface )
{
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI query_result_base_GetIids( IStorageQueryResultBase *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_result_base_GetRuntimeClassName( IStorageQueryResultBase *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_result_base_GetTrustLevel( IStorageQueryResultBase *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_result_base_GetItemCountAsync( IStorageQueryResultBase *iface, IAsyncOperation_UINT32 **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_UINT32 };

    TRACE( "iface %p, operation %p\n", iface, operation );

    // Arguments 
    if ( !operation ) return E_POINTER;

    hr = async_operation_uint32_create( (IUnknown *)iface, NULL, query_result_base_SearchCountAsync, iids, operation );
    TRACE( "created IAsyncOperation_UINT32 %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI query_result_base_get_Folder( IStorageQueryResultBase *iface, IStorageFolder **container )
{
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );
    TRACE( "iface %p, container %p\n", iface, container );
    if ( !container ) return E_POINTER;
    *container = impl->Folder;
    return S_OK;
}

static HRESULT WINAPI query_result_base_add_ContentsChanged( IStorageQueryResultBase *iface, ITypedEventHandler_IStorageQueryResultBase_IInspectable *handler, EventRegistrationToken *eventCookie )
{
    FIXME( "iface %p, handler %p stub!\n", iface, handler );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_result_base_remove_ContentsChanged( IStorageQueryResultBase *iface, EventRegistrationToken eventCookie )
{
    FIXME( "iface %p, eventCookie %p stub!\n", iface, &eventCookie );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_result_base_add_OptionsChanged( IStorageQueryResultBase *iface, ITypedEventHandler_IStorageQueryResultBase_IInspectable *handler, EventRegistrationToken *eventCookie )
{
    EventRegistrationToken registeredToken;

    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );
    struct options_changed_event_handler **tmp = impl->optionsChangedEventHandlers;

    TRACE( "iface %p, handler %p, eventCookie %p\n", iface, handler, eventCookie );

    if (impl->handlerSize == impl->handlerCapacity)
    {
        impl->handlerCapacity = max( 32, impl->handlerCapacity * 3 / 2 );
        if (!(impl->optionsChangedEventHandlers = realloc( impl->optionsChangedEventHandlers, impl->handlerCapacity * sizeof(*impl->optionsChangedEventHandlers) )))
        {
            impl->optionsChangedEventHandlers = tmp;
            return E_OUTOFMEMORY;
        }
    }

    impl->optionsChangedEventHandlers[impl->handlerSize] = malloc( sizeof(*impl->optionsChangedEventHandlers[impl->handlerSize]) );
    if (!impl->optionsChangedEventHandlers[impl->handlerSize])
    {
        impl->optionsChangedEventHandlers = tmp;
        return E_OUTOFMEMORY;
    }

    ITypedEventHandler_IStorageQueryResultBase_IInspectable_AddRef( handler );

    registeredToken.value = impl->handlerSize;

    impl->optionsChangedEventHandlers[impl->handlerSize]->token = registeredToken;
    impl->optionsChangedEventHandlers[impl->handlerSize]->contentEventHandler = handler;

    *eventCookie = registeredToken;

    impl->handlerSize++;

    return S_OK;
}

static HRESULT WINAPI query_result_base_remove_OptionsChanged( IStorageQueryResultBase *iface, EventRegistrationToken eventCookie )
{
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );

    TRACE( "iface %p, eventCookie %p\n", iface, &eventCookie );

    //Tokens remain catenated 
    ITypedEventHandler_IStorageQueryResultBase_IInspectable_Release( impl->optionsChangedEventHandlers[eventCookie.value]->contentEventHandler );
    free(impl->optionsChangedEventHandlers[eventCookie.value]);
    
    return S_OK;
}

static HRESULT WINAPI query_result_base_FindStartIndexAsync( IStorageQueryResultBase *iface, IInspectable *value, IAsyncOperation_UINT32 **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_UINT32 };
    hr = async_operation_uint32_create( (IUnknown *)iface, (IUnknown *)value, query_result_base_FindFirstAsync, iids, operation );
    TRACE( "created IAsyncOperation_UINT32 %p.\n", *operation );
    return hr;
}

static HRESULT WINAPI query_result_base_GetCurrentQueryOptions( IStorageQueryResultBase *iface, IQueryOptions **value )
{
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Options;
    return S_OK;
}

static HRESULT WINAPI query_result_base_ApplyNewQueryOptions( IStorageQueryResultBase *iface, IQueryOptions *newQueryOptions )
{
    UINT32 eventIterator;
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );
    TRACE( "iface %p, newQueryOptions %p\n", iface, newQueryOptions );
    impl->Options = newQueryOptions;

    for ( eventIterator = 0; eventIterator < impl->handlerSize; eventIterator++ )
    {
        if ( impl->optionsChangedEventHandlers[eventIterator] )
            ITypedEventHandler_IStorageQueryResultBase_IInspectable_Invoke( impl->optionsChangedEventHandlers[eventIterator]->contentEventHandler, iface, (IInspectable *)newQueryOptions );
    }

    return S_OK;
}

const struct IStorageQueryResultBaseVtbl query_result_base_vtbl =
{
    query_result_base_QueryInterface,
    query_result_base_AddRef,
    query_result_base_Release,
    /* IInspectable methods */
    query_result_base_GetIids,
    query_result_base_GetRuntimeClassName,
    query_result_base_GetTrustLevel,
    /* IStorageQueryResultBase methods */
    query_result_base_GetItemCountAsync,
    query_result_base_get_Folder,
    query_result_base_add_ContentsChanged,
    query_result_base_remove_ContentsChanged,
    query_result_base_add_OptionsChanged,
    query_result_base_remove_OptionsChanged,
    query_result_base_FindStartIndexAsync,
    query_result_base_GetCurrentQueryOptions,
    query_result_base_ApplyNewQueryOptions
};

struct folder_query_result *impl_from_IStorageFolderQueryResult( IStorageFolderQueryResult *iface )
{
    return CONTAINING_RECORD( iface, struct folder_query_result, IStorageFolderQueryResult_iface );
}

static HRESULT WINAPI folder_query_result_QueryInterface( IStorageFolderQueryResult *iface, REFIID iid, void **out )
{
    struct folder_query_result *impl = impl_from_IStorageFolderQueryResult( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFolderQueryResult ))
    {
        *out = &impl->IStorageFolderQueryResult_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageQueryResultBase ))
    {
        *out = &impl->IStorageQueryResultBase_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI folder_query_result_AddRef( IStorageFolderQueryResult *iface )
{
    struct folder_query_result *impl = impl_from_IStorageFolderQueryResult( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI folder_query_result_Release( IStorageFolderQueryResult *iface )
{
    struct folder_query_result *impl = impl_from_IStorageFolderQueryResult( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI folder_query_result_GetIids( IStorageFolderQueryResult *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI folder_query_result_GetRuntimeClassName( IStorageFolderQueryResult *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI folder_query_result_GetTrustLevel( IStorageFolderQueryResult *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI folder_query_result_GetFoldersAsync( IStorageFolderQueryResult *iface, UINT32 startIndex, UINT32 maxNumberOfItems, IAsyncOperation_IVectorView_StorageFolder **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_StorageFolder };
    struct storage_query_options *options;
    if (!(options = calloc( 1, sizeof(*options) ))) return E_OUTOFMEMORY;

    options->maxNumberOfItems = maxNumberOfItems;
    options->startIndex = startIndex;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)options, query_result_base_FetchFoldersAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFolder %p.\n", *operation );
    return hr;
}

static HRESULT WINAPI folder_query_result_GetFoldersAsyncDefaultStartAndCount( IStorageFolderQueryResult *iface, IAsyncOperation_IVectorView_StorageFolder **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_StorageFolder };
    struct storage_query_options *options;
    if (!(options = calloc( 1, sizeof(*options) ))) return E_OUTOFMEMORY;

    options->maxNumberOfItems = INFINITE;
    options->startIndex = 0;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)options, query_result_base_FetchFoldersAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFolder %p.\n", *operation );
    return hr;
}

const struct IStorageFolderQueryResultVtbl folder_query_result_vtbl =
{
    folder_query_result_QueryInterface,
    folder_query_result_AddRef,
    folder_query_result_Release,
    /* IInspectable methods */
    folder_query_result_GetIids,
    folder_query_result_GetRuntimeClassName,
    folder_query_result_GetTrustLevel,
    /* IStorageFolderQueryResult methods */
    folder_query_result_GetFoldersAsync,
    folder_query_result_GetFoldersAsyncDefaultStartAndCount
};

struct file_query_result *impl_from_IStorageFileQueryResult( IStorageFileQueryResult *iface )
{
    return CONTAINING_RECORD( iface, struct file_query_result, IStorageFileQueryResult_iface );
}

static HRESULT WINAPI file_query_result_QueryInterface( IStorageFileQueryResult *iface, REFIID iid, void **out )
{
    struct file_query_result *impl = impl_from_IStorageFileQueryResult( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFileQueryResult ))
    {
        *out = &impl->IStorageFileQueryResult_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageQueryResultBase ))
    {
        *out = &impl->IStorageQueryResultBase_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI file_query_result_AddRef( IStorageFileQueryResult *iface )
{
    struct file_query_result *impl = impl_from_IStorageFileQueryResult( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI file_query_result_Release( IStorageFileQueryResult *iface )
{
    struct file_query_result *impl = impl_from_IStorageFileQueryResult( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI file_query_result_GetIids( IStorageFileQueryResult *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_query_result_GetRuntimeClassName( IStorageFileQueryResult *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_query_result_GetTrustLevel( IStorageFileQueryResult *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI file_query_result_GetFilesAsync( IStorageFileQueryResult *iface, UINT32 startIndex, UINT32 maxNumberOfItems, IAsyncOperation_IVectorView_StorageFile **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_StorageFile };
    struct storage_query_options *options;
    if (!(options = calloc( 1, sizeof(*options) ))) return E_OUTOFMEMORY;

    options->maxNumberOfItems = maxNumberOfItems;
    options->startIndex = startIndex;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)options, query_result_base_FetchFilesAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFile %p.\n", *operation );
    return hr;
}

static HRESULT WINAPI file_query_result_GetFilesAsyncDefaultStartAndCount( IStorageFileQueryResult *iface, IAsyncOperation_IVectorView_StorageFile **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_StorageFile };
    struct storage_query_options *options;
    if (!(options = calloc( 1, sizeof(*options) ))) return E_OUTOFMEMORY;

    options->maxNumberOfItems = INFINITE;
    options->startIndex = 0;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)options, query_result_base_FetchFilesAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFile %p.\n", *operation );
    return hr;
}

const struct IStorageFileQueryResultVtbl file_query_result_vtbl =
{
    file_query_result_QueryInterface,
    file_query_result_AddRef,
    file_query_result_Release,
    /* IInspectable methods */
    file_query_result_GetIids,
    file_query_result_GetRuntimeClassName,
    file_query_result_GetTrustLevel,
    /* IStorageFileQueryResult methods */
    file_query_result_GetFilesAsync,
    file_query_result_GetFilesAsyncDefaultStartAndCount
};

struct item_query_result *impl_from_IStorageItemQueryResult( IStorageItemQueryResult *iface )
{
    return CONTAINING_RECORD( iface, struct item_query_result, IStorageItemQueryResult_iface );
}

static HRESULT WINAPI item_query_result_QueryInterface( IStorageItemQueryResult *iface, REFIID iid, void **out )
{
    struct item_query_result *impl = impl_from_IStorageItemQueryResult( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageItemQueryResult ))
    {
        *out = &impl->IStorageItemQueryResult_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageQueryResultBase ))
    {
        *out = &impl->IStorageQueryResultBase_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI item_query_result_AddRef( IStorageItemQueryResult *iface )
{
    struct item_query_result *impl = impl_from_IStorageItemQueryResult( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI item_query_result_Release( IStorageItemQueryResult *iface )
{
    struct item_query_result *impl = impl_from_IStorageItemQueryResult( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI item_query_result_GetIids( IStorageItemQueryResult *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI item_query_result_GetRuntimeClassName( IStorageItemQueryResult *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI item_query_result_GetTrustLevel( IStorageItemQueryResult *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI item_query_result_GetItemsAsync( IStorageItemQueryResult *iface, UINT32 startIndex, UINT32 maxNumberOfItems, IAsyncOperation_IVectorView_IStorageItem **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_IStorageItem };
    struct storage_query_options *options;
    if (!(options = calloc( 1, sizeof(*options) ))) return E_OUTOFMEMORY;

    options->maxNumberOfItems = maxNumberOfItems;
    options->startIndex = startIndex;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)options, query_result_base_FetchItemsAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFile %p.\n", *operation );
    return hr;
}

static HRESULT WINAPI item_query_result_GetItemsAsyncDefaultStartAndCount( IStorageItemQueryResult *iface, IAsyncOperation_IVectorView_IStorageItem **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_IStorageItem };
    struct storage_query_options *options;
    if (!(options = calloc( 1, sizeof(*options) ))) return E_OUTOFMEMORY;

    options->maxNumberOfItems = INFINITE;
    options->startIndex = 0;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)options, query_result_base_FetchItemsAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFile %p.\n", *operation );
    return hr;
}

const struct IStorageItemQueryResultVtbl item_query_result_vtbl =
{
    item_query_result_QueryInterface,
    item_query_result_AddRef,
    item_query_result_Release,
    /* IInspectable methods */
    item_query_result_GetIids,
    item_query_result_GetRuntimeClassName,
    item_query_result_GetTrustLevel,
    /* IStorageItemQueryResult methods */
    item_query_result_GetItemsAsync,
    item_query_result_GetItemsAsyncDefaultStartAndCount
};