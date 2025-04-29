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

// Storage Folder

static struct storage_folder_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct storage_folder_statics *impl = impl_from_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageFolderStatics ))
    {
        *out = &impl->IStorageFolderStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageFolderStatics2 ))
    {
        *out = &impl->IStorageFolderStatics2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct storage_folder_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct storage_folder_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    /* IInspectable methods */
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    /* IActivationFactory methods */
    factory_ActivateInstance,
};

struct storage_folder *impl_from_IStorageFolder( IStorageFolder *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder, IStorageFolder_iface );
}

static HRESULT WINAPI storage_folder_QueryInterface( IStorageFolder *iface, REFIID iid, void **out )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFolder ))
    {
        *out = &impl->IStorageFolder_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageFolder2 ))
    {
        *out = &impl->IStorageFolder2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageItem ))
    {
        *out = &impl->IStorageItem_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageItem2 ))
    {
        *out = &impl->IStorageItem2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageItemProperties ))
    {
        *out = &impl->IStorageItemProperties_iface;
        IInspectable_AddRef( *out );        
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageItemPropertiesWithProvider ))
    {
        *out = &impl->IStorageItemPropertiesWithProvider_iface;
        IInspectable_AddRef( *out );        
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageFolderQueryOperations ))
    {
        *out = &impl->IStorageFolderQueryOperations_iface;
        IInspectable_AddRef( *out );        
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_folder_AddRef( IStorageFolder *iface )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_folder_Release( IStorageFolder *iface )
{
    struct storage_folder *impl = impl_from_IStorageFolder( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_folder_GetIids( IStorageFolder *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_GetRuntimeClassName( IStorageFolder *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_GetTrustLevel( IStorageFolder *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::StorageFolder
*/

static HRESULT WINAPI storage_folder_CreateFileAsyncOverloadDefaultOptions( IStorageFolder *iface, HSTRING name, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };
    struct storage_folder_creation_options *creation_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(creation_options = calloc( 1, sizeof(*creation_options) ))) return E_OUTOFMEMORY;

    creation_options->name = name;
    creation_options->option = CreationCollisionOption_FailIfExists;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)creation_options, storage_folder_CreateFile, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );

    free( creation_options );

    return hr;
}

static HRESULT WINAPI storage_folder_CreateFileAsync( IStorageFolder *iface, HSTRING name, CreationCollisionOption options, IAsyncOperation_StorageFile ** operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };
    struct storage_folder_creation_options *creation_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(creation_options = calloc( 1, sizeof(*creation_options) ))) return E_OUTOFMEMORY;

    creation_options->name = name;
    creation_options->option = options;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)creation_options, storage_folder_CreateFile, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );

    free( creation_options );

    return hr;
}

static HRESULT WINAPI storage_folder_CreateFolderAsyncOverloadDefaultOptions( IStorageFolder *iface, HSTRING name, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFolder };
    struct storage_folder_creation_options *creation_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(creation_options = calloc( 1, sizeof(*creation_options) ))) return E_OUTOFMEMORY;

    creation_options->name = name;
    creation_options->option = CreationCollisionOption_FailIfExists;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)creation_options, storage_folder_CreateFolder, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFolder %p.\n", *operation );

    free( creation_options );

    return hr;
}

static HRESULT WINAPI storage_folder_CreateFolderAsync( IStorageFolder *iface, HSTRING name, CreationCollisionOption options, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFolder };
    struct storage_folder_creation_options *creation_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(creation_options = calloc( 1, sizeof(*creation_options) ))) return E_OUTOFMEMORY;

    creation_options->name = name;
    creation_options->option = options;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)creation_options, storage_folder_CreateFolder, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFolder %p.\n", *operation );

    free( creation_options );

    return hr;
}

static HRESULT WINAPI storage_folder_GetFileAsync( IStorageFolder *iface, HSTRING name, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)name, storage_folder_FetchFile, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI storage_folder_GetFolderAsync( IStorageFolder *iface, HSTRING name, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFolder };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)name, storage_folder_FetchFolder, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFolder %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI storage_folder_GetItemAsync( IStorageFolder *iface, HSTRING name, IAsyncOperation_IStorageItem **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IStorageItem };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)name, storage_folder_FetchItem, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IStorageItem %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI storage_folder_GetFilesAsyncOverloadDefaultOptionsStartAndCount( IStorageFolder *iface, IAsyncOperation_IVectorView_StorageFile **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_StorageFile };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, NULL, storage_folder_FetchFilesAndCount, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFile %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI storage_folder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount( IStorageFolder *iface, IAsyncOperation_IVectorView_StorageFolder **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_StorageFolder };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, NULL, storage_folder_FetchFoldersAndCount, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_StorageFolder %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI storage_folder_GetItemsAsyncOverloadDefaultStartAndCount( IStorageFolder *iface, IAsyncOperation_IVectorView_IStorageItem **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVectorView_IStorageItem };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, NULL, storage_folder_FetchItemsAndCount, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IVectorView_IStorageItem %p.\n", *operation );

    return hr;
}

