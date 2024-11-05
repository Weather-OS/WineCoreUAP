/* WinRT Windows.Storage.KnownFolders Implementation
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

//TODO: Permission handling is to be stored in Windows.Storage.AccessCache

#include "KnownFoldersInternal.h"

#include <initguid.h>
#include <knownfolders.h>

#include <shlobj.h>
#include <shlwapi.h>

_ENABLE_DEBUGGING_

HRESULT WINAPI known_folders_statics_GetKnownFolder( KnownFolderId folderId, HSTRING *value ) 
{    
    HRESULT status = S_OK;
    BOOLEAN musicLibraryAllowed = FALSE;
    BOOLEAN picturesLibraryAllowed = FALSE;
    BOOLEAN videosLibraryAllowed = FALSE;
    BOOLEAN documentsLibraryAllowed = FALSE;
    BOOLEAN homeGroupAllowed = TRUE;
    BOOLEAN removableDevicesAllowed = FALSE;
    BOOLEAN mediaServerAllowed = FALSE;
    PWSTR path;
    WCHAR manifestPath[MAX_PATH];

    struct appx_package package;

    TRACE( "folderid %d, value %p\n", folderId, value );

    path = (PWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );

    GetModuleFileNameW(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecW(manifestPath);
    PathAppendW(manifestPath, L"AppxManifest.xml");

    if ( !OK( registerAppxPackage( manifestPath, &package ) ) )
    {
        status = E_UNEXPECTED;
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
                    status = SHGetKnownFolderPath(&FOLDERID_Music, 0, NULL, &path);
                break;

            case KnownFolderId_PicturesLibrary:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Pictures, 0, NULL, &path);
                break;
            
            case KnownFolderId_VideosLibrary:
                if ( !videosLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Videos, 0, NULL, &path);
                break;

            case KnownFolderId_DocumentsLibrary:
                if ( !documentsLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Documents, 0, NULL, &path);
                break;

            case KnownFolderId_RemovableDevices:
                if ( !removableDevicesAllowed )
                    status = E_ACCESSDENIED;
                else
                    wcscpy( path, L"\\\\.\\" );
                break;

            case KnownFolderId_HomeGroup:
                if ( !homeGroupAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_HomeGroup, 0, NULL, &path);
                break;

            case KnownFolderId_MediaServerDevices:
                if ( !mediaServerAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = E_NOTIMPL;
                break;

            case KnownFolderId_Objects3D:
                status = SHGetKnownFolderPath(&FOLDERID_Objects3D, 0, NULL, &path);
                CreateDirectoryW( path, NULL );
                break;

            case KnownFolderId_AppCaptures:
                if ( !videosLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_AppCaptures, 0, NULL, &path);
                break;

            case KnownFolderId_RecordedCalls:
                status = E_NOTIMPL;
                break;

            case KnownFolderId_CameraRoll:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_CameraRoll, 0, NULL, &path);
                break;

            case KnownFolderId_Playlists:
                if ( !musicLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_Playlists, 0, NULL, &path);
                break;

            case KnownFolderId_SavedPictures:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = SHGetKnownFolderPath(&FOLDERID_SavedPictures, 0, NULL, &path);
                break;

            case KnownFolderId_DownloadsFolder:
                status = SHGetKnownFolderPath(&FOLDERID_Downloads, 0, NULL, &path);
                break;

            default:
                status = E_NOTIMPL;
        }
    }

    if ( SUCCEEDED( status ) )
    {
        status = WindowsCreateString( path, wcslen( path ), value );
    }

    return status;
}

HRESULT WINAPI known_folders_statics_RequestAccess( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING KnownFolderPath;
    INT promptResult;
    WCHAR title[MAX_BUFFER];
    WCHAR message[MAX_BUFFER];
    WCHAR manifestPath[MAX_PATH];
    DWORD attributes;

    struct appx_package package;

    TRACE( "iface %p, value %p\n", invoker, result );

    GetModuleFileNameW(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecW(manifestPath);
    PathAppendW(manifestPath, L"AppxManifest.xml");

    if ( !OK( registerAppxPackage( manifestPath, &package ) ) )
    {
        status = E_UNEXPECTED;
    }

    status = known_folders_statics_GetKnownFolder ( (KnownFolderId)param, &KnownFolderPath );
    if ( status == E_ACCESSDENIED )
    {
        result->vt = VT_UI4;
        result->ulVal = (ULONG)KnownFoldersAccessStatus_NotDeclaredByApp;
        return status;
    }

    attributes = GetFileAttributesW( WindowsGetStringRawBuffer( KnownFolderPath, NULL ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES )
    {
        if ( GetLastError() == ERROR_ACCESS_DENIED )
        {
            result->vt = VT_UI4;
            result->ulVal = (ULONG)KnownFoldersAccessStatus_DeniedBySystem;
        }
    }

    if ( SUCCEEDED( status ) )
    {
        if ( package.Package.Properties.DisplayName )
        {
            swprintf( title, MAX_BUFFER, L"Let %s access the following file location?", package.Package.Properties.DisplayName );
        } else
        {
            swprintf( title, MAX_BUFFER, L"Let this app access the following file location?" );
        }

        swprintf( message, MAX_BUFFER, L"%s\n%s", title, WindowsGetStringRawBuffer( KnownFolderPath, NULL ) );
        
        promptResult = MessageBoxW (
            NULL,
            message,
            title,
            MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2
        );

        result->vt = VT_UI4;

        switch ( promptResult )
        {
            case IDYES:
                result->ulVal = (ULONG)KnownFoldersAccessStatus_Allowed;
                break;
                
            case IDNO:
                result->ulVal = (ULONG)KnownFoldersAccessStatus_DeniedByUser;
                break;

            default:
                result->ulVal = (ULONG)KnownFoldersAccessStatus_AllowedPerAppFolder;
                break;
        }
    }

    return status;
}