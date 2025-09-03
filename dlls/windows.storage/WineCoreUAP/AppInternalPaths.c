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

_ENABLE_DEBUGGING_

HRESULT WINAPI
app_data_paths_GetKnownFolder(
    IAppDataPaths *iface,
    GUID FOLDERID,
    HSTRING *value
) {
    appxstatus regStatus;
    HRESULT status = S_OK;
    LPCWSTR AppName;
    PWSTR path;
    WCHAR manifestPath[MAX_PATH];

    struct appx_package package;

    TRACE( "iface %p, folderid %s, value %p\n", iface, debugstr_guid( &FOLDERID ), value );

    path = (PWSTR)CoTaskMemAlloc( MAX_PATH * sizeof( WCHAR ) );

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

    SHGetKnownFolderPath( &FOLDERID_LocalAppData, 0, NULL, &path );

    PathAppendW( path, L"Packages" );
    PathAppendW( path, AppName );

    if ( IsEqualGUID( &FOLDERID, &FOLDERID_Cookies ) ) 
    {
        PathAppendW( path, L"AC\\INetCookies" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_Desktop ) ) 
    {
        PathAppendW( path, L"LocalState\\Desktop" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_Documents ) ) 
    {
        PathAppendW( path, L"LocalState\\Documents" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_Favorites ) ) 
    {
        PathAppendW( path, L"LocalState\\Favorites" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_History ) ) 
    {
        PathAppendW( path, L"AC\\INetHistory" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_InternetCache ) ) 
    {
        PathAppendW( path, L"AC\\INetCache" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_LocalAppData ) ) 
    {
        PathAppendW( path, L"LocalState" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_ProgramData ) ) 
    {
        PathAppendW( path, L"LocalState\\ProgramData" );
    } else if ( IsEqualGUID( &FOLDERID, &FOLDERID_RoamingAppData ) ) 
    {
        PathAppendW( path, L"RoamingState" );
    }

    status = WindowsCreateString( path, wcslen( path ), value );

_CLEANUP:
    CoTaskMemFree( path );
    return status;
}