/* WinRT Windows.Storage.AppDataPaths Internal Implementation
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

#include "AppInternalPaths.h"

#include <shlobj.h>
#include <shlwapi.h>

_ENABLE_DEBUGGING_

HRESULT WINAPI app_data_paths_GetKnownFolder(IAppDataPaths *iface, const char * FOLDERID, HSTRING *value) 
{    
    HRESULT status = S_OK;
    appxstatus regStatus;

    WCHAR path[MAX_PATH] = L"C:\\users\\";
    WCHAR username[256];
    WCHAR manifestPath[MAX_PATH];
    LPCWSTR AppName;
    DWORD username_len = sizeof(username);

    struct appx_package package;

    TRACE( "iface %p, folderid %s, value %p\n", iface, FOLDERID, value );

    GetModuleFileNameW( NULL, manifestPath, MAX_PATH );
    PathRemoveFileSpecW( manifestPath );
    PathAppendW( manifestPath, L"AppxManifest.xml" );

    if ( !PathFileExistsW( manifestPath ) )
    {
        status = APPX_E_MISSING_REQUIRED_FILE;
        goto _CLEANUP;
    }

    regStatus = registerAppxPackage( manifestPath, &package );
    if ( regStatus == STATUS_FAIL )
    {
        status = APPX_E_INVALID_MANIFEST;
        goto _CLEANUP;
    }

    AppName = package.Package.Identity.Name;

    if ( !GetUserNameW( username, &username_len ) ) {
        status = HRESULT_FROM_WIN32( GetLastError() );
        goto _CLEANUP;
    }

    PathAppendW( path, username );
    PathAppendW( path, L"AppData\\Local\\Packages" );
    PathAppendW( path, AppName ); // Assuming AppName now holds the correct package name

    if ( !strcmp( FOLDERID, "cookies" ) ) {
        PathAppendW( path, L"AC\\INetCookies" );
    } else if ( !strcmp( FOLDERID, "desktop" ) ) {
        PathAppendW( path, L"LocalState\\Desktop" );
    } else if ( !strcmp( FOLDERID, "documents" ) ) {
        PathAppendW( path, L"LocalState\\Documents" );
    } else if ( !strcmp( FOLDERID, "favorites" ) ) {
        PathAppendW( path, L"LocalState\\favorites" );
    } else if ( !strcmp( FOLDERID, "history" ) ) {
        PathAppendW( path, L"AC\\INetHistory" );
    } else if ( !strcmp( FOLDERID, "internet_cache" ) ) {
        PathAppendW( path, L"AC\\INetCache" );
    } else if ( !strcmp( FOLDERID, "localappdata" ) ) {
        PathAppendW( path, L"LocalState" );
    } else if ( !strcmp( FOLDERID, "programdata" ) ) {
        PathAppendW( path, L"LocalState\\ProgramData" );
    } else if ( !strcmp( FOLDERID, "roamingappdata") ) {
        PathAppendW( path, L"RoamingState" );
    }

    status = WindowsCreateString( path, wcslen( path ), value );

_CLEANUP:
    return status;
}