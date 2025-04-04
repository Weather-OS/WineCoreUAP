/* WinRT Windows.Storage.KnownFolders Implementation
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

#ifndef KNOWN_FOLDERS_INTERNAL_H
#define KNOWN_FOLDERS_INTERNAL_H

#include "../private.h"

#include "StorageFolderInternal.h"

#include <appx.h>

#define MAX_BUFFER 1024

struct known_folders_statics
{
    IActivationFactory IActivationFactory_iface;
    IKnownFoldersStatics IKnownFoldersStatics_iface;
    IKnownFoldersStatics2 IKnownFoldersStatics2_iface;
    IKnownFoldersStatics3 IKnownFoldersStatics3_iface;
    IKnownFoldersStatics4 IKnownFoldersStatics4_iface;
    IKnownFoldersCameraRollStatics IKnownFoldersCameraRollStatics_iface;
    IKnownFoldersPlaylistsStatics IKnownFoldersPlaylistsStatics_iface;
    IKnownFoldersSavedPicturesStatics IKnownFoldersSavedPicturesStatics_iface;

    LONG ref;
};

HRESULT WINAPI known_folders_statics_GetKnownFolder( KnownFolderId folderId, IStorageFolder **value );
HRESULT WINAPI known_folders_statics_GetKnownFolderAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI known_folders_statics_RequestAccess( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif