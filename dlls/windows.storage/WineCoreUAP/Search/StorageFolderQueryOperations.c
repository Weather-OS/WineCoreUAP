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

// Storage Folder Query Operations

struct storage_folder_query_operations *impl_from_IStorageFolderQueryOperations( IStorageFolderQueryOperations *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder_query_operations, IStorageFolderQueryOperations_iface );
}

static HRESULT WINAPI storage_folder_query_operations_QueryInterface( IStorageFolderQueryOperations *iface, REFIID iid, void **out )
{
    struct storage_folder_query_operations *impl = impl_from_IStorageFolderQueryOperations( iface );

    // Inheritance 
    struct storage_folder *inheritedFolder = CONTAINING_RECORD( &impl->IStorageFolderQueryOperations_iface, struct storage_folder, IStorageFolderQueryOperations_iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFolderQueryOperations ))
    {
        *out = &impl->IStorageFolderQueryOperations_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if ( inheritedFolder->IStorageFolder_iface.lpVtbl == &storage_folder_vtbl )
    {
        return IStorageFolder_QueryInterface( &inheritedFolder->IStorageFolder_iface, iid, out );
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_folder_query_operations_AddRef( IStorageFolderQueryOperations *iface )
{
    struct storage_folder_query_operations *impl = impl_from_IStorageFolderQueryOperations( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_folder_query_operations_Release( IStorageFolderQueryOperations *iface )
{
    struct storage_folder_query_operations *impl = impl_from_IStorageFolderQueryOperations( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_folder_query_operations_GetIids( IStorageFolderQueryOperations *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_query_operations_GetRuntimeClassName( IStorageFolderQueryOperations *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_query_operations_GetTrustLevel( IStorageFolderQueryOperations *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_query_operations_GetIndexedStateAsync( IStorageFolderQueryOperations *iface, IAsyncOperation_IndexedState **operation )
{
    HRESULT hr;    
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IndexedState };
    TRACE( "iface %p, operation %p\n", iface, operation );
    if ( !operation ) return E_POINTER;
    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)NULL, storage_folder_query_operations_GetIndexedState, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IndexedState %p.\n", operation );
    return hr;
}

static HRESULT WINAPI storage_folder_query_operations_CreateFileQueryOverloadDefault( IStorageFolderQueryOperations *iface, IStorageFileQueryResult **value )
{
    IQueryOptions *options;

    HRESULT hr;

    struct query_options_factory *queryOptionsFactory;

    TRACE( "iface %p, value %p.\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if (!(queryOptionsFactory = calloc( 1, sizeof(*queryOptionsFactory) ))) return E_OUTOFMEMORY;

    queryOptionsFactory->IQueryOptionsFactory_iface.lpVtbl = &query_options_factory_vtbl;
    queryOptionsFactory->ref = 1;

    hr = IQueryOptionsFactory_CreateCommonFileQuery( &queryOptionsFactory->IQueryOptionsFactory_iface, CommonFileQuery_DefaultQuery, NULL, &options );
    if ( FAILED( hr ) ) return hr;

    return storage_folder_query_operations_FetchFileQuery( iface, options, value );
}

static HRESULT WINAPI storage_folder_query_operations_CreateFileQuery( IStorageFolderQueryOperations *iface, CommonFileQuery query, IStorageFileQueryResult **value )
{
    IQueryOptions *options;

    HRESULT hr;

    struct query_options_factory *queryOptionsFactory;

    TRACE( "iface %p, query %d, value %p.\n", iface, query, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if (!(queryOptionsFactory = calloc( 1, sizeof(*queryOptionsFactory) ))) return E_OUTOFMEMORY;

    queryOptionsFactory->IQueryOptionsFactory_iface.lpVtbl = &query_options_factory_vtbl;
    queryOptionsFactory->ref = 1;

    hr = IQueryOptionsFactory_CreateCommonFileQuery( &queryOptionsFactory->IQueryOptionsFactory_iface, query, NULL, &options );
    if ( FAILED( hr ) ) return hr;

    return storage_folder_query_operations_FetchFileQuery( iface, options, value );
}

static HRESULT WINAPI storage_folder_query_operations_CreateFileQueryWithOptions( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageFileQueryResult **value )
{
    TRACE( "iface %p, queryOptions %p, value %p.\n", iface, queryOptions, value );
    // Arguments    
    if ( !queryOptions ) return E_INVALIDARG;
    if ( !value ) return E_POINTER;    
    return storage_folder_query_operations_FetchFileQuery( iface, queryOptions, value );
}

static HRESULT WINAPI storage_folder_query_operations_CreateFolderQueryOverloadDefault( IStorageFolderQueryOperations *iface, IStorageFolderQueryResult **value )
{
    IQueryOptions *options;

    HRESULT hr;

    struct query_options_factory *queryOptionsFactory;

    TRACE( "iface %p, value %p.\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if (!(queryOptionsFactory = calloc( 1, sizeof(*queryOptionsFactory) ))) return E_OUTOFMEMORY;

    queryOptionsFactory->IQueryOptionsFactory_iface.lpVtbl = &query_options_factory_vtbl;
    queryOptionsFactory->ref = 1;

    hr = IQueryOptionsFactory_CreateCommonFolderQuery( &queryOptionsFactory->IQueryOptionsFactory_iface, CommonFolderQuery_DefaultQuery, &options );
    if ( FAILED( hr ) ) return hr;

    return storage_folder_query_operations_FetchFolderQuery( iface, options, value );
}

static HRESULT WINAPI storage_folder_query_operations_CreateFolderQuery( IStorageFolderQueryOperations *iface, CommonFolderQuery query, IStorageFolderQueryResult **value )
{
    IQueryOptions *options;

    HRESULT hr;

    struct query_options_factory *queryOptionsFactory;

    TRACE( "iface %p, query %d, value %p.\n", iface, query, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if (!(queryOptionsFactory = calloc( 1, sizeof(*queryOptionsFactory) ))) return E_OUTOFMEMORY;

    queryOptionsFactory->IQueryOptionsFactory_iface.lpVtbl = &query_options_factory_vtbl;
    queryOptionsFactory->ref = 1;

    hr = IQueryOptionsFactory_CreateCommonFolderQuery( &queryOptionsFactory->IQueryOptionsFactory_iface, query, &options );
    if ( FAILED( hr ) ) return hr;

    return storage_folder_query_operations_FetchFolderQuery( iface, options, value );
}

static HRESULT WINAPI storage_folder_query_operations_CreateFolderQueryWithOptions( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageFolderQueryResult **value )
{
    TRACE( "iface %p, queryOptions %p, value %p.\n", iface, queryOptions, value );
    // Arguments
    if ( !queryOptions ) return E_INVALIDARG;
    if ( !value ) return E_POINTER;
    return storage_folder_query_operations_FetchFolderQuery( iface, queryOptions, value );
}

static HRESULT WINAPI storage_folder_query_operations_CreateItemQuery( IStorageFolderQueryOperations *iface, IStorageItemQueryResult **value )
{
    HRESULT hr;

    struct query_options *queryOptions;

    //non-runtime class require redefinition
    #undef SortEntry
    DEFINE_VECTOR_IIDS( SortEntry )
    #define SortEntry __x_ABI_CWindows_CStorage_CSearch_CSortEntry

    TRACE( "iface %p, value %p.\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if (!(queryOptions = calloc( 1, sizeof(*queryOptions) ))) return E_OUTOFMEMORY;

    queryOptions->IQueryOptions_iface.lpVtbl = &query_options_vtbl;

    hr = vector_create( &SortEntry_iids, (void **)&queryOptions->SortOrders );
    if ( FAILED( hr ) ) return hr;

    queryOptions->Depth = FolderDepth_Shallow;
    queryOptions->Indexer = IndexerOption_DoNotUseIndexer;
    queryOptions->DateStack = DateStackOption_None;
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->ApplicationSearchFilter );
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->UserSearchFilter );
    WindowsCreateString( L"en", wcslen( L"en" ), &queryOptions->Language );
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->GroupPropertyName );
    queryOptions->ref = 1;

    return storage_folder_query_operations_FetchItemQuery( iface, &queryOptions->IQueryOptions_iface, value );
}

static HRESULT WINAPI storage_folder_query_operations_CreateItemQueryWithOptions( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, IStorageItemQueryResult **value )
{
    TRACE( "iface %p, queryOptions %p, value %p.\n", iface, queryOptions, value );
    // Arguments
    if ( !queryOptions ) return E_INVALIDARG;
    if ( !value ) return E_POINTER;
    return storage_folder_query_operations_FetchItemQuery( iface, queryOptions, value );
}

static HRESULT WINAPI storage_folder_query_operations_GetFilesAsync( IStorageFolderQueryOperations *iface, CommonFileQuery query, UINT32 startIndex, UINT32 maxItemsToRetrieve, IAsyncOperation_IVectorView_StorageFile **operation )
{
    IStorageFileQueryResult *queryResult;

    HRESULT hr = S_OK;

    TRACE( "iface %p, query %d, startIndex %d, maxItemsToRetrieve %d, operation %p.\n", iface, query, startIndex, maxItemsToRetrieve, operation );

    // Arguments
    if ( !startIndex || !maxItemsToRetrieve ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = storage_folder_query_operations_CreateFileQuery( iface, query, &queryResult );
    if ( FAILED( hr ) ) return hr;

    hr = IStorageFileQueryResult_GetFilesAsync( queryResult, startIndex, maxItemsToRetrieve, operation );
    
    return hr;
}

static HRESULT WINAPI storage_folder_query_operations_GetFilesAsyncOverloadDefaultStartAndCount( IStorageFolderQueryOperations *iface, CommonFileQuery query, IAsyncOperation_IVectorView_StorageFile **operation )
{
    IStorageFileQueryResult *queryResult;

    HRESULT hr = S_OK;

    TRACE( "iface %p, query %d, operation %p.\n", iface, query, operation );

    // Arguments
    if ( !operation ) return E_POINTER;

    hr = storage_folder_query_operations_CreateFileQuery( iface, query, &queryResult );
    if ( FAILED( hr ) ) return hr;

    hr = IStorageFileQueryResult_GetFilesAsyncDefaultStartAndCount( queryResult, operation );
    
    return hr;
}

static HRESULT WINAPI storage_folder_query_operations_GetFoldersAsync( IStorageFolderQueryOperations *iface, CommonFolderQuery query, UINT32 startIndex, UINT32 maxItemsToRetrieve, IAsyncOperation_IVectorView_StorageFolder **operation )
{
    IStorageFolderQueryResult *queryResult;

    HRESULT hr = S_OK;

    TRACE( "iface %p, query %d, startIndex %d, maxItemsToRetrieve %d, operation %p.\n", iface, query, startIndex, maxItemsToRetrieve, operation );

    // Arguments
    if ( !startIndex || !maxItemsToRetrieve ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = storage_folder_query_operations_CreateFolderQuery( iface, query, &queryResult );
    if ( FAILED( hr ) ) return hr;

    hr = IStorageFolderQueryResult_GetFoldersAsync( queryResult, startIndex, maxItemsToRetrieve, operation );
    
    return hr;
}

static HRESULT WINAPI storage_folder_query_operations_GetFoldersAsyncOverloadDefaultStartAndCount( IStorageFolderQueryOperations *iface, CommonFolderQuery query, IAsyncOperation_IVectorView_StorageFolder **operation )
{
    IStorageFolderQueryResult *queryResult;

    HRESULT hr = S_OK;

    TRACE( "iface %p, query %d, operation %p.\n", iface, query, operation );

    // Arguments
    if ( !operation ) return E_POINTER;

    hr = storage_folder_query_operations_CreateFolderQuery( iface, query, &queryResult );
    if ( FAILED( hr ) ) return hr;

    hr = IStorageFolderQueryResult_GetFoldersAsyncDefaultStartAndCount( queryResult, operation );
    
    return hr;
}

static HRESULT WINAPI storage_folder_query_operations_GetItemsAsync( IStorageFolderQueryOperations *iface, UINT32 startIndex, UINT32 maxItemsToRetrieve, IAsyncOperation_IVectorView_IStorageItem **operation )
{
    IStorageItemQueryResult *queryResult;

    HRESULT hr = S_OK;

    TRACE( "iface %p, startIndex %d, maxItemsToRetrieve %d, operation %p.\n", iface, startIndex, maxItemsToRetrieve, operation );

    // Arguments
    if ( !startIndex || !maxItemsToRetrieve ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = storage_folder_query_operations_CreateItemQuery( iface, &queryResult );
    if ( FAILED( hr ) ) return hr;

    hr = IStorageItemQueryResult_GetItemsAsync( queryResult, startIndex, maxItemsToRetrieve, operation );
    
    return hr;
}

static HRESULT WINAPI storage_folder_query_operations_AreQueryOptionsSupported( IStorageFolderQueryOperations *iface, IQueryOptions *queryOptions, boolean *value )
{
    FIXME( "iface %p, options %p stub!\n", iface, queryOptions );
    if ( !value ) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI storage_folder_query_operations_IsCommonFolderQuerySupported( IStorageFolderQueryOperations *iface, CommonFolderQuery query, boolean *value )
{
    FIXME( "iface %p, query %d stub!\n", iface, query );
    if ( !value ) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

static HRESULT WINAPI storage_folder_query_operations_IsCommonFileQuerySupported( IStorageFolderQueryOperations *iface, CommonFileQuery query, boolean *value )
{
    FIXME( "iface %p, query %d stub!\n", iface, query );
    if ( !value ) return E_POINTER;
    *value = TRUE;
    return S_OK;
}

struct IStorageFolderQueryOperationsVtbl storage_folder_query_operations_vtbl =
{
    storage_folder_query_operations_QueryInterface,
    storage_folder_query_operations_AddRef,
    storage_folder_query_operations_Release,
    /* IInspectable methods */
    storage_folder_query_operations_GetIids,
    storage_folder_query_operations_GetRuntimeClassName,
    storage_folder_query_operations_GetTrustLevel,
    /* IStorageFolderQueryOperations methods */
    storage_folder_query_operations_GetIndexedStateAsync,
    storage_folder_query_operations_CreateFileQueryOverloadDefault,
    storage_folder_query_operations_CreateFileQuery,
    storage_folder_query_operations_CreateFileQueryWithOptions,
    storage_folder_query_operations_CreateFolderQueryOverloadDefault,
    storage_folder_query_operations_CreateFolderQuery,
    storage_folder_query_operations_CreateFolderQueryWithOptions,
    storage_folder_query_operations_CreateItemQuery,
    storage_folder_query_operations_CreateItemQueryWithOptions,
    storage_folder_query_operations_GetFilesAsync,
    storage_folder_query_operations_GetFilesAsyncOverloadDefaultStartAndCount,
    storage_folder_query_operations_GetFoldersAsync,
    storage_folder_query_operations_GetFoldersAsyncOverloadDefaultStartAndCount,
    storage_folder_query_operations_GetItemsAsync,
    storage_folder_query_operations_AreQueryOptionsSupported,
    storage_folder_query_operations_IsCommonFolderQuerySupported,
    storage_folder_query_operations_IsCommonFileQuerySupported
};