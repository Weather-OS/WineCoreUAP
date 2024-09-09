/* WinRT Windows.Storage.StorageFile Implementation
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

#ifndef STORAGE_FILE_INTERNAL_H
#define STORAGE_FILE_INTERNAL_H

#include "StorageFileInternal.h"
#include "StorageFolderInternal.h"
#include "StorageItemInternal.h"

#include "AppInternalPaths.h"

#include "util.h"
#include "../private.h"
#include "wine/debug.h"

#define BUFFER_SIZE 512

extern struct IStorageFileVtbl storage_file_vtbl;
extern struct IStorageItemVtbl storage_item_vtbl;

struct storage_file
{
    //Derivatives
    IStorageFile IStorageFile_iface;
    IStorageItem IStorageItem_iface;

    HSTRING FileType;
    HSTRING ContentType;
    LONG ref;
};


struct storage_file_statics
{
    IActivationFactory IActivationFactory_iface;
    IStorageFileStatics IStorageFileStatics_iface;

    LONG ref;
};

/**
 * Parametized structs
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

HRESULT WINAPI storage_file_AssignFile ( HSTRING filePath, IStorageFile * result );
HRESULT WINAPI storage_file_AssignFileAsync ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_Copy ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_CopyAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_Move ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI storage_file_MoveAndReplace ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif