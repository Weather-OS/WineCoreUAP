/* WinRT Windows.Storage.StorageFolder Implementation
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

#ifndef STORAGE_ITEM_INTERNAL_H
#define STORAGE_ITEM_INTERNAL_H

#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdbool.h>
#include <stdio.h>
#include <combaseapi.h>

#include "StorageFolderInternal.h"
#include "FileProperties/BasicPropertiesInternal.h"

#include "util.h"
#include "../private.h"
#include "wine/debug.h"

extern struct IStorageItemPropertiesVtbl storage_item_properties_vtbl;

struct storage_item
{
    IStorageItem IStorageItem_iface;

    IStorageItemProperties IStorageItemProperties_iface; //This sort of counts as padding as well.
    HSTRING Path;
    HSTRING Name;    
    FileAttributes Attributes;
    DateTime DateCreated;

    LONG ref;
};

struct storage_item_properties
{
    IStorageItemProperties IStorageItemProperties_iface;
    IStorageItemContentProperties Properties;

    HSTRING DisplayName;
    HSTRING DisplayType;
    HSTRING FolderRelativeId;

    LONG ref;
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

HRESULT WINAPI storage_item_Internal_CreateNew( HSTRING itemPath, IStorageItem * result );
HRESULT WINAPI storage_item_Rename( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_item_Delete( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_item_GetType( IStorageItem * iface, StorageItemTypes * type );
HRESULT WINAPI storage_item_GetProperties( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif