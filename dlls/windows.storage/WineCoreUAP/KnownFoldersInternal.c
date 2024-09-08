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

#include "KnownFoldersInternal.h"

HRESULT WINAPI known_folders_statics_GetKnownFolder( KnownFolderId folderId , HSTRING *value ) 
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
                    PathAppendA( path, "Videos" );
                    PathAppendA( path, "Captures" );
                break;

            case KnownFolderId_RecordedCalls:
                status = E_NOTIMPL;

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