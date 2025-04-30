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

//TODO: Permission handling is to be stored in Windows.Storage.AccessCache

#include "KnownFoldersInternal.h"

#include <initguid.h>
#include <knownfolders.h>

#include <shlobj.h>
#include <shlwapi.h>

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_folder_handler, IAsyncOperationCompletedHandler_StorageFolder, IAsyncOperation_StorageFolder )

HRESULT WINAPI known_folders_statics_GetKnownFolder( KnownFolderId folderId, IStorageFolder **value ) 
{    
    HRESULT status = S_OK;
    BOOLEAN musicLibraryAllowed = FALSE;
    BOOLEAN picturesLibraryAllowed = FALSE;
    BOOLEAN videosLibraryAllowed = FALSE;
    BOOLEAN documentsLibraryAllowed = FALSE;
    BOOLEAN homeGroupAllowed = FALSE;
    BOOLEAN removableDevicesAllowed = FALSE;
    BOOLEAN mediaServerAllowed = FALSE;    
    HSTRING path = NULL;
    PWSTR pathStr = NULL;
    DWORD asyncRes;
    WCHAR manifestPath[MAX_PATH];

    IStorageFolderStatics *storageFolderStatics = NULL;
    IAsyncOperation_StorageFolder *storageFolderOperation = NULL;

    struct appx_package package;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_StorageFolder, storageFolderStatics, IID_IStorageFolderStatics );
    
    TRACE( "folderid %d, value %p\n", folderId, value );

    pathStr = (PWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );

    GetModuleFileNameW(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecW(manifestPath);
    PathAppendW(manifestPath, L"AppxManifest.xml");

    if ( !PathFileExistsW( manifestPath ) )
    {
        IStorageFolderStatics_Release( storageFolderStatics );
        free( pathStr );
        return APPX_E_MISSING_REQUIRED_FILE;
    }

    if ( !OK( registerAppxPackage( manifestPath, &package ) ) )
    {
        IStorageFolderStatics_Release( storageFolderStatics );
        free( pathStr );
        return APPX_E_INVALID_MANIFEST;
    }

    for ( xmlNode *capabilityNode = package.Package.Capabilities; capabilityNode; capabilityNode = capabilityNode->next )
    {
        if ( capabilityNode->type == XML_ELEMENT_NODE
            && !xmlStrcmp( capabilityNode->name, (const xmlChar*)"Capability" ) )
        {
            if ( !xmlStrcmp( xmlGetProp( capabilityNode, (const xmlChar *)"Name" ), (const xmlChar*)"musicLibrary" ) )
                musicLibraryAllowed = TRUE;
            
            if ( !xmlStrcmp( xmlGetProp( capabilityNode, (const xmlChar *)"Name" ), (const xmlChar*)"picturesLibrary" ) )
                picturesLibraryAllowed = TRUE;

            if ( !xmlStrcmp( xmlGetProp( capabilityNode, (const xmlChar *)"Name" ), (const xmlChar*)"videosLibrary" ) )
                videosLibraryAllowed = TRUE;

            if ( !xmlStrcmp( xmlGetProp( capabilityNode, (const xmlChar *)"Name" ), (const xmlChar*)"documentsLibrary" ) )
                documentsLibraryAllowed = TRUE;

            if ( !xmlStrcmp( xmlGetProp( capabilityNode, (const xmlChar *)"Name" ), (const xmlChar*)"removableStorage" ) )
                removableDevicesAllowed = TRUE;

            if ( !xmlStrcmp( xmlGetProp( capabilityNode, (const xmlChar *)"Name" ), (const xmlChar*)"internetClient" ) )
                mediaServerAllowed = TRUE;
        }
    }


    if ( SUCCEEDED( status ) )
    {
        switch ( folderId )
        {
            case KnownFolderId_MusicLibrary:
                if ( !musicLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Music, 0, NULL, &pathStr);
                break;

            case KnownFolderId_PicturesLibrary:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Pictures, 0, NULL, &pathStr);
                break;
            
            case KnownFolderId_VideosLibrary:
                if ( !videosLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Videos, 0, NULL, &pathStr);
                break;

            case KnownFolderId_DocumentsLibrary:
                if ( !documentsLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Documents, 0, NULL, &pathStr);
                break;

            case KnownFolderId_RemovableDevices:
                if ( !removableDevicesAllowed )
                    status = E_ACCESSDENIED;
                else
                    wcscpy( pathStr, L"\\\\.\\" );
                break;

            case KnownFolderId_HomeGroup:
                if ( !homeGroupAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_HomeGroup, 0, NULL, &pathStr);
                break;

            case KnownFolderId_MediaServerDevices:
                if ( !mediaServerAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = E_NOTIMPL;
                break;

            case KnownFolderId_Objects3D:
                status = SHGetKnownFolderPath(&FOLDERID_Objects3D, 0, NULL, &pathStr);
                CreateDirectoryW( pathStr, NULL );
                break;

            case KnownFolderId_AppCaptures:
                if ( !videosLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_AppCaptures, 0, NULL, &pathStr);
                break;

            case KnownFolderId_RecordedCalls:
                status = E_NOTIMPL;
                break;

            case KnownFolderId_CameraRoll:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_CameraRoll, 0, NULL, &pathStr);
                break;

            case KnownFolderId_Playlists:
                if ( !musicLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Playlists, 0, NULL, &pathStr);
                break;

            case KnownFolderId_SavedPictures:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_SavedPictures, 0, NULL, &pathStr);
                break;

            case KnownFolderId_DownloadsFolder:
                status = SHGetKnownFolderPath(&FOLDERID_Downloads, 0, NULL, &pathStr);
                break;

            default:
                status = E_NOTIMPL;
        }
    }

    if ( SUCCEEDED( status ) )
    {
        WindowsCreateString( pathStr, wcslen ( pathStr ), &path );
        status = IStorageFolderStatics_GetFolderFromPathAsync( storageFolderStatics, path, &storageFolderOperation );
        if ( SUCCEEDED( status ) )
        {
            asyncRes = await_IAsyncOperation_StorageFolder( storageFolderOperation, INFINITE );
            if ( !asyncRes )
            {
                status = IAsyncOperation_StorageFolder_GetResults( storageFolderOperation, value );
                IStorageFolder_AddRef( *value );
            }
        }
    } else {
        if ( status == E_ACCESSDENIED )
        {
            status = SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_PATHNOTALLOWED ), pathStr );
            if ( FAILED( status ) ) 
                status = E_FAIL;
        }
    }

    if ( pathStr )
        free( pathStr );
    if ( path ) 
        WindowsDeleteString( path );
    IStorageFolderStatics_Release( storageFolderStatics );
    if ( storageFolderOperation )
        IAsyncOperation_StorageFolder_Release( storageFolderOperation );
    CHECK_LAST_RESTRICTED_ERROR();
    
    return status;
}

HRESULT WINAPI known_folders_statics_GetKnownFolderAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    //Must be called from an asynchronous context.
    IStorageFolder *folder = NULL;

    HRESULT status = S_OK;

    status = known_folders_statics_GetKnownFolder ( (KnownFolderId)param, &folder );

    if ( SUCCEEDED( status ) ) 
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)folder;
    }

    return status;
}

HRESULT WINAPI known_folders_statics_RequestAccess( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    IStorageFolder *knownFolder = NULL;

    HRESULT status = S_OK;    

    TRACE( "iface %p, value %p\n", invoker, result );

    status = known_folders_statics_GetKnownFolder ( (KnownFolderId)param, &knownFolder );
    if ( status == E_ACCESSDENIED )
    {
        result->vt = VT_UI4;
        result->ulVal = (ULONG)KnownFoldersAccessStatus_NotDeclaredByApp;
        return status;
    } else if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UI4;
        result->ulVal = (ULONG)KnownFoldersAccessStatus_Allowed;
        return status;
    } else {
        result->vt = VT_UI4;
        result->ulVal = (ULONG)KnownFoldersAccessStatus_DeniedBySystem;
        return status;
    }

    return status;
}