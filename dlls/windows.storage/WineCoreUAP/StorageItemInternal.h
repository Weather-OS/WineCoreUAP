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

#ifndef STORAGE_ITEM_INTERNAL_H
#define STORAGE_ITEM_INTERNAL_H

#include "../private.h"
#include "wine/debug.h"

#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <combaseapi.h>
#include <shcore.h>

#include "StorageProviderInternal.h"
#include "StorageFolderInternal.h"
#include "FileProperties/BasicPropertiesInternal.h"

extern struct IStorageItemPropertiesVtbl storage_item_properties_vtbl;
extern struct IStorageItemPropertiesWithProviderVtbl storage_item_properties_with_provider_vtbl;
extern struct IStorageItemExtraPropertiesVtbl storage_item_extra_properties_vtbl;
extern struct IStorageProviderVtbl storage_provider_vtbl;
extern struct IStorageItemVtbl storage_item_vtbl;
extern struct IStorageItem2Vtbl storage_item2_vtbl;
extern struct IStorageFolderVtbl storage_folder_vtbl;
extern struct IBasicPropertiesVtbl basic_properties_vtbl;

// This volatile struct is ordered with padding in mind.
// Do not change the order of this struct.
struct storage_item
{
    IStorageItem IStorageItem_iface;
    IStorageItem2 IStorageItem2_iface;

    DateTime DateCreated;    
    HSTRING Name;    
    HSTRING Path;
    FileAttributes Attributes;
    LONG IStorageItemRef;
};

struct storage_item_properties
{    
    IStorageItemProperties IStorageItemProperties_iface;    
    IStorageItemPropertiesWithProvider IStorageItemPropertiesWithProvider_iface;
    IStorageItemContentProperties Properties;    

    HSTRING DisplayType;
    HSTRING DisplayName;
    HSTRING FolderRelativeId;

    LONG IStorageItemPropertiesRef;    
};

/**
 * Parametizred structs
 */

struct storage_item_rename_options
{
    NameCollisionOption option;
    HSTRING name;
};

struct storage_item *impl_from_IStorageItem( IStorageItem *iface );
struct storage_item_properties *impl_from_IStorageItemProperties( IStorageItemProperties *iface );
struct storage_item_properties *impl_from_IStorageItemPropertiesWithProvider( IStorageItemPropertiesWithProvider *iface );

HRESULT WINAPI storage_item_Internal_CreateNew( HSTRING itemPath, IStorageItem ** result );
HRESULT WINAPI storage_item_Rename( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_item_Delete( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_item_GetType( IStorageItem * iface, StorageItemTypes * type );
HRESULT WINAPI storage_item_GetProperties( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_item_properties_AssignProperties( IStorageItem* iface, IStorageItemProperties ** result );
HRESULT WINAPI storage_item_properties_with_provider_GetProvider( IStorageItemPropertiesWithProvider *iface, IStorageProvider **value );

#endif