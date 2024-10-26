/* WinRT Windows.Storage.ApplicationData ApplicationData Implementation
 *
 * Written by Weather
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

#include "ApplicationDataInternal.h"

WINE_DEFAULT_DEBUG_CHANNEL(data);

HRESULT WINAPI application_data_Init( IApplicationData **value )
{
    HRESULT status = S_OK;    
    LPCWSTR AppName;    
    HANDLE settingsFile;
    WCHAR path[MAX_PATH] = L"C:\\users\\";
    WCHAR username[256];
    WCHAR manifestPath[MAX_PATH];
    DWORD username_len = sizeof(username);
    DWORD bytesRead;
    DWORD bytesWritten;    
    CHAR versionChar[9];
    BYTE versionData[8];
    INT i;

    struct application_data *data;
    struct appx_package package;


    GetModuleFileNameW( NULL, manifestPath, MAX_PATH );
    PathRemoveFileSpecW( manifestPath );
    PathAppendW( manifestPath, L"AppxManifest.xml" );

    registerAppxPackage( manifestPath, &package );

    AppName = package.Package.Identity.Name;

    if ( !GetUserNameW( username, &username_len ) ) {
        return E_UNEXPECTED;
    }

    PathAppendW( path, username );
    PathAppendW( path, L"AppData\\Local\\Packages" );
    PathAppendW( path, AppName );

    if ( GetFileAttributesW( path ) == INVALID_FILE_ATTRIBUTES || 
         GetFileAttributesW( path ) != FILE_ATTRIBUTE_DIRECTORY )
    {
        if ( !CreateDirectoryW( path, NULL ) )
            return E_ABORT;
    }

    if (!(data = calloc( 1, sizeof(*data) ))) return E_OUTOFMEMORY;
    if (!value) return E_INVALIDARG;

    data->IApplicationData_iface.lpVtbl = &application_data_vtbl;
    data->ref = 1;

    WindowsCreateString( path, wcslen( path ), &data->appDataPath );

    PathAppendW( path, SETTINGS_PATH );

    if ( GetFileAttributesW( path ) == INVALID_FILE_ATTRIBUTES || 
         GetFileAttributesW( path ) != FILE_ATTRIBUTE_DIRECTORY )
    {
        if ( !CreateDirectoryW( path, NULL ) )
            return E_ABORT;
    }

    PathAppendW( path, SETTINGS_DATA_PATH );

    settingsFile = CreateFileW( path, GENERIC_ALL, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    if ( settingsFile == INVALID_HANDLE_VALUE )
        return E_UNEXPECTED;

    if ( !ReadFile( settingsFile, versionData, 8, &bytesRead, NULL ) )
    {
        CloseHandle( settingsFile );
        return E_ABORT;
    }

    if ( !bytesRead )
    {
        if ( !WriteFile( settingsFile, (BYTE *)"00000000", 8, &bytesWritten, NULL ) )
        {
            CloseHandle( settingsFile );
            return E_ABORT;
        }

        if ( bytesWritten != 8 )
        {
            CloseHandle( settingsFile );
            return E_UNEXPECTED;
        }

        data->Version = 0u;
    } else 
    {
        for ( i = 0; i < 8; i++ ) versionChar[i] = (char)versionData[i];
        versionChar[8] = '\0';
        data->Version = atoi( versionChar );
    }

    *value = &data->IApplicationData_iface;

    CloseHandle( settingsFile );

    TRACE( "created IApplicationData %p.\n", value );
    return status;
}


HRESULT WINAPI application_data_SetVersion( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HANDLE settingsFile;
    LPWSTR appDataPath;
    UINT32 ver;
    DWORD bytesWritten;
    CHAR newVer[9];

    struct set_version_options *set_version_options = (struct set_version_options*)param;
    appDataPath = (LPWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );

    status = IApplicationDataSetVersionHandler_Invoke( set_version_options->handler, &set_version_options->request );

    if ( SUCCEEDED( status ))
    {
        wcscpy( appDataPath, WindowsGetStringRawBuffer( impl_from_IApplicationData( (IApplicationData *)invoker )->appDataPath, NULL ) );
        PathAppendW( appDataPath, SETTINGS_PATH );
        PathAppendW( appDataPath, SETTINGS_DATA_PATH );

        settingsFile = CreateFileW( appDataPath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if ( settingsFile == INVALID_HANDLE_VALUE )
            return E_UNEXPECTED;

        if ( SetFilePointer( settingsFile, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) 
        {
            CloseHandle( settingsFile );
            return E_UNEXPECTED;
        }

        ISetVersionRequest_get_DesiredVersion( &set_version_options->request, &ver );

        sprintf( newVer, "%08d", ver );

        if ( !WriteFile( settingsFile, (BYTE *)newVer, 8, &bytesWritten, NULL ) )
        {
            CloseHandle( settingsFile );
            return E_ABORT;
        }

        if ( bytesWritten != 8 )
        {
            CloseHandle( settingsFile );
            return E_UNEXPECTED;
        }

        CloseHandle( settingsFile );
    }

    return status;
}