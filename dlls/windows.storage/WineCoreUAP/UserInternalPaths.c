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

#include "UserInternalPaths.h"

#include <initguid.h>
#include <knownfolders.h>

#include <shlobj.h>
#include <shlwapi.h>

_ENABLE_DEBUGGING_

HRESULT WINAPI user_data_paths_GetKnownFolder( IUserDataPaths *iface, const char * FOLDERID, HSTRING *value ) 
{
    //This only returns the path. No permissions system is required.
    HRESULT status = S_OK;
    PWSTR path;

    TRACE( "iface %p, folderid %s, value %p\n", iface, FOLDERID, value );

    path = (PWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );

    if ( !strcmp( FOLDERID, "cameraroll" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_CameraRoll, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "cookies" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Cookies, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "desktop" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Desktop, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "documents" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Documents, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "favorites" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Favorites, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "history" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_History, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "internetcache" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_InternetCache, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "localappdata ") ) {
        status = SHGetKnownFolderPath( &FOLDERID_LocalAppData, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "localappdatalow" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_LocalAppDataLow, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "music" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Music, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "pictures" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Pictures, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "profile" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Profile, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "recent" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Recent, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "roamingappdata" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_RoamingAppData, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "savedpictures" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_SavedPictures, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "screenshots" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Screenshots, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "templates" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Templates, 0, NULL, &path );
    } else if ( !strcmp( FOLDERID, "videos" ) ) {
        status = SHGetKnownFolderPath( &FOLDERID_Videos, 0, NULL, &path );
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