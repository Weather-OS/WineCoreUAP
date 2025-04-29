/* WinRT Windows.Storage.DownloadsFolder Implementation
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
#include "DownloadsFolderInternal.h"

_ENABLE_DEBUGGING_

// Downloads Folder

static struct downloads_folder_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct downloads_folder_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct downloads_folder_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IDownloadsFolderStatics ))
    {
        *out = &impl->IDownloadsFolderStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IDownloadsFolderStatics2 ))
    {
        *out = &impl->IDownloadsFolderStatics2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct downloads_folder_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct downloads_folder_statics *impl = impl_from_IActivationFactory( iface );
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

DEFINE_IINSPECTABLE( downloads_folder_statics, IDownloadsFolderStatics, struct downloads_folder_statics, IActivationFactory_iface )

static HRESULT WINAPI downloads_folder_statics_CreateFileAsync( IDownloadsFolderStatics *iface, HSTRING desiredName, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFileAsync( downloadsFolder, desiredName, CreationCollisionOption_FailIfExists, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static HRESULT WINAPI downloads_folder_statics_CreateFolderAsync( IDownloadsFolderStatics *iface, HSTRING desiredName, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFolderAsync( downloadsFolder, desiredName, CreationCollisionOption_FailIfExists, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static HRESULT WINAPI downloads_folder_statics_CreateFileWithCollisionOptionAsync( IDownloadsFolderStatics *iface, HSTRING desiredName, CreationCollisionOption option, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    // The examined behavior disallows any modifications to existing data in the downloads folder.
    // Even though the client could query the folder using StorageFolder::GetFolderFromPathAsync() and bypass this,
    // The behavior is different when using IDownloadsFolderStatics.
    if ( option == CreationCollisionOption_ReplaceExisting || option == CreationCollisionOption_OpenIfExists )
    {
        hr = E_INVALIDARG;
        if ( FAILED( SetLastRestrictedErrorWithMessageW( hr, GetResourceW( IDS_COLLISIONNOTALLOWED ) ) ) )
            return E_UNEXPECTED;
        return hr;
    }

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFileAsync( downloadsFolder, desiredName, option, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static HRESULT WINAPI downloads_folder_statics_CreateFolderWithCollisionOptionAsync( IDownloadsFolderStatics *iface, HSTRING desiredName, CreationCollisionOption option, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    // The examined behavior disallows any modifications to existing data in the downloads folder.
    // Even though the client could query the folder using StorageFolder::GetFolderFromPathAsync() and bypass this,
    // The behavior is different when using IDownloadsFolderStatics.
    if ( option == CreationCollisionOption_ReplaceExisting || option == CreationCollisionOption_OpenIfExists )
    {
        hr = E_INVALIDARG;
        if ( FAILED( SetLastRestrictedErrorWithMessageW( hr, GetResourceW( IDS_COLLISIONNOTALLOWED ) ) ) )
            return E_UNEXPECTED;
        return hr;
    }

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFolderAsync( downloadsFolder, desiredName, option, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static const struct IDownloadsFolderStaticsVtbl downloads_folder_statics_vtbl =
{
    downloads_folder_statics_QueryInterface,
    downloads_folder_statics_AddRef,
    downloads_folder_statics_Release,
    /* IInspectable methods */
    downloads_folder_statics_GetIids,
    downloads_folder_statics_GetRuntimeClassName,
    downloads_folder_statics_GetTrustLevel,
    /* IDownloadsFolderStatics methods */
    downloads_folder_statics_CreateFileAsync,
    downloads_folder_statics_CreateFolderAsync,
    downloads_folder_statics_CreateFileWithCollisionOptionAsync,
    downloads_folder_statics_CreateFolderWithCollisionOptionAsync
};

DEFINE_IINSPECTABLE( downloads_folder_statics2, IDownloadsFolderStatics2, struct downloads_folder_statics, IActivationFactory_iface )


//Wine does not support User Profiles. Default methods used.
static HRESULT WINAPI downloads_folder_statics2_CreateFileForUserAsync( IDownloadsFolderStatics2 *iface, IUser *user, HSTRING desiredName, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFileAsync( downloadsFolder, desiredName, CreationCollisionOption_FailIfExists, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static HRESULT WINAPI downloads_folder_statics2_CreateFolderForUserAsync( IDownloadsFolderStatics2 *iface, IUser *user, HSTRING desiredName, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFolderAsync( downloadsFolder, desiredName, CreationCollisionOption_FailIfExists, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static HRESULT WINAPI downloads_folder_statics2_CreateFileForUserWithCollisionOptionAsync( IDownloadsFolderStatics2 *iface, IUser *user, HSTRING desiredName, CreationCollisionOption option, IAsyncOperation_StorageFile **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFileAsync( downloadsFolder, desiredName, option, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static HRESULT WINAPI downloads_folder_statics2_CreateFolderForUserWithCollisionOptionAsync( IDownloadsFolderStatics2 *iface, IUser *user, HSTRING desiredName, CreationCollisionOption option, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    IStorageFolder *downloadsFolder = NULL;

    TRACE( "iface %p, value %p\n", iface, operation );

    //Arguments
    if ( !desiredName || WindowsIsStringEmpty( desiredName ) ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = downloads_folder_GetDownloadsFolder( &downloadsFolder );
    if ( SUCCEEDED ( hr ) )
    {
        hr = IStorageFolder_CreateFolderAsync( downloadsFolder, desiredName, option, operation );
        TRACE( "created IAsyncOperation_StorageFile %p.\n", *operation );
    }

    IStorageFolder_Release( downloadsFolder );

    return hr;
}

static const struct IDownloadsFolderStatics2Vtbl downloads_folder_statics2_vtbl =
{
    downloads_folder_statics2_QueryInterface,
    downloads_folder_statics2_AddRef,
    downloads_folder_statics2_Release,
    /* IInspectable methods */
    downloads_folder_statics2_GetIids,
    downloads_folder_statics2_GetRuntimeClassName,
    downloads_folder_statics2_GetTrustLevel,
    /* IDownloadsFolderStatics methods */
    downloads_folder_statics2_CreateFileForUserAsync,
    downloads_folder_statics2_CreateFolderForUserAsync,
    downloads_folder_statics2_CreateFileForUserWithCollisionOptionAsync,
    downloads_folder_statics2_CreateFolderForUserWithCollisionOptionAsync
};

static struct downloads_folder_statics downloads_folder_statics =
{
    {&factory_vtbl},
    {&downloads_folder_statics_vtbl},
    {&downloads_folder_statics2_vtbl},
    1,
};

IActivationFactory *downloads_folder_factory = &downloads_folder_statics.IActivationFactory_iface;
