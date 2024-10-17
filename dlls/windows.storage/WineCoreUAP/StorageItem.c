/* WinRT Windows.Storage.StorageItem Implementation
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

#include "StorageItemInternal.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

// Storage Item

struct storage_item *impl_from_IStorageItem( IStorageItem *iface )
{
    return CONTAINING_RECORD( iface, struct storage_item, IStorageItem_iface );
}

static HRESULT WINAPI storage_item_QueryInterface( IStorageItem *iface, REFIID iid, void **out )
{
    struct storage_item *impl = impl_from_IStorageItem( iface );

    // Inheritence 
    struct storage_folder *inheritedFolder = CONTAINING_RECORD( &impl->IStorageItem_iface, struct storage_folder, IStorageItem_iface );
    struct storage_file *inheritedFile = CONTAINING_RECORD( &impl->IStorageItem_iface, struct storage_file, IStorageItem_iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageItem ))
    {
        *out = &impl->IStorageItem_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if ( inheritedFile->IStorageFile_iface.lpVtbl == &storage_file_vtbl )
    {
        return IStorageFile_QueryInterface( &inheritedFile->IStorageFile_iface, iid, out );
    }

    if ( inheritedFolder->IStorageFolder_iface.lpVtbl == &storage_folder_vtbl )
    {
        return IStorageFolder_QueryInterface( &inheritedFolder->IStorageFolder_iface, iid, out );
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_item_AddRef( IStorageItem *iface )
{
    struct storage_item *impl = impl_from_IStorageItem( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_item_Release( IStorageItem *iface )
{
    struct storage_item *impl = impl_from_IStorageItem( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_item_GetIids( IStorageItem *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_GetRuntimeClassName( IStorageItem *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_GetTrustLevel( IStorageItem *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::StorageItem
*/

static HRESULT WINAPI storage_item_RenameAsyncOverloadDefaultOptions( IStorageItem *iface, HSTRING name, IAsyncAction **operation )
{
    HRESULT hr;
    struct storage_item_rename_options *rename_options;

    if (!(rename_options = calloc( 1, sizeof(*rename_options) ))) return E_OUTOFMEMORY;
    rename_options->name = name;
    rename_options->option = NameCollisionOption_FailIfExists;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)rename_options, storage_item_Rename, operation );
    return hr;
}

static HRESULT WINAPI storage_item_RenameAsync( IStorageItem *iface, HSTRING name, NameCollisionOption option ,IAsyncAction **operation )
{
    HRESULT hr;
    struct storage_item_rename_options *rename_options;
    
    if (!(rename_options = calloc( 1, sizeof(*rename_options) ))) return E_OUTOFMEMORY;
    rename_options->name = name;
    rename_options->option = option;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)rename_options, storage_item_Rename, operation );
    return hr;
}

static HRESULT WINAPI storage_item_DeleteAsyncOverloadDefaultOptions( IStorageItem *iface, IAsyncAction **operation )
{
    HRESULT hr;
    hr = async_action_create( (IUnknown *)iface, (IUnknown *)StorageDeleteOption_Default, storage_item_Delete, operation );
    return hr;
}

static HRESULT WINAPI storage_item_DeleteAsync( IStorageItem *iface, StorageDeleteOption option, IAsyncAction **operation )
{
    HRESULT hr;
    hr = async_action_create( (IUnknown *)iface, (IUnknown *)option, storage_item_Delete, operation );
    return hr;
}

static HRESULT WINAPI storage_item_GetBasicPropertiesAsync( IStorageItem *iface, IAsyncOperation_BasicProperties **operation )
{
    HRESULT hr;
    hr = async_operation_basic_properties_create( (IUnknown *)iface, (IUnknown *)NULL, storage_item_GetProperties, operation );
    TRACE( "created IAsyncOperation_BasicProperties %p.\n", *operation );
    return hr;
}

static HRESULT WINAPI storage_item_get_Name( IStorageItem *iface, HSTRING *value )
{
    struct storage_item *impl = impl_from_IStorageItem( iface );
    WindowsDuplicateString( impl->Name, value );
    return S_OK;
}

static HRESULT WINAPI storage_item_get_Path( IStorageItem *iface, HSTRING *value )
{
    struct storage_item *impl = impl_from_IStorageItem( iface );
    WindowsDuplicateString( impl->Path, value );
    return S_OK;
}

static HRESULT WINAPI storage_item_get_Attributes( IStorageItem *iface, FileAttributes *value )
{
    struct storage_item *impl = impl_from_IStorageItem( iface );
    *value = impl->Attributes;
    return S_OK;
}

static HRESULT WINAPI storage_item_get_DateCreated( IStorageItem *iface, DateTime *value )
{
    struct storage_item *impl = impl_from_IStorageItem( iface );
    *value = impl->DateCreated;
    return S_OK;
}

static HRESULT WINAPI storage_item_IsOfType( IStorageItem *iface, StorageItemTypes type, BOOLEAN *value )
{    
    HRESULT hr;
    StorageItemTypes fileType;
    hr = storage_item_GetType( iface, &fileType );
    if ( SUCCEEDED( hr ) )
    {
        if ( type == fileType )
            *value = TRUE;
        else
            *value = FALSE;
    }
    return hr;
}

struct IStorageItemVtbl storage_item_vtbl =
{
    storage_item_QueryInterface,
    storage_item_AddRef,
    storage_item_Release,
    /* IInspectable methods */
    storage_item_GetIids,
    storage_item_GetRuntimeClassName,
    storage_item_GetTrustLevel,
    /* IStorageItem methods */
    storage_item_RenameAsyncOverloadDefaultOptions,
    storage_item_RenameAsync,
    storage_item_DeleteAsyncOverloadDefaultOptions,
    storage_item_DeleteAsync,
    storage_item_GetBasicPropertiesAsync,
    storage_item_get_Name,
    storage_item_get_Path,
    storage_item_get_Attributes,
    storage_item_get_DateCreated,
    storage_item_IsOfType
};

struct storage_item_properties *impl_from_IStorageItemProperties( IStorageItemProperties *iface )
{
    return CONTAINING_RECORD( iface, struct storage_item_properties, IStorageItemProperties_iface );
}

static HRESULT WINAPI storage_item_properties_QueryInterface( IStorageItemProperties *iface, REFIID iid, void **out )
{
    struct storage_item_properties *impl = impl_from_IStorageItemProperties( iface );

    // Inheritence 
    struct storage_item *inheritedItem = CONTAINING_RECORD( &impl->IStorageItemProperties_iface, struct storage_item, IStorageItemProperties_iface );
    
    if ( inheritedItem->IStorageItem_iface.lpVtbl != &storage_item_vtbl )
    {
        printf("IStorageItemProperties is lone in IStorageItem inheritence! Please check for any memory leaks.\n");
        return E_UNEXPECTED;
    }

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageItemProperties ))
    {
        *out = &impl->IStorageItemProperties_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    return IStorageItem_QueryInterface( &inheritedItem->IStorageItem_iface, iid, out );
}

static ULONG WINAPI storage_item_properties_AddRef( IStorageItemProperties *iface )
{
    struct storage_item_properties *impl = impl_from_IStorageItemProperties( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_item_properties_Release( IStorageItemProperties *iface )
{
    struct storage_item_properties *impl = impl_from_IStorageItemProperties( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_item_properties_GetIids( IStorageItemProperties *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_properties_GetRuntimeClassName( IStorageItemProperties *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_properties_GetTrustLevel( IStorageItemProperties *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::StorageItemProperties
*/

static HRESULT WINAPI storage_item_properties_get_DisplayName( IStorageItemProperties *iface, HSTRING *value )
{
    struct storage_item_properties *impl = impl_from_IStorageItemProperties( iface );
    WindowsDuplicateString( impl->DisplayName, value );
    return S_OK;    
}

static HRESULT WINAPI storage_item_properties_get_DisplayType( IStorageItemProperties *iface, HSTRING *value )
{
    struct storage_item_properties *impl = impl_from_IStorageItemProperties( iface );
    WindowsDuplicateString( impl->DisplayType, value );
    return S_OK;    
}

static HRESULT WINAPI storage_item_properties_get_FolderRelativeId( IStorageItemProperties *iface, HSTRING *value )
{
    struct storage_item_properties *impl = impl_from_IStorageItemProperties( iface );
    WindowsDuplicateString( impl->FolderRelativeId, value );
    return S_OK;    
}

static HRESULT WINAPI storage_item_properties_get_Properties( IStorageItemProperties *iface, IStorageItemContentProperties **value )
{
    struct storage_item_properties *impl = impl_from_IStorageItemProperties( iface );
    *value = &impl->Properties;
    return S_OK;    
}

struct IStorageItemPropertiesVtbl storage_item_properties_vtbl =
{
    storage_item_properties_QueryInterface,
    storage_item_properties_AddRef,
    storage_item_properties_Release,
    /* IInspectable methods */
    storage_item_properties_GetIids,
    storage_item_properties_GetRuntimeClassName,
    storage_item_properties_GetTrustLevel,
    /* IStorageItemProperties methods */
    storage_item_properties_get_DisplayName,
    storage_item_properties_get_DisplayType,
    storage_item_properties_get_FolderRelativeId,
    storage_item_properties_get_Properties
};