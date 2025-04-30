/* WinRT Windows.Storage.StorageFile Implementation
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

#include "StorageFileInternal.h"

_ENABLE_DEBUGGING_

// Storage File

static struct storage_file_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{

    struct storage_file_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IStorageFileStatics ))
    {
        *out = &impl->IStorageFileStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IStorageFileStatics2 ))
    {
        *out = &impl->IStorageFileStatics2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct storage_file_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct storage_file_statics *impl = impl_from_IActivationFactory( iface );
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

struct storage_file *impl_from_IStorageFile( IStorageFile *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IStorageFile_iface );
}

static HRESULT WINAPI storage_file_QueryInterface( IStorageFile *iface, REFIID iid, void **out )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageFile ))
    {
        *out = &impl->IStorageFile_iface;
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

    if (IsEqualGUID( iid, &IID_IStorageFilePropertiesWithAvailability ))
    {
        *out = &impl->IStorageFilePropertiesWithAvailability_iface;
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

    if (IsEqualGUID( iid, &IID_IRandomAccessStreamReference ))
    {
        *out = impl->IRandomAccessStreamReference_iface;
        IInspectable_AddRef( *out );        
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_file_AddRef( IStorageFile *iface )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_file_Release( IStorageFile *iface )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_file_GetIids( IStorageFile *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_GetRuntimeClassName( IStorageFile *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_GetTrustLevel( IStorageFile *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::StorageFile
*/

static HRESULT WINAPI storage_file_get_FileType( IStorageFile *iface, HSTRING *value )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    
    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;

    return WindowsDuplicateString( impl->FileType, value );
}

static HRESULT WINAPI storage_file_get_ContentType( IStorageFile *iface, HSTRING *value )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;

    return WindowsDuplicateString( impl->ContentType, value );
}

static HRESULT WINAPI storage_file_OpenAsync( IStorageFile *iface, FileAccessMode mode, IAsyncOperation_IRandomAccessStream **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IRandomAccessStream };

    TRACE( "iface %p, mode %d value %p\n", iface, mode, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)mode, storage_file_Open, iids, (IAsyncOperation_IInspectable **)operation );

    return hr;
}

static HRESULT WINAPI storage_file_OpenTransactedWriteAsync( IStorageFile *iface, IAsyncOperation_StorageStreamTransaction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_CopyOverloadDefaultNameAndOptions( IStorageFile *iface, IStorageFolder *folder, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;
    HSTRING name;

    struct storage_file_copy_options *copy_options;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };
    struct storage_file *impl = impl_from_IStorageFile( iface );
    struct storage_item *implItem = impl_from_IStorageItem( &impl->IStorageItem_iface );

    TRACE( "iface %p, folder %p, value %p\n", iface, folder, operation );

    //Arguments
    if ( !folder ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    WindowsDuplicateString( implItem->Name, &name );

    if (!(copy_options = calloc( 1, sizeof(*copy_options) ))) return E_OUTOFMEMORY;

    copy_options->folder = folder;
    copy_options->name = name;
    copy_options->option = NameCollisionOption_FailIfExists;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)copy_options, storage_file_Copy, iids, (IAsyncOperation_IInspectable **)operation );
    
    free( copy_options );
    WindowsDeleteString( name );

    return hr;
}

static HRESULT WINAPI storage_file_CopyOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder, HSTRING name, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    struct storage_file_copy_options *copy_options;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };

    TRACE( "iface %p, folder %p, value %p\n", iface, folder, operation );

    //Arguments
    if ( !folder || !name || WindowsIsStringEmpty( name ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(copy_options = calloc( 1, sizeof(*copy_options) ))) return E_OUTOFMEMORY;

    copy_options->folder = folder;
    copy_options->name = name;
    copy_options->option = NameCollisionOption_FailIfExists;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)copy_options, storage_file_Copy, iids, (IAsyncOperation_IInspectable **)operation );
    
    free( copy_options );

    return hr;
}

static HRESULT WINAPI storage_file_CopyOverload( IStorageFile *iface, IStorageFolder *folder, HSTRING name, NameCollisionOption option, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    struct storage_file_move_options *copy_options;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };

    TRACE( "iface %p, folder %p, value %p\n", iface, folder, operation );

    //Arguments
    if ( !folder || !name || WindowsIsStringEmpty( name ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(copy_options = calloc( 1, sizeof(*copy_options) ))) return E_OUTOFMEMORY;

    copy_options->folder = folder;
    copy_options->name = name;
    copy_options->option = option;
    
    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)copy_options, storage_file_Copy, iids, (IAsyncOperation_IInspectable **)operation );

    free( copy_options );

    return hr;
}

