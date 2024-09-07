/* WinRT Windows.Storage.AppDataPaths Internal Implementation
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

#include "util.h"
#include "AppInternalPaths.h"

#define BUFFER_SIZE 1024

HRESULT WINAPI app_data_paths_GetKnownFolder(IAppDataPaths *iface, const char * FOLDERID, HSTRING *value) 
{    
    CHAR path[MAX_PATH] = "C:\\users\\";
    CHAR username[256];
    CHAR manifestPath[MAX_PATH];
    LPSTR AppName;
    DWORD username_len = sizeof(username);

    struct appx_package package;

    GetModuleFileNameA(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecA(manifestPath);
    PathAppendA(manifestPath, "AppxManifest.xml");

    registerAppxPackage( manifestPath, &package );

    AppName = (CHAR *)package.Package.Identity.Name;

    if (!GetUserNameA(username, &username_len)) {
        return E_UNEXPECTED;
    }

    PathAppendA(path, username);
    PathAppendA(path, "AppData\\Local\\Packages");
    PathAppendA(path, AppName); // Assuming AppName now holds the correct package name

    if (!strcmp(FOLDERID, "cookies")) {
        PathAppendA(path, "AC\\INetCookies");
    } else if (!strcmp(FOLDERID, "desktop")) {
        PathAppendA(path, "LocalState\\Desktop");
    } else if (!strcmp(FOLDERID, "documents")) {
        PathAppendA(path, "LocalState\\Documents");
    } else if (!strcmp(FOLDERID, "favorites")) {
        PathAppendA(path, "LocalState\\favorites");
    } else if (!strcmp(FOLDERID, "history")) {
        PathAppendA(path, "AC\\INetHistory");
    } else if (!strcmp(FOLDERID, "internet_cache")) {
        PathAppendA(path, "AC\\INetCache");
    } else if (!strcmp(FOLDERID, "localappdata")) {
        PathAppendA(path, "LocalState");
    } else if (!strcmp(FOLDERID, "programdata")) {
        PathAppendA(path, "LocalState\\ProgramData");
    } else if (!strcmp(FOLDERID, "roamingappdata")) {
        PathAppendA(path, "RoamingState");
    }

    if (WindowsCreateString(CharToLPCWSTR(path), strlen(path), value) != S_OK) {
        return E_UNEXPECTED;
    }
    
    return S_OK;
}