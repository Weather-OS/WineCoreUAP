/* WinRT Windows.Storage.FileProperties.BasicProperties Implementation
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

#ifndef STORAGE_ITEM_EXTRA_PROPERTIES_INTERNAL_H
#define STORAGE_ITEM_EXTRA_PROPERTIES_INTERNAL_H

#include "../../private.h"
#include "wine/debug.h"

#include <propsys.h>
#include <shobjidl.h>

struct storage_item_extra_properties
{
    IStorageItemExtraProperties IStorageItemExtraProperties_iface;
    IMap_HSTRING_IInspectable *Properties;
    IStorageItem *Item; //Used for submitting saved properties
    LONG ref;
};

struct storage_item_extra_properties *impl_from_IStorageItemExtraProperties( IStorageItemExtraProperties *iface );
HRESULT WINAPI storage_item_extra_properties_FetchPropertiesAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_item_extra_properties_SubmitPropertiesAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif