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

_ENABLE_DEBUGGING_

static HRESULT setSePrivilege( LPCSTR privilege, BOOL enable )
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
        return E_FAIL;

    if ( !LookupPrivilegeValueA( NULL, privilege, &luid ) )
    {
        CloseHandle( hToken );
        return HRESULT_FROM_WIN32( GetLastError() );
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

    AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(tp), NULL, NULL );

    CloseHandle( hToken );
    return S_OK;
}

HRESULT WINAPI application_data_Init( IApplicationData **value )
{
    HRESULT status = S_OK;
    LPCWSTR AppName;    
    LSTATUS hiveStatus;
    WCHAR path[MAX_PATH] = L"C:\\users\\";
    WCHAR username[256];
    WCHAR manifestPath[MAX_PATH];
    DWORD username_len = sizeof(username);
    DWORD dataSize = sizeof(UINT32);
    DWORD type;
    HKEY rootKey;
    HKEY tmpKey;

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

    status = setSePrivilege( SE_RESTORE_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    status = setSePrivilege( SE_BACKUP_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    // Guard against re-entry
    RegDeleteTreeW( HKEY_LOCAL_MACHINE, rootKeyName );

    if ( PathFileExistsW( path ) )
    {
        hiveStatus = RegLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName, path );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, rootKeyName, 0, KEY_ALL_ACCESS, &rootKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegQueryValueExW( rootKey, L"Version", NULL, &type, (LPBYTE)&data->Version, &dataSize );
        if ( hiveStatus != ERROR_SUCCESS ) data->Version = 0;

        hiveStatus = RegCloseKey( rootKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegUnLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );
    } else
    {
        hiveStatus = RegCreateKeyW( HKEY_LOCAL_MACHINE, rootKeyName, &rootKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegCreateKeyW( rootKey, L"LocalState", &tmpKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegCloseKey( tmpKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegCreateKeyW( rootKey, L"RoamingState", &tmpKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegCloseKey( tmpKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegSaveKeyW( rootKey, path, NULL );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );
        data->Version = 0;    
        
        hiveStatus = RegDeleteTreeW( HKEY_LOCAL_MACHINE, rootKeyName );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegCloseKey( rootKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );
    }

    status = setSePrivilege( SE_RESTORE_NAME, FALSE );
    if ( FAILED( status ) ) return status;

    status = setSePrivilege( SE_BACKUP_NAME, FALSE );
    if ( FAILED( status ) ) return status;

    *value = &data->IApplicationData_iface;

    TRACE( "created IApplicationData %p.\n", value );
    return status;
}


HRESULT WINAPI application_data_SetVersion( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    LSTATUS hiveStatus;
    LPWSTR appDataPath;
    UINT32 ver;
    HKEY rootKey;    

    struct application_data *data = impl_from_IApplicationData( (IApplicationData *)invoker );
    struct set_version_options *set_version_options = (struct set_version_options*)param;
    appDataPath = (LPWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );

    status = IApplicationDataSetVersionHandler_Invoke( set_version_options->handler, &set_version_options->request );

    if ( SUCCEEDED( status ))
    {
        wcscpy( appDataPath, WindowsGetStringRawBuffer( impl_from_IApplicationData( (IApplicationData *)invoker )->appDataPath, NULL ) );
        PathAppendW( appDataPath, SETTINGS_PATH );
        PathAppendW( appDataPath, SETTINGS_DATA_PATH );

        ISetVersionRequest_get_DesiredVersion( &set_version_options->request, &ver );

        status = setSePrivilege( SE_RESTORE_NAME, TRUE );
        if ( FAILED( status ) ) return status;

        status = setSePrivilege( SE_BACKUP_NAME, TRUE );
        if ( FAILED( status ) ) return status;
    
        // Load the hive, set the version, and unload the hive
        hiveStatus = RegLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName, appDataPath );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, rootKeyName, 0, KEY_ALL_ACCESS, &rootKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegSetValueExW( rootKey, L"Version", 0, REG_DWORD, (LPBYTE)&ver, sizeof(ver) );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegSaveKeyW( rootKey, appDataPath, NULL );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        hiveStatus = RegCloseKey( rootKey );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );
    
        hiveStatus = RegUnLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName );
        if ( hiveStatus != ERROR_SUCCESS ) return HRESULT_FROM_WIN32( hiveStatus );

        status = setSePrivilege( SE_RESTORE_NAME, FALSE );
        if ( FAILED( status ) ) return status;
    
        status = setSePrivilege( SE_BACKUP_NAME, FALSE );
        if ( FAILED( status ) ) return status;
    }

    data->Version = ver;

    return status;
}