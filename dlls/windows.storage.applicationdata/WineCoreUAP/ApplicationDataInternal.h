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

#ifndef APPLICATION_DATA_INTERNAL_H
#define APPLICATION_DATA_INTERNAL_H

#include <appx.h>

#include "../private.h"
#include "wine/debug.h"
#include "shlwapi.h"

#include "SetVersionInternal.h"

#define SETTINGS_PATH L"Settings"
#define SETTINGS_DATA_PATH L"settings.dat"
#define ROAMING_DATA_PATH L"roaming.lock"
#define rootKeyName L"ApplicationData"

extern const struct ISetVersionRequestVtbl set_version_vtbl;
extern const struct ISetVersionDeferralVtbl set_version_deferral_vtbl;
extern const struct IApplicationDataVtbl application_data_vtbl;

/**
 * WINE DOES NOT SUPPORT REGISTRY HIVES!!!
 * Attempting to make settings.dat confromant would 
 * require adding regedit hive support to wine.
 * 
 * This implementation is custom.
*/

struct application_data_statics
{
    IActivationFactory IActivationFactory_iface;
    IApplicationDataStatics IApplicationDataStatics_iface;
    LONG ref;
};

struct application_data
{
    IApplicationData IApplicationData_iface;
    HSTRING appDataPath;
    IApplicationDataContainer *LocalSettings;
    IApplicationDataContainer *RoamingSettings;
    UINT32 Version;
    LONG ref;
};

struct set_version_options
{
    IApplicationDataSetVersionHandler *handler;
    ISetVersionRequest request;
};

struct application_data *impl_from_IApplicationData( IApplicationData *iface );

HRESULT WINAPI application_data_Init( IApplicationData **value );
HRESULT WINAPI application_data_SetVersion( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif