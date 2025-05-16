/* WinRT Windows.Storage.ApplicationData Implementation
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

#ifndef APPLICATION_DATA_CONTAINER_INTERNAL_H
#define APPLICATION_DATA_CONTAINER_INTERNAL_H

#include "../private.h"
#include "wine/debug.h"
#include "shlwapi.h"
#include "propidl.h"
#include "propvarutil.h"

#include "ApplicationDataInternal.h"

#define MAX_VALUE_SIZE (1024 * 1024)
#define MAX_VALUE_NAME (128 * 128)

extern const struct IApplicationDataContainerVtbl application_data_container_vtbl;

/**
 * Developer note: you may be wondering why we don't automate registry operations by
 * assigning every container with it's own key. The reason is that we can't trust the client
 * to release the container object.
 */

struct application_data_container
{
    //Derivatives
    IApplicationDataContainer IApplicationDataContainer_iface;
    HSTRING Name;
    ApplicationDataLocality Locality; //Client handles everything regarding Locales. We don't need to do anything.
    IPropertySet *Values;
    IMap_HSTRING_ApplicationDataContainer *Containers;

    WCHAR hivePath[MAX_PATH];
    WCHAR containerPath[MAX_PATH];
    HSTRING appDataPath;
    LONG ref;
};

struct event_handler_data
{
    WCHAR hivePath[MAX_PATH];
    WCHAR containerPath[MAX_PATH];
};

struct application_data_container *impl_from_IApplicationDataContainer( IApplicationDataContainer *iface );

HRESULT WINAPI application_data_AssignAndTrackContainer( HSTRING appDataPath, LPCWSTR containerPath, IApplicationDataContainer **container );
HRESULT WINAPI application_data_container_CreateAndTrackContainer( IApplicationDataContainer *iface, HSTRING name, ApplicationDataCreateDisposition composition, IApplicationDataContainer **outContainer );

#endif