/* WinRT Windows.Storage.DownloadsFolder Implementation
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

#include "DownloadsFolderInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_folder_handler, IAsyncOperationCompletedHandler_StorageFolder, IAsyncOperation_StorageFolder )

HRESULT WINAPI downloads_folder_GetDownloadsFolder( IStorageFolder **outFolder )
{
    HRESULT status = S_OK;
    DWORD asyncRes;

    IAsyncOperation_StorageFolder *storageFolderOperation = NULL;
    IKnownFoldersStatics4 *knownFoldersStatics4 = NULL;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_KnownFolders, knownFoldersStatics4, IID_IKnownFoldersStatics4 );

    TRACE( "outFolder %p\n", outFolder );

    status = IKnownFoldersStatics4_GetFolderAsync( knownFoldersStatics4, KnownFolderId_DownloadsFolder, &storageFolderOperation );
    if ( FAILED( status ) ) goto _CLEANUP;

    asyncRes = await_IAsyncOperation_StorageFolder( storageFolderOperation, INFINITE );
    if ( asyncRes ) 
    {
        status = E_UNEXPECTED;
        goto _CLEANUP;
    }

    status = IAsyncOperation_StorageFolder_GetResults( storageFolderOperation, outFolder );

_CLEANUP:
    if ( knownFoldersStatics4 ) 
        IKnownFoldersStatics4_Release( knownFoldersStatics4 );
    if ( storageFolderOperation ) 
        IAsyncOperation_StorageFolder_Release( storageFolderOperation );
    if ( FAILED( status ) ) 
        if ( *outFolder )
            IStorageFolder_Release( *outFolder );
    CHECK_LAST_RESTRICTED_ERROR();
    return status;
}