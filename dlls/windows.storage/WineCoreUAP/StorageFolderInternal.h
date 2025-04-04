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

#ifndef STORAGE_FOLDER_INTERNAL_H
#define STORAGE_FOLDER_INTERNAL_H

#include <shlwapi.h>

#include "StorageItemInternal.h"
#include "StorageFileInternal.h"

#include "../private.h"
#include "wine/debug.h"

extern struct IStorageFolderVtbl storage_folder_vtbl;
extern struct IStorageFolder2Vtbl storage_folder2_vtbl;
extern struct IStorageItemVtbl storage_item_vtbl;
extern struct IStorageItem2Vtbl storage_item2_vtbl;
extern struct IStorageFileVtbl storage_file_vtbl;
extern struct IStorageFilePropertiesWithAvailabilityVtbl storage_file_properties_with_availability_vtbl;
extern struct IStorageFolderQueryOperationsVtbl storage_folder_query_operations_vtbl;

struct storage_folder
{
    //IStorageFolder Derivatives
    IStorageFolder IStorageFolder_iface;
        IStorageFolder2 IStorageFolder2_iface;
        
    //IStorageItem Derivatives    
    IStorageItem IStorageItem_iface;
        IStorageItem2 IStorageItem2_iface;
        DateTime DateCreated;
        HSTRING Name;        
        HSTRING Path;
        FileAttributes Attributes;
        LONG IStorageItemRef;

    //IStorageItemProperties Derivatives
    IStorageItemProperties IStorageItemProperties_iface;
        IStorageItemPropertiesWithProvider IStorageItemPropertiesWithProvider_iface;
        IStorageItemContentPropertiesVtbl Properties;
        HSTRING DisplayType;
        HSTRING DisplayName;
        HSTRING FolderRelativeId;
        LONG IStorageItemPropertiesRef;

    //IStorageFolderQueryOperations Derivatives
    IStorageFolderQueryOperations IStorageFolderQueryOperations_iface;
        LONG IStorageFolderQueryOperationsRef;

    LONG ref;
};

struct storage_folder_statics
{
    IActivationFactory IActivationFactory_iface;
    IStorageFolderStatics IStorageFolderStatics_iface;
    IStorageFolderStatics2 IStorageFolderStatics2_iface;
    LONG ref;
};

/**
 * Parametizred structs
 */

struct storage_folder_creation_options
{
    CreationCollisionOption option;
    HSTRING name;
};

struct storage_folder *impl_from_IStorageFolder( IStorageFolder *iface );
struct storage_folder *impl_from_IStorageFolder2( IStorageFolder2 *iface );

HRESULT WINAPI storage_folder_AssignFolder ( HSTRING path, IStorageFolder **value );
HRESULT WINAPI storage_folder_AssignFolderAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchFile( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_CreateFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_CreateFile( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchItemsAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchFoldersAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchFilesAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder2_TryFetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif