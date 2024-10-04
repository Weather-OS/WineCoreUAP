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

#include "DownloadsFolderInternal.h"

HRESULT WINAPI downloads_folder_GetDownloadsFolder( HSTRING folderName, CreationCollisionOption creationOption, IStorageFolder *outFolder )
{
    HRESULT status = S_OK;
    HSTRING path;

    struct storage_folder *folder;
    
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder = impl_from_IStorageFolder( outFolder );

    status = known_folders_statics_GetKnownFolder( KnownFolderId_DownloadsFolder, &path );

    if ( FAILED( status ) )
        return status;

    status = storage_folder_AssignFolder( path, &folder->IStorageFolder_iface );

    return status;
}