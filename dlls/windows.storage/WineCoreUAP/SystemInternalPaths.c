/* WinRT Windows.Storage.UserDataPaths Internal Implementation
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

#include "SystemInternalPaths.h"

#include <shlobj.h>
#include <shlwapi.h>

_ENABLE_DEBUGGING_

HRESULT WINAPI system_data_paths_GetKnownFolder( ISystemDataPaths *iface, const char * FOLDERID, HSTRING *value )
{
    //This only returns the path. No permissions system is required.
    HRESULT status = S_OK;
    PWSTR path;

    TRACE( "iface %p, folderid %s, value %p\n", iface, FOLDERID, value );

    path = (PWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );

    if ( !strcmp( FOLDERID, "fonts" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Fonts, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "programdata" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_ProgramData, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "public" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Public, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "publicdesktop" ) ) {
        status = SHGetKnownFolderPath(&FOLDERID_PublicDesktop, 0, NULL, &path);
    } else if ( !strcmp( FOLDERID, "publicdocuments" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_PublicDocuments, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "publicdownloads" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_PublicDownloads, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "publicmusic" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_PublicMusic, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "publicpictures" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_PublicPictures, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "publicvideos" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_PublicVideos, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "system" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_System, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "systemhost" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_System, 0, NULL, &path );
    } else if (!strcmp(FOLDERID, "systemx86")) {
        status = SHGetKnownFolderPath( &FOLDERID_SystemX86, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "systemx64" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_SystemX86, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "systemarm" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_SystemX86, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "userprofiles" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_UserProfiles, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "windows" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Windows, 0, NULL, &path );
    } else {
        status = E_NOTIMPL;
    }

    if ( SUCCEEDED( status ) )
    {
        status = WindowsCreateString( path, wcslen( path ), value );
    }

    free( path );
    
    return S_OK;
}