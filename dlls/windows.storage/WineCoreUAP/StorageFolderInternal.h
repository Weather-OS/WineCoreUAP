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

#include "../private.h"

#ifndef STORAGE_FOLDER_INTERNAL_H
#define STORAGE_FOLDER_INTERNAL_H

extern struct IStorageFolderVtbl storage_folder_vtbl;
extern struct IStorageItemVtbl storage_item_vtbl;
extern struct IVectorView_IStorageItemVtbl storage_item_vector_view_vtbl;

struct storage_folder
{
    //Derivatives
    IStorageFolder IStorageFolder_iface;
    IStorageItem IStorageItem_iface;

    LONG ref;
};


struct storage_folder_statics
{
    IActivationFactory IActivationFactory_iface;
    IStorageFolderStatics IStorageFolderStatics_iface;
    LONG ref;
};

struct storage_folder_statics *impl_from_IActivationFactory( IActivationFactory *iface );
struct storage_folder *impl_from_IStorageFolder( IStorageFolder *iface );

HRESULT WINAPI storage_folder_AssignFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_FetchFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_folder_CreateFolder( IStorageFolder* folder, CreationCollisionOption collisionOption, HSTRING Name, HSTRING *OutPath );
HRESULT WINAPI storage_folder_FetchItemsAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif