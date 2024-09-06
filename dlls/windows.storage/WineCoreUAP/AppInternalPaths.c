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

LPSTR extractNameAttribute(LPSTR line) {
    LPSTR nameStart = strstr(line, "Name=\"");
    LPSTR nameEnd;
    if (nameStart) {
        nameStart += 6; // Move past 'Name="'
        nameEnd = strchr(nameStart, '"');
        if (nameEnd) {
            *nameEnd = '\0'; // Null-terminate the string at the end quote
            printf("WineCoreUAP: Registered Package with Name: %s\n", nameStart);
            return nameStart;
        }
    }
    return nameEnd;
}

HRESULT WINAPI app_data_paths_GetKnownFolder(IAppDataPaths *iface, const char * FOLDERID, HSTRING *value) 
{    
    FILE *file;
    CHAR path[MAX_PATH] = "C:\\users\\";
    CHAR username[256];
    CHAR manifestPath[MAX_PATH];
    CHAR buffer[BUFFER_SIZE] = {0};
    CHAR AppName[BUFFER_SIZE];
    DWORD username_len = sizeof(username);

    GetModuleFileNameA(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecA(manifestPath);
    PathAppendA(manifestPath, "AppxManifest.xml");

    file = fopen(manifestPath, "r");
    if (!file) {
        MessageBoxW(NULL, L"Failed to read AppxManifest.xml", L"WineCoreUAP", MB_ICONERROR);
        printf("Manifest path was %s\n", manifestPath);
        return E_INVALIDARG;
    }

    while (fgets(buffer, BUFFER_SIZE, file)) {
        if (strstr(buffer, "<Identity")) {
            strcat(AppName, extractNameAttribute(buffer));
            break;
        }
    }
    fclose(file);

    if (!GetUserNameA(username, &username_len)) {
        MessageBoxW(NULL, L"Failed to get username", L"WineCoreUAP", MB_ICONERROR);
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
        MessageBoxW(NULL, L"Failed to create Windows string", L"WineCoreUAP", MB_ICONERROR);
        return E_UNEXPECTED;
    }
    return S_OK;
}