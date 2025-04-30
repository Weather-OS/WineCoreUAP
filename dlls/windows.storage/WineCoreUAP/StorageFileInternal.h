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

#ifndef STORAGE_FILE_INTERNAL_H
#define STORAGE_FILE_INTERNAL_H

#include "StorageFolderInternal.h"
#include "StorageItemInternal.h"

#include "../private.h"
#include "wine/debug.h"

#define BUFFER_SIZE 512

extern struct IStorageFileVtbl storage_file_vtbl;
extern struct IStorageItemVtbl storage_item_vtbl;
extern struct IStorageItem2Vtbl storage_item2_vtbl;
extern struct IStorageFilePropertiesWithAvailabilityVtbl storage_file_properties_with_availability_vtbl;

struct storage_file
{
    //IStorageFile Derivatives 
    IStorageFile IStorageFile_iface;    
        IStorageFilePropertiesWithAvailability IStorageFilePropertiesWithAvailability_iface;
        HSTRING FileType;
        HSTRING ContentType;

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

    IRandomAccessStreamReference *IRandomAccessStreamReference_iface;

    LONG ref;
};


struct storage_file_statics
{
    IActivationFactory IActivationFactory_iface;
    IStorageFileStatics IStorageFileStatics_iface;
    IStorageFileStatics2 IStorageFileStatics2_iface;

    LONG ref;
};

/**
 * Parametizred structs
 */

struct storage_file_move_options
{
    IStorageFolder *folder;
    NameCollisionOption option;
    HSTRING name;
};

struct storage_file_copy_options
{
    IStorageFolder *folder;
    NameCollisionOption option;
    HSTRING name;
};

struct storage_file *impl_from_IStorageFile( IStorageFile *iface );

HRESULT WINAPI storage_file_AssignFile ( HSTRING filePath, IStorageFile ** result );
HRESULT WINAPI storage_file_AssignFileAsync ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_Copy ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_CopyAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_Move ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_MoveAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_Open ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_properties_with_availability_IsAvailable ( IStorageItem *fileItem, boolean *value );

#endif