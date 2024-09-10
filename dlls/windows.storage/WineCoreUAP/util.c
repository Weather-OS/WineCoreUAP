/* WinRT Windows.Storage Internal Utilities.
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

INT64 FileTimeToUnixTime( const FILETIME *ft ) {
    ULARGE_INTEGER ull;

    // Copy the FILETIME (which is a 64-bit value) to a ULARGE_INTEGER.
    ull.LowPart = ft->dwLowDateTime;
    ull.HighPart = ft->dwHighDateTime;

    // Convert from Windows epoch (1601-01-01) to Unix epoch (1970-01-01)
    return (ull.QuadPart / WINDOWS_TICK) - SEC_TO_UNIX_EPOCH;
}

VOID GenerateUniqueFileName( LPWSTR buffer, SIZE_T bufferSize ) {
    UUID uuid;
    LPWSTR str;

    UuidCreate(&uuid);
    UuidToStringW(&uuid, (RPC_WSTR*)&str);
    swprintf(buffer, bufferSize, L"%s", str);

    RpcStringFreeW((RPC_WSTR*)&str);
}

VOID DeleteDirectoryRecursively(LPCWSTR directoryPath)
{
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind;
    WCHAR searchPath[MAX_PATH];
    WCHAR fullPath[MAX_PATH];

    swprintf(searchPath, sizeof(searchPath), L"%s\\*.*", directoryPath);

    hFind = FindFirstFileW(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        wprintf(L"Error: Cannot open directory: %s\n", directoryPath);
        return;
    }

    do 
    {
        if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) 
        {
            swprintf(fullPath, sizeof(fullPath), L"%s\\%s", directoryPath, findFileData.cFileName);

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
            {
                DeleteDirectoryRecursively(fullPath);

                RemoveDirectoryW(fullPath);
            } 
            else 
            {
                if (!DeleteFileW(fullPath)) 
                {
                    wprintf(L"Error: Unable to delete file: %s\n", fullPath);
                }
            }
        }
    } 
    while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);

    if (!RemoveDirectoryW(directoryPath)) 
    {
        wprintf(L"Error: Unable to delete directory: %s\n", directoryPath);
    }
}