struct IStorageFolderVtbl storage_folder_vtbl =
{
    storage_folder_QueryInterface,
    storage_folder_AddRef,
    storage_folder_Release,
    /* IInspectable methods */
    storage_folder_GetIids,
    storage_folder_GetRuntimeClassName,
    storage_folder_GetTrustLevel,
    /* IStorageFolder methods */
    storage_folder_CreateFileAsyncOverloadDefaultOptions,
    storage_folder_CreateFileAsync,
    storage_folder_CreateFolderAsyncOverloadDefaultOptions,
    storage_folder_CreateFolderAsync,
    storage_folder_GetFileAsync,
    storage_folder_GetFolderAsync,
    storage_folder_GetItemAsync,
    storage_folder_GetFilesAsyncOverloadDefaultOptionsStartAndCount,
    storage_folder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount,
    storage_folder_GetItemsAsyncOverloadDefaultStartAndCount
};

struct storage_folder *impl_from_IStorageFolder2( IStorageFolder2 *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder, IStorageFolder2_iface );
}

static HRESULT WINAPI storage_folder2_QueryInterface( IStorageFolder2 *iface, REFIID iid, void **out )
{
    struct storage_folder *impl = impl_from_IStorageFolder2( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    return IStorageFolder_QueryInterface( &impl->IStorageFolder_iface, iid, out );
}

static ULONG WINAPI storage_folder2_AddRef( IStorageFolder2 *iface )
{
    struct storage_folder *impl = impl_from_IStorageFolder2( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_folder2_Release( IStorageFolder2 *iface )
{
    struct storage_folder *impl = impl_from_IStorageFolder2( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_folder2_GetIids( IStorageFolder2 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder2_GetRuntimeClassName( IStorageFolder2 *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder2_GetTrustLevel( IStorageFolder2 *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::StorageFolder2
*/

static HRESULT WINAPI storage_folder2_TryGetItemAsync( IStorageFolder2 *iface, HSTRING name, IAsyncOperation_IStorageItem **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IStorageItem };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !name ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)name, storage_folder2_TryFetchItem, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IStorageItem %p.\n", *operation );

    return hr;
}

struct IStorageFolder2Vtbl storage_folder2_vtbl =
{
    storage_folder2_QueryInterface,
    storage_folder2_AddRef,
    storage_folder2_Release,
    /* IInspectable methods */
    storage_folder2_GetIids,
    storage_folder2_GetRuntimeClassName,
    storage_folder2_GetTrustLevel,
    /* IStorageFolder methods */
    storage_folder2_TryGetItemAsync
};

DEFINE_IINSPECTABLE( storage_folder_statics, IStorageFolderStatics, struct storage_folder_statics, IActivationFactory_iface )

static HRESULT WINAPI storage_folder_statics_GetFolderFromPathAsync( IStorageFolderStatics *iface, HSTRING path, IAsyncOperation_StorageFolder **result )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFolder };

    TRACE( "iface %p, operation %p\n", iface, result );

    //Arguments
    if ( !path ) return E_INVALIDARG;
    if ( !result ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)path, storage_folder_AssignFolderAsync, iids, (IAsyncOperation_IInspectable **)result );
    TRACE( "created IAsyncOperation_StorageFolder %p.\n", *result );

    return hr;
}

static const struct IStorageFolderStaticsVtbl storage_folder_statics_vtbl =
{
    storage_folder_statics_QueryInterface,
    storage_folder_statics_AddRef,
    storage_folder_statics_Release,
    /* IInspectable methods */
    storage_folder_statics_GetIids,
    storage_folder_statics_GetRuntimeClassName,
    storage_folder_statics_GetTrustLevel,
    /* IStorageFolderStatics methods */
    storage_folder_statics_GetFolderFromPathAsync
};

DEFINE_IINSPECTABLE( storage_folder_statics2, IStorageFolderStatics2, struct storage_folder_statics, IActivationFactory_iface )

static HRESULT WINAPI storage_folder_statics2_GetFolderFromPathForUserAsync( IStorageFolderStatics2 *iface, IUser *user, HSTRING path, IAsyncOperation_StorageFolder **result )
{
    //IUser is not used
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFolder };

    TRACE( "iface %p, operation %p\n", iface, result );

    //Arguments
    if ( !path ) return E_INVALIDARG;
    if ( !result ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)path, storage_folder_AssignFolderAsync, iids, (IAsyncOperation_IInspectable **)result );
    TRACE( "created IAsyncOperation_StorageFolder %p.\n", *result );

    return hr;
}

static const struct IStorageFolderStatics2Vtbl storage_folder_statics2_vtbl =
{
    storage_folder_statics2_QueryInterface,
    storage_folder_statics2_AddRef,
    storage_folder_statics2_Release,
    /* IInspectable methods */
    storage_folder_statics2_GetIids,
    storage_folder_statics2_GetRuntimeClassName,
    storage_folder_statics2_GetTrustLevel,
    /* IStorageFolderStatics2 methods */
    storage_folder_statics2_GetFolderFromPathForUserAsync
};

static struct storage_folder_statics storage_folder_statics =
{
    {&factory_vtbl},
    {&storage_folder_statics_vtbl},
    {&storage_folder_statics2_vtbl},
    1,
};

IActivationFactory *storage_folder_factory = &storage_folder_statics.IActivationFactory_iface;
