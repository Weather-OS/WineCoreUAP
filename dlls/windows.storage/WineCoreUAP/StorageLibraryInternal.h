/* WinRT Windows.Storage.StorageLibrary Implementation
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

#ifndef STORAGE_LIBRARY_INTERNAL_H
#define STORAGE_LIBRARY_INTERNAL_H

#include "../private.h"
#include "wine/debug.h"

struct storage_library_statics
{
    //Derivatives
    IActivationFactory IActivationFactory_iface;
    IStorageLibraryStatics IStorageLibraryStatics_iface;

    LONG ref;
};

struct definition_changed_handler
{
    ITypedEventHandler_StorageLibrary_IInspectable ITypedEventHandler_StorageLibrary_IInspectable_iface;
    EventRegistrationToken token;
};

struct storage_library
{
    //Derivatives
    IStorageLibrary IStorageLibrary_iface;
    HANDLE folderHandle;
    IObservableVector_StorageFolder *folderContents;
    IStorageFolder *folder;

    struct definition_changed_handler **definitionChangedHandlers;
    UINT32 handlerSize;
    UINT32 handlerCapacity;
};

struct storage_provider *impl_from_IStorageProvider( IStorageProvider *iface );

#endif