static HRESULT WINAPI storage_file_CopyAndReplaceAsync( IStorageFile *iface, IStorageFile *file, IAsyncAction **operation )
{
    HRESULT hr;

    TRACE( "iface %p, folder %p, value %p\n", iface, file, operation );

    //Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)file, storage_file_CopyAndReplace, operation );

    return hr;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultNameAndOptions( IStorageFile *iface, IStorageFolder *folder, IAsyncAction **operation )
{
    HRESULT hr;
    HSTRING name;

    struct storage_file_move_options *move_options;
    struct storage_file *impl = impl_from_IStorageFile( iface );
    struct storage_item *implItem = impl_from_IStorageItem( &impl->IStorageItem_iface );

    TRACE( "iface %p, folder %p, value %p\n", iface, folder, operation );

    //Arguments
    if ( !folder ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    WindowsDuplicateString( implItem->Name, &name );

    if (!(move_options = calloc( 1, sizeof(*move_options) ))) return E_OUTOFMEMORY;

    move_options->folder = folder;
    move_options->name = name;
    move_options->option = NameCollisionOption_FailIfExists;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)move_options, storage_file_Move, operation );
    
    free( move_options );
    WindowsDeleteString( name );

    return hr;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder, HSTRING name, IAsyncAction **operation )
{
    HRESULT hr;

    struct storage_file_move_options *move_options;

    TRACE( "iface %p, folder %p, value %p\n", iface, folder, operation );

    //Arguments
    if ( !folder || !name || WindowsIsStringEmpty( name ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(move_options = calloc( 1, sizeof(*move_options) ))) return E_OUTOFMEMORY;

    move_options->folder = folder;
    move_options->name = name;
    move_options->option = NameCollisionOption_FailIfExists;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)move_options, storage_file_Move, operation );

    free( move_options );

    return hr;
}

static HRESULT WINAPI storage_file_MoveOverload( IStorageFile *iface, IStorageFolder *folder, HSTRING name, NameCollisionOption option, IAsyncAction **operation )
{
    HRESULT hr;
    struct storage_file_move_options *move_options;

    TRACE( "iface %p, folder %p, value %p\n", iface, folder, operation );

    //Arguments
    if ( !folder || !name || WindowsIsStringEmpty( name ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(move_options = calloc( 1, sizeof(*move_options) ))) return E_OUTOFMEMORY;

    move_options->folder = folder;
    move_options->name = name;
    move_options->option = option;
    
    hr = async_action_create( (IUnknown *)iface, (IUnknown *)move_options, storage_file_Move, operation );

    free( move_options );

    return hr;
}

static HRESULT WINAPI storage_file_MoveAndReplaceAsync( IStorageFile *iface, IStorageFile *file, IAsyncAction **operation )
{
    HRESULT hr;

    TRACE( "iface %p, file %p, value %p\n", iface, file, operation );

    //Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)file, storage_file_MoveAndReplace, operation );

    return hr;
}

struct IStorageFileVtbl storage_file_vtbl =
{
    storage_file_QueryInterface,
    storage_file_AddRef,
    storage_file_Release,
    /* IInspectable methods */
    storage_file_GetIids,
    storage_file_GetRuntimeClassName,
    storage_file_GetTrustLevel,
    /* IStorageFile methods */
    storage_file_get_FileType,
    storage_file_get_ContentType,
    storage_file_OpenAsync,
    storage_file_OpenTransactedWriteAsync,
    storage_file_CopyOverloadDefaultNameAndOptions,
    storage_file_CopyOverloadDefaultOptions,
    storage_file_CopyOverload,
    storage_file_CopyAndReplaceAsync,
    storage_file_MoveOverloadDefaultNameAndOptions,
    storage_file_MoveOverloadDefaultOptions,
    storage_file_MoveOverload,
    storage_file_MoveAndReplaceAsync
};

struct storage_file *impl_from_IStorageFilePropertiesWithAvailability( IStorageFilePropertiesWithAvailability *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file, IStorageFilePropertiesWithAvailability_iface );
}

