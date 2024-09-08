/* WinRT Windows.Storage.KnownFolders Implementation
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

#include "KnownFoldersInternal.h"

#include "../private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

// Storage File

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
    HRESULT hr;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    hr = known_folders_statics_GetKnownFolder( iface, KnownFolderId_MusicLibrary, &path );
    if ( SUCCEEDED( hr ) )
    {
        hr = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );
    }

    if ( SUCCEEDED( hr ) )
    {
        *value = &folder->IStorageFolder_iface;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_PicturesLibrary( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    HRESULT hr;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    hr = known_folders_statics_GetKnownFolder( iface, KnownFolderId_PicturesLibrary, &path );
    if ( SUCCEEDED( hr ) )
    {
        hr = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );
    }

    if ( SUCCEEDED( hr ) )
    {
        *value = &folder->IStorageFolder_iface;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_VideosLibrary( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    HRESULT hr;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    hr = known_folders_statics_GetKnownFolder( iface, KnownFolderId_VideosLibrary, &path );
    if ( SUCCEEDED( hr ) )
    {
        hr = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );
    }

    if ( SUCCEEDED( hr ) )
    {
        *value = &folder->IStorageFolder_iface;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_DocumentsLibrary( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    HRESULT hr;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    hr = known_folders_statics_GetKnownFolder( iface, KnownFolderId_DocumentsLibrary, &path );
    if ( SUCCEEDED( hr ) )
    {
        hr = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );
    }

    if ( SUCCEEDED( hr ) )
    {
        *value = &folder->IStorageFolder_iface;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_HomeGroup( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    HRESULT hr;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    hr = known_folders_statics_GetKnownFolder( iface, KnownFolderId_HomeGroup, &path );
    if ( SUCCEEDED( hr ) )
    {
        hr = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );
    }

    if ( SUCCEEDED( hr ) )
    {
        *value = &folder->IStorageFolder_iface;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_RemovableDevices( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    HRESULT hr;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    hr = known_folders_statics_GetKnownFolder( iface, KnownFolderId_RemovableDevices, &path );
    if ( SUCCEEDED( hr ) )
    {
        hr = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );
    }

    if ( SUCCEEDED( hr ) )
    {
        *value = &folder->IStorageFolder_iface;
    }

    return hr;
}

static HRESULT WINAPI known_folders_statics_get_MediaServerDevices( IKnownFoldersStatics *iface, IStorageFolder **value )
{
    HRESULT hr;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    hr = known_folders_statics_GetKnownFolder( iface, KnownFolderId_MediaServerDevices, &path );
    if ( SUCCEEDED( hr ) )
    {
        hr = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );
    }

    if ( SUCCEEDED( hr ) )
    {
        *value = &folder->IStorageFolder_iface;
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

static struct known_folders_statics known_folders_statics =
{
    {&factory_vtbl},
    {&known_folders_statics_vtbl},
    1,
};

IActivationFactory *known_folders_factory = &known_folders_statics.IActivationFactory_iface;
