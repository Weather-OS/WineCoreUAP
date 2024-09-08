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
    CHAR username[256];
    CHAR path[MAX_PATH] = "C:\\users\\";
    CHAR manifestPath[MAX_PATH];
    DWORD username_len = sizeof(username);

    struct appx_package package;

    GetModuleFileNameA(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecA(manifestPath);
    PathAppendA(manifestPath, "AppxManifest.xml");

    if (!GetUserNameA(username, &username_len)) {
        printf("Something went wrong\n.");
        return E_UNEXPECTED;
    }

    if ( !OK( registerAppxPackage( manifestPath, &package ) ) )
    {
        printf("Something went wrong\n.");
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

    PathAppendA(path, username);

    if ( SUCCEEDED( status ) )
    {
        switch ( folderId )
        {
            case KnownFolderId_MusicLibrary:
                if ( !musicLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    PathAppendA( path, "Music" );
                break;

            case KnownFolderId_PicturesLibrary:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    PathAppendA( path, "Pictures" );
                break;
            
            case KnownFolderId_VideosLibrary:
                if ( !videosLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    PathAppendA( path, "Videos" );
                break;

            case KnownFolderId_DocumentsLibrary:
                if ( !documentsLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    PathAppendA( path, "Documents" );
                break;

            case KnownFolderId_RemovableDevices:
                if ( !removableDevicesAllowed )
                    status = E_ACCESSDENIED;
                else
                    strcpy( path, "\\\\.\\" );
                break;

            case KnownFolderId_HomeGroup:
                if ( !homeGroupAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = E_NOTIMPL;
                break;

            case KnownFolderId_MediaServerDevices:
                if ( !mediaServerAllowed )
                    status = E_ACCESSDENIED;
                else
                    status = E_NOTIMPL;
                break;

            case KnownFolderId_Objects3D:
                PathAppendA( path, "Objects3D" );
                CreateDirectoryA( path, NULL );
                break;

            case KnownFolderId_AppCaptures:
                if ( !videosLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                {
                    PathAppendA( path, "Videos" );
                    PathAppendA( path, "Captures" );
                }
                break;

            case KnownFolderId_RecordedCalls:
                status = E_NOTIMPL;
                break;

            case KnownFolderId_CameraRoll:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                {
                    PathAppendA( path, "Pictures" );
                    PathAppendA( path, "Camera Roll" );
                }
                break;

            case KnownFolderId_Playlists:
                if ( !musicLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                    PathAppendA( path, "Music" );
                    PathAppendA( path, "Playlists" );
                    CreateDirectoryA( path, NULL );
                break;

            case KnownFolderId_SavedPictures:
                if ( !picturesLibraryAllowed )
                    status = E_ACCESSDENIED;
                else
                {
                    PathAppendA( path, "Pictures" );
                    PathAppendA( path, "Saved Pictures" );
                }
                break;

            default:
                status = E_NOTIMPL;
        }
    }
    
    if ( SUCCEEDED( status ) )
    {
        status = WindowsCreateString( CharToLPCWSTR( path ), wcslen( CharToLPCWSTR( path ) ), value );
    }

    return status;
}

HRESULT WINAPI known_folders_statics_RequestAccess( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HSTRING KnownFolderPath;
    INT promptResult;
    CHAR title[1024];
    CHAR message[1024];
    CHAR manifestPath[MAX_PATH];
    DWORD attributes;

    struct appx_package package;

    GetModuleFileNameA(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecA(manifestPath);
    PathAppendA(manifestPath, "AppxManifest.xml");

    if ( !OK( registerAppxPackage( manifestPath, &package ) ) )
    {
        printf("Something went wrong\n.");
        status = E_UNEXPECTED;
    }

    status = known_folders_statics_GetKnownFolder ( (KnownFolderId)param, &KnownFolderPath );
    if ( status == E_ACCESSDENIED )
    {
        result->vt = VT_INT;
        result->lVal = (LONG)KnownFoldersAccessStatus_NotDeclaredByApp;
        return status;
    }

    attributes = GetFileAttributesA( HStringToLPCSTR( KnownFolderPath ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES )
    {
        if ( GetLastError() == ERROR_ACCESS_DENIED )
        {
            result->vt = VT_INT;
            result->lVal = (LONG)KnownFoldersAccessStatus_DeniedBySystem;
        }
    }

    if ( SUCCEEDED( status ) )
    {
        if ( package.Package.Properties.DisplayName )
        {
            sprintf( title, "Let %s access the following file location?", package.Package.Properties.DisplayName );
        } else
        {
            sprintf( title, "Let this app access the following file location?" );
        }

        sprintf( message, "%s\n%s", title, HStringToLPCSTR( KnownFolderPath ) );
        
        promptResult = MessageBoxA (
            NULL,
            message,
            title,
            MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2
        );

        result->vt = VT_INT;

        switch ( promptResult )
        {
            case IDYES:
                result->lVal = (LONG)KnownFoldersAccessStatus_Allowed;
                break;
                
            case IDNO:
                result->lVal = (LONG)KnownFoldersAccessStatus_DeniedByUser;
                break;

            default:
                result->lVal = (LONG)KnownFoldersAccessStatus_AllowedPerAppFolder;
                break;
        }
    }

    return status;
}