/* WinRT Windows.Storage.DownloadsFolder Implementation
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

#ifndef DOWNLOADS_FOLDER_INTERNAL_H
#define DOWNLOADS_FOLDER_INTERNAL_H

#include "StorageFolderInternal.h"
#include "KnownFoldersInternal.h"

#include "../private.h"
#include "wine/debug.h"

struct downloads_folder_statics
{
    IActivationFactory IActivationFactory_iface;
    IDownloadsFolderStatics IDownloadsFolderStatics_iface;
    IDownloadsFolderStatics2 IDownloadsFolderStatics2_iface;

    LONG ref;
};

HRESULT WINAPI downloads_folder_CreateFile( HSTRING fileName, CreationCollisionOption creationOption, HSTRING *outPath );
HRESULT WINAPI downloads_folder_CreateFolder( HSTRING folderName, CreationCollisionOption creationOption, HSTRING *outPath );

#endif