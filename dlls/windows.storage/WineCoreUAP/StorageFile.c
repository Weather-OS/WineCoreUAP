/* WinRT Windows.Storage.StorageFile Implementation
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
#include "StorageFileInternal.h"
#include "AppInternalPaths.h"

#include "../private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

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
    *value = impl->FileType;
    return S_OK;
}

static HRESULT WINAPI storage_file_get_ContentType( IStorageFile *iface, HSTRING *value )
{
    struct storage_file *impl = impl_from_IStorageFile( iface );
    *value = impl->ContentType;
    return S_OK;
}

static HRESULT WINAPI storage_file_OpenAsync( IStorageFile *iface, FileAccessMode mode, IAsyncOperation_IRandomAccessStream **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
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
    NameCollisionOption option = NameCollisionOption_FailIfExists;
    struct storage_file *impl = impl_from_IStorageFile( iface );
    struct storage_item *implItem = impl_from_IStorageItem( &impl->IStorageItem_iface );

    WindowsDuplicateString( implItem->Name, &name );

    hr = storage_file_Copy( iface, folder, name, option );
    if( SUCCEEDED( hr ) )
        hr = async_operation_storage_file_create( (IUnknown *)iface, (IUnknown *)implItem->Path, storage_file_AssignFile, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    
    return hr;
}

static HRESULT WINAPI storage_file_CopyOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder, HSTRING name, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;
    NameCollisionOption option = NameCollisionOption_FailIfExists;
    struct storage_file *impl = impl_from_IStorageFile( iface );
    struct storage_item *implItem = impl_from_IStorageItem( &impl->IStorageItem_iface );

    hr = storage_file_Copy( iface, folder, name, option );
    if( SUCCEEDED( hr ) )
        hr = async_operation_storage_file_create( (IUnknown *)iface, (IUnknown *)implItem->Path, storage_file_AssignFile, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    
    return hr;
}

static HRESULT WINAPI storage_file_CopyOverload( IStorageFile *iface, IStorageFolder *folder, HSTRING name, NameCollisionOption option, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;
    struct storage_file *impl = impl_from_IStorageFile( iface );
    struct storage_item *implItem = impl_from_IStorageItem( &impl->IStorageItem_iface );

    hr = storage_file_Copy( iface, folder, name, option );
    if( SUCCEEDED( hr ) )
        hr = async_operation_storage_file_create( (IUnknown *)iface, (IUnknown *)implItem->Path, storage_file_AssignFile, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    
    return hr;
}

static HRESULT WINAPI storage_file_CopyAndReplaceAsync( IStorageFile *iface, IStorageFile *file, IAsyncAction **operation )
{
    HRESULT hr;
    HSTRING name;
    NameCollisionOption option = NameCollisionOption_ReplaceExisting;
    struct storage_file *impl = impl_from_IStorageFile( iface );
    struct storage_item *implItem = impl_from_IStorageItem( &impl->IStorageItem_iface );

    WindowsDuplicateString( implItem->Name, &name );

    hr = storage_file_Copy( iface, folder, name, option );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultNameAndOptions( IStorageFile *iface, IStorageFolder *folder, IAsyncAction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverloadDefaultOptions( IStorageFile *iface, IStorageFolder *folder, HSTRING name, IAsyncAction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveOverload( IStorageFile *iface, IStorageFolder *folder, HSTRING name, NameCollisionOption option, IAsyncAction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_file_MoveAndReplaceAsync( IStorageFile *iface, IStorageFile *file,  IAsyncAction **operation )
{
    FIXME( "iface %p, operation %p stub!\n", iface, operation );
    return E_NOTIMPL;
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

DEFINE_IINSPECTABLE( storage_file_statics, IStorageFileStatics, struct storage_file_statics, IActivationFactory_iface )

static HRESULT WINAPI storage_file_statics_GetFileFromPathAsync( IStorageFileStatics *iface, HSTRING path, IAsyncOperation_StorageFile **result )
{
    HRESULT hr;
    hr = async_operation_storage_file_create( (IUnknown *)iface, (IUnknown *)path, storage_file_AssignFile, result );
    TRACE( "created IAsyncOperation_StorageFile %p.\n", *result );
    return hr;
}

static HRESULT WINAPI storage_file_statics_GetFileFromApplicationUriAsync( IStorageFileStatics *iface, IUriRuntimeClass *uri, IAsyncOperation_StorageFile **result )
{
    HRESULT hr;
    HSTRING uriScheme;
    HSTRING uriPath;
    HSTRING appDataPath;
    HSTRING path;
    LPCSTR uriSchemeStr;
    LPCSTR uriPathStr;
    LPCSTR appDataPathStr;
    char pathStr[MAX_PATH];

    app_data_paths_GetKnownFolder( NULL, "localappdata", &appDataPath );
    IUriRuntimeClass_get_SchemeName( uri, &uriScheme );
    IUriRuntimeClass_get_Domain( uri, &uriPath );

    uriSchemeStr = HStringToLPCSTR( uriScheme );
    uriPathStr = HStringToLPCSTR( uriPath );
    appDataPathStr = HStringToLPCSTR( appDataPath );
    
    //ms-appx: appx install path
    //ms-appdata: appx app data

    if ( !strcmp( uriSchemeStr, "ms-appx" ) )
    {
        GetModuleFileNameA(NULL, pathStr, MAX_PATH);
        PathAppendA( pathStr, uriPathStr );
    } else if ( !strcmp( uriSchemeStr, "ms-appdata" ) )
    {
        strcpy( pathStr, appDataPathStr );
        PathAppendA( pathStr, uriPathStr );
    } else {
        return E_INVALIDARG;
    }

    WindowsCreateString( CharToLPCWSTR( pathStr ), wcslen( CharToLPCWSTR( pathStr ) ), &path );

    hr = async_operation_storage_file_create( (IUnknown *)iface, (IUnknown *)path, storage_file_AssignFile, result );
    TRACE( "created IAsyncOperation_StorageFile %p.\n", *result );
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

static struct storage_file_statics storage_file_statics =
{
    {&factory_vtbl},
    {&storage_file_statics_vtbl},
    1,
};

IActivationFactory *storage_file_factory = &storage_file_statics.IActivationFactory_iface;