static HRESULT WINAPI storage_file_properties_with_availability_QueryInterface( IStorageFilePropertiesWithAvailability *iface, REFIID iid, void **out )
{
    struct storage_file *impl = impl_from_IStorageFilePropertiesWithAvailability( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    return IStorageFile_QueryInterface( &impl->IStorageFile_iface, iid, out );
}

static ULONG WINAPI storage_file_properties_with_availability_AddRef( IStorageFilePropertiesWithAvailability *iface )
{
    struct storage_file *impl = impl_from_IStorageFilePropertiesWithAvailability( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_file_properties_with_availability_Release( IStorageFilePropertiesWithAvailability *iface )
{
    struct storage_file *impl = impl_from_IStorageFilePropertiesWithAvailability( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_file_properties_with_availability_GetIids( IStorageFilePropertiesWithAvailability *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_properties_with_availability_GetRuntimeClassName( IStorageFilePropertiesWithAvailability *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_properties_with_availability_GetTrustLevel( IStorageFilePropertiesWithAvailability *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::StorageFile
*/

static HRESULT WINAPI storage_file_properties_with_availability_get_IsAvailable( IStorageFilePropertiesWithAvailability *iface, boolean *value )
{
    struct storage_file *impl = impl_from_IStorageFilePropertiesWithAvailability( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;

    storage_file_properties_with_availability_IsAvailable( &impl->IStorageItem_iface, value );

    return S_OK;
}

struct IStorageFilePropertiesWithAvailabilityVtbl storage_file_properties_with_availability_vtbl =
{
    storage_file_properties_with_availability_QueryInterface,
    storage_file_properties_with_availability_AddRef,
    storage_file_properties_with_availability_Release,
    /* IInspectable methods */
    storage_file_properties_with_availability_GetIids,
    storage_file_properties_with_availability_GetRuntimeClassName,
    storage_file_properties_with_availability_GetTrustLevel,
    /* IStorageFilePropertiesWithAvailability methods */
    storage_file_properties_with_availability_get_IsAvailable
};

DEFINE_IINSPECTABLE( storage_file_statics, IStorageFileStatics, struct storage_file_statics, IActivationFactory_iface )

static HRESULT WINAPI storage_file_statics_GetFileFromPathAsync( IStorageFileStatics *iface, HSTRING path, IAsyncOperation_StorageFile **result )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };

    TRACE( "iface %p, result %p\n", iface, result );

    //Arguments
    if ( !path || WindowsIsStringEmpty( path ) ) return E_INVALIDARG;
    if ( !result ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)path, storage_file_AssignFileAsync, iids, (IAsyncOperation_IInspectable **)result );
    TRACE( "created IAsyncOperation_StorageFile %p.\n", *result );

    return hr;
}

static HRESULT WINAPI storage_file_statics_GetFileFromApplicationUriAsync( IStorageFileStatics *iface, IUriRuntimeClass *uri, IAsyncOperation_StorageFile **result )
{
    HRESULT hr = S_OK;
    HSTRING uriScheme;
    HSTRING uriPath;
    HSTRING appDataPath;
    HSTRING path;
    WCHAR pathStr[MAX_PATH];

    IAppDataPathsStatics *app_data_paths_statics = NULL;
    IAppDataPaths *app_data_paths = NULL;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_AppDataPaths, app_data_paths_statics, IID_IAppDataPathsStatics );

    TRACE( "iface %p, path %p, result %p\n", iface, uri, result );

    //Arguments
    if ( !uri ) return E_INVALIDARG;
    if ( !result ) return E_POINTER;

    hr = IAppDataPathsStatics_GetDefault( app_data_paths_statics, &app_data_paths );
    if ( FAILED( hr ) ) goto _CLEANUP;
    IAppDataPaths_get_LocalAppData( app_data_paths, &appDataPath );

    IUriRuntimeClass_get_SchemeName( uri, &uriScheme );
    IUriRuntimeClass_get_Domain( uri, &uriPath );
    
    //ms-appx: appx install path
    //ms-appdata: appx app data

    if ( !wcscmp( WindowsGetStringRawBuffer( uriScheme, NULL ), L"ms-appx" ) )
    {
        GetModuleFileNameW(NULL, pathStr, MAX_PATH);
        PathAppendW( pathStr, WindowsGetStringRawBuffer( uriPath, NULL ) );
    } else if ( !wcscmp( WindowsGetStringRawBuffer( uriScheme, NULL ), L"ms-appdata" ) )
    {
        wcscpy( pathStr, WindowsGetStringRawBuffer( appDataPath, NULL ) );
        PathAppendW( pathStr, WindowsGetStringRawBuffer( uriPath, NULL ) );
    } else {
        hr = E_INVALIDARG;
        goto _CLEANUP;
    }

    if ( SUCCEEDED( hr ) )
    {
        WindowsCreateString( pathStr, wcslen( pathStr ), &path );
        hr = async_operation_create( (IUnknown *)iface, (IUnknown *)path, storage_file_AssignFileAsync, iids, (IAsyncOperation_IInspectable **)result );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *result );
    }

_CLEANUP:
    if ( app_data_paths_statics )
        IAppDataPathsStatics_Release( app_data_paths_statics );
    if ( app_data_paths )
        IAppDataPaths_Release( app_data_paths );
    
    return hr;
}

// UNIX has no concept of "File Thumbnails". thumbnail is to be ignored.
// Storage.Stream needs to be implemented.
static HRESULT WINAPI storage_file_statics_CreateStreamedFileAsync( IStorageFileStatics *iface, HSTRING displayNameWithExtension, IStreamedFileDataRequestedHandler *dataRequested, IRandomAccessStreamReference *thumbnail, IAsyncOperation_StorageFile **result )
{
    FIXME( "iface %p, result %p stub!\n", iface, result );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_statics_ReplaceWithStreamedFileAsync( IStorageFileStatics *iface, IStorageFile *fileToReplace, IStreamedFileDataRequestedHandler *dataRequested, IRandomAccessStreamReference *thumbnail, IAsyncOperation_StorageFile **result )
{
    FIXME( "iface %p, result %p stub!\n", iface, result );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_statics_CreateStreamedFileFromUriAsync( IStorageFileStatics *iface, HSTRING displayNameWithExtension, IUriRuntimeClass *uri, IRandomAccessStreamReference *thumbnail, IAsyncOperation_StorageFile **result )
{
    FIXME( "iface %p, result %p stub!\n", iface, result );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_statics_ReplaceWithStreamedFileFromUriAsync( IStorageFileStatics *iface, IStorageFile *fileToReplace, IUriRuntimeClass *uri, IRandomAccessStreamReference *thumbnail, IAsyncOperation_StorageFile **result )
{
    FIXME( "iface %p, result %p stub!\n", iface, result );
    return E_NOTIMPL;
}

static const struct IStorageFileStaticsVtbl storage_file_statics_vtbl =
{
    storage_file_statics_QueryInterface,
    storage_file_statics_AddRef,
    storage_file_statics_Release,
    /* IInspectable methods */
    storage_file_statics_GetIids,
    storage_file_statics_GetRuntimeClassName,
    storage_file_statics_GetTrustLevel,
    /* IStorageFileStatics methods */
    storage_file_statics_GetFileFromPathAsync,
    storage_file_statics_GetFileFromApplicationUriAsync,
    storage_file_statics_CreateStreamedFileAsync,
    storage_file_statics_ReplaceWithStreamedFileAsync,
    storage_file_statics_CreateStreamedFileFromUriAsync,
    storage_file_statics_ReplaceWithStreamedFileFromUriAsync
};

DEFINE_IINSPECTABLE( storage_file_statics2, IStorageFileStatics2, struct storage_file_statics, IActivationFactory_iface )

static HRESULT WINAPI storage_file_statics2_GetFileFromPathForUserAsync( IStorageFileStatics2 *iface, IUser *user, HSTRING path, IAsyncOperation_StorageFile **result )
{
    //User is not used.
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFile };

    TRACE( "iface %p, path %p, result %p\n", iface, path, result );

    //Arguments
    if ( !path || WindowsIsStringEmpty( path ) ) return E_INVALIDARG;
    if ( !result ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)path, storage_file_AssignFileAsync, iids, (IAsyncOperation_IInspectable **)result );
    TRACE( "created IAsyncOperation_StorageFile %p.\n", *result );

    return hr;
}

static const struct IStorageFileStatics2Vtbl storage_file_statics2_vtbl =
{
    storage_file_statics2_QueryInterface,
    storage_file_statics2_AddRef,
    storage_file_statics2_Release,
    /* IInspectable methods */
    storage_file_statics2_GetIids,
    storage_file_statics2_GetRuntimeClassName,
    storage_file_statics2_GetTrustLevel,
    /* IStorageFileStatics methods */
    storage_file_statics2_GetFileFromPathForUserAsync
};

static struct storage_file_statics storage_file_statics =
{
    {&factory_vtbl},
    {&storage_file_statics_vtbl},
    {&storage_file_statics2_vtbl},
    1,
};

IActivationFactory *storage_file_factory = &storage_file_statics.IActivationFactory_iface;
