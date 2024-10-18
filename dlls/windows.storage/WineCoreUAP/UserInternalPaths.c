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

#include "UserInternalPaths.h"

HRESULT WINAPI user_data_paths_GetKnownFolder( IUserDataPaths *iface, const char * FOLDERID, HSTRING *value ) 
{
    //This only returns the path. No permissions system is required.
    WCHAR path[MAX_PATH] = L"C:\\users\\";
    WCHAR username[256];
    DWORD username_len = sizeof(username);

    if (!GetUserNameW(username, &username_len)) {
        return E_UNEXPECTED;
    }

    PathAppendW(path, username);

    if (!strcmp(FOLDERID, "cameraroll")) {
        PathAppendW(path, L"Pictures");
    } else if (!strcmp(FOLDERID, "cookies")) {
        PathAppendW(path, L"Cookies"); //Doesn't exist...
    } else if (!strcmp(FOLDERID, "desktop")) {
        PathAppendW(path, L"Desktop");
    } else if (!strcmp(FOLDERID, "documents")) {
        PathAppendW(path, L"Documents");
    } else if (!strcmp(FOLDERID, "favorites")) {
        PathAppendW(path, L"Favorites");
    } else if (!strcmp(FOLDERID, "history")) {
        PathAppendW(path, L"INetHistory");
    } else if (!strcmp(FOLDERID, "internetcache")) {
        PathAppendW(path, L"INetCache");
    } else if (!strcmp(FOLDERID, "localappdata")) {
        PathAppendW(path, L"AppData\\Local");
    } else if (!strcmp(FOLDERID, "localappdatalow")) {
        PathAppendW(path, L"AppData\\LocalLow");
    } else if (!strcmp(FOLDERID, "music")) {
        PathAppendW(path, L"Music");
    } else if (!strcmp(FOLDERID, "pictures")) {
        PathAppendW(path, L"Pictures");
    } else if (!strcmp(FOLDERID, "profile")) {
        PathAppendW(path, L"");
    } else if (!strcmp(FOLDERID, "recent")) {
        PathAppendW(path, L"Recent");
    } else if (!strcmp(FOLDERID, "roamingappdata")) {
        PathAppendW(path, L"AppData\\Roaming");
    } else if (!strcmp(FOLDERID, "savedpictures")) {
        PathAppendW(path, L"Pictures");
    } else if (!strcmp(FOLDERID, "screenshots")) {
        PathAppendW(path, L"Pictures\\Screenshots");
    } else if (!strcmp(FOLDERID, "templates")) {
        PathAppendW(path, L"Templates");
    } else if (!strcmp(FOLDERID, "videos")) {
        PathAppendW(path, L"Videos");
    }

    if (WindowsCreateString( path, wcslen(path), value ) != S_OK) {
        return E_UNEXPECTED;
    }
    
    return S_OK;
}