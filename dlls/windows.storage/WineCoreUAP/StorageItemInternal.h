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

#ifndef STORAGE_ITEM_INTERNAL_H
#define STORAGE_ITEM_INTERNAL_H

struct storage_item
{
    //Derivatives
    IStorageItem IStorageItem_iface;

    FileAttributes Attributes;
    DateTime DateCreated;
    HSTRING Path;    
    HSTRING Name;
    LONG ref;
};

struct storage_item *impl_from_IStorageItem( IStorageItem *iface );

HRESULT WINAPI storage_item_Internal_CreateNew( HSTRING itemPath, IStorageItem * result );
HRESULT WINAPI storage_item_Rename( IStorageItem * iface, NameCollisionOption collisionOption, HSTRING name );
HRESULT WINAPI storage_item_Delete( IStorageItem * iface, StorageDeleteOption deleteOption );
HRESULT WINAPI storage_item_GetType( IStorageItem * iface, StorageItemTypes * type );

#endif