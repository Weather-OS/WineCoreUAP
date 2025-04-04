/* WinRT Windows.Storage.KnownFolders Implementation
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

#include "KnownFoldersInternal.h"

_ENABLE_DEBUGGING_

// Known Folders

static struct known_folders_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct known_folders_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct known_folders_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IKnownFoldersStatics ))
    {
        *out = &impl->IKnownFoldersStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IKnownFoldersStatics2 ))
    {
        *out = &impl->IKnownFoldersStatics2_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IKnownFoldersStatics3 ))
    {
        *out = &impl->IKnownFoldersStatics3_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IKnownFoldersStatics4 ))
    {
        *out = &impl->IKnownFoldersStatics4_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IKnownFoldersCameraRollStatics ))
    {
        *out = &impl->IKnownFoldersCameraRollStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IKnownFoldersPlaylistsStatics ))
    {
        *out = &impl->IKnownFoldersPlaylistsStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IKnownFoldersSavedPicturesStatics ))
    {
        *out = &impl->IKnownFoldersSavedPicturesStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct known_folders_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct known_folders_statics *impl = impl_from_IActivationFactory( iface );
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

DEFINE_IINSPECTABLE( known_folders_statics, IKnownFoldersStatics, struct known_folders_statics, IActivationFactory_iface )

static HRESULT WINAPI known_folders_statics_get_MusicLibrary( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_MusicLibrary, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_PicturesLibrary( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_PicturesLibrary, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_VideosLibrary( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_VideosLibrary, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_DocumentsLibrary( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_DocumentsLibrary, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_HomeGroup( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_HomeGroup, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_RemovableDevices( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_RemovableDevices, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_MediaServerDevices( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_MediaServerDevices, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static const struct IKnownFoldersStaticsVtbl known_folders_statics_vtbl =
{
    known_folders_statics_QueryInterface,
    known_folders_statics_AddRef,
    known_folders_statics_Release,
    /* IInspectable methods */
    known_folders_statics_GetIids,
    known_folders_statics_GetRuntimeClassName,
    known_folders_statics_GetTrustLevel,
    /* IKnownFoldersStatics methods */
    known_folders_statics_get_MusicLibrary,
    known_folders_statics_get_PicturesLibrary,
    known_folders_statics_get_VideosLibrary,
    known_folders_statics_get_DocumentsLibrary,
    known_folders_statics_get_HomeGroup,
    known_folders_statics_get_RemovableDevices,
    known_folders_statics_get_MediaServerDevices
};

DEFINE_IINSPECTABLE( known_folders_statics2, IKnownFoldersStatics2, struct known_folders_statics, IActivationFactory_iface )

static HRESULT WINAPI known_folders_statics2_get_Objects3D( IKnownFoldersStatics2 *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_Objects3D, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics2_get_AppCaptures( IKnownFoldersStatics2 *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_AppCaptures, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics2_get_RecordedCalls( IKnownFoldersStatics2 *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_RecordedCalls, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static const struct IKnownFoldersStatics2Vtbl known_folders_statics2_vtbl =
{
    known_folders_statics2_QueryInterface,
    known_folders_statics2_AddRef,
    known_folders_statics2_Release,
    /* IInspectable methods */
    known_folders_statics2_GetIids,
    known_folders_statics2_GetRuntimeClassName,
    known_folders_statics2_GetTrustLevel,
    /* IKnownFoldersStatics2 methods */
    known_folders_statics2_get_Objects3D,
    known_folders_statics2_get_AppCaptures,
    known_folders_statics2_get_RecordedCalls
};

DEFINE_IINSPECTABLE( known_folders_statics3, IKnownFoldersStatics3, struct known_folders_statics, IActivationFactory_iface )

static HRESULT WINAPI known_folders_statics3_GetFolderForUserAsync( IKnownFoldersStatics3 *iface, IUser *user, KnownFolderId folder_id, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFolder };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)folder_id, known_folders_statics_GetKnownFolderAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFolder %p.\n", *operation );

    return hr;
}

static const struct IKnownFoldersStatics3Vtbl known_folders_statics3_vtbl =
{
    known_folders_statics3_QueryInterface,
    known_folders_statics3_AddRef,
    known_folders_statics3_Release,
    /* IInspectable methods */
    known_folders_statics3_GetIids,
    known_folders_statics3_GetRuntimeClassName,
    known_folders_statics3_GetTrustLevel,
    /* IKnownFoldersStatics3 methods */
    known_folders_statics3_GetFolderForUserAsync
};

DEFINE_IINSPECTABLE( known_folders_statics4, IKnownFoldersStatics4, struct known_folders_statics, IActivationFactory_iface )

static HRESULT WINAPI known_folders_statics4_RequestAccessAsync( IKnownFoldersStatics4 *iface, KnownFolderId folder_id, IAsyncOperation_KnownFoldersAccessStatus **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_KnownFoldersAccessStatus };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_uint32_create( (IUnknown *)iface, (IUnknown *)folder_id, known_folders_statics_RequestAccess, iids, (IAsyncOperation_UINT32 **)operation );
    TRACE( "created IAsyncOperation_KnownFoldersAccessStatus %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI known_folders_statics4_RequestAccessForUserAsync( IKnownFoldersStatics4 *iface, IUser *user, KnownFolderId folder_id, IAsyncOperation_KnownFoldersAccessStatus **operation )
{
    //User is not used. 
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_KnownFoldersAccessStatus };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_uint32_create( (IUnknown *)iface, (IUnknown *)folder_id, known_folders_statics_RequestAccess, iids, (IAsyncOperation_UINT32 **)operation );
    TRACE( "created IAsyncOperation_KnownFoldersAccessStatus %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI known_folders_statics4_GetFolderAsync( IKnownFoldersStatics4 *iface, KnownFolderId folder_id, IAsyncOperation_StorageFolder **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_StorageFolder };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)folder_id, known_folders_statics_GetKnownFolderAsync, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_StorageFolder %p.\n", *operation );

    return hr;
}

static const struct IKnownFoldersStatics4Vtbl known_folders_statics4_vtbl =
{
    known_folders_statics4_QueryInterface,
    known_folders_statics4_AddRef,
    known_folders_statics4_Release,
    /* IInspectable methods */
    known_folders_statics4_GetIids,
    known_folders_statics4_GetRuntimeClassName,
    known_folders_statics4_GetTrustLevel,
    /* IKnownFoldersStatics4 methods */
    known_folders_statics4_RequestAccessAsync,
    known_folders_statics4_RequestAccessForUserAsync,
    known_folders_statics4_GetFolderAsync
};

DEFINE_IINSPECTABLE( known_folders_camera_roll_statics, IKnownFoldersCameraRollStatics, struct known_folders_statics, IActivationFactory_iface )

static HRESULT WINAPI known_folders_camera_roll_statics_get_CameraRoll( IKnownFoldersCameraRollStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_CameraRoll, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static const struct IKnownFoldersCameraRollStaticsVtbl known_folders_camera_roll_statics_vtbl =
{
    known_folders_camera_roll_statics_QueryInterface,
    known_folders_camera_roll_statics_AddRef,
    known_folders_camera_roll_statics_Release,
    /* IInspectable methods */
    known_folders_camera_roll_statics_GetIids,
    known_folders_camera_roll_statics_GetRuntimeClassName,
    known_folders_camera_roll_statics_GetTrustLevel,
    /* IKnownFoldersCameraRollStatics methods */
    known_folders_camera_roll_statics_get_CameraRoll
};

DEFINE_IINSPECTABLE( known_folders_playlists_statics, IKnownFoldersPlaylistsStatics, struct known_folders_statics, IActivationFactory_iface )

static HRESULT WINAPI known_folders_playlists_statics_get_Playlists( IKnownFoldersPlaylistsStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_Playlists, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static const struct IKnownFoldersPlaylistsStaticsVtbl known_folders_playlists_statics_vtbl =
{
    known_folders_playlists_statics_QueryInterface,
    known_folders_playlists_statics_AddRef,
    known_folders_playlists_statics_Release,
    /* IInspectable methods */
    known_folders_playlists_statics_GetIids,
    known_folders_playlists_statics_GetRuntimeClassName,
    known_folders_playlists_statics_GetTrustLevel,
    /* IKnownFoldersPlaylistsStatics methods */
    known_folders_playlists_statics_get_Playlists
};

DEFINE_IINSPECTABLE( known_folders_saved_pictures_statics, IKnownFoldersSavedPicturesStatics, struct known_folders_statics, IActivationFactory_iface )

static HRESULT WINAPI known_folders_saved_pictures_statics_get_SavedPictures( IKnownFoldersSavedPicturesStatics *iface, IStorageFolder **value )
{
    IStorageFolder *folder = NULL;

    HRESULT hr;

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;
    
    hr = known_folders_statics_GetKnownFolder( KnownFolderId_SavedPictures, &folder );

    if ( SUCCEEDED( hr ) )
    {
        *value = folder;
    }

    return hr;
}

static const struct IKnownFoldersSavedPicturesStaticsVtbl known_folders_saved_pictures_statics_vtbl =
{
    known_folders_saved_pictures_statics_QueryInterface,
    known_folders_saved_pictures_statics_AddRef,
    known_folders_saved_pictures_statics_Release,
    /* IInspectable methods */
    known_folders_saved_pictures_statics_GetIids,
    known_folders_saved_pictures_statics_GetRuntimeClassName,
    known_folders_saved_pictures_statics_GetTrustLevel,
    /* IKnownFoldersSavedPicturesStatics methods */
    known_folders_saved_pictures_statics_get_SavedPictures
};

static struct known_folders_statics known_folders_statics =
{
    {&factory_vtbl},
    {&known_folders_statics_vtbl},
    {&known_folders_statics2_vtbl},
    {&known_folders_statics3_vtbl},
    {&known_folders_statics4_vtbl},
    {&known_folders_camera_roll_statics_vtbl},
    {&known_folders_playlists_statics_vtbl},
    {&known_folders_saved_pictures_statics_vtbl},
    1,
};

IActivationFactory *known_folders_factory = &known_folders_statics.IActivationFactory_iface;
