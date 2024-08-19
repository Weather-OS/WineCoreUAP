/* WinRT IVectorView<Windows.Storage.IStorageItem *> Implementation
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

#include "../../private.h"
#include "wine/debug.h"

#ifndef STORAGE_FOLDER_VECTOR_VIEW_H
#define STORAGE_FOLDER_VECTOR_VIEW_H

struct storage_folder_vector_view
{
    IVectorView_StorageFolder IVectorView_StorageFolder_iface;
    struct vector_iids iids;
    LONG ref;

    UINT32 size;
    IStorageFolder *elements[1];
};

struct storage_folder_vector_view *impl_from_IVectorView_StorageFolder( IVectorView_StorageFolder *iface );

#endif