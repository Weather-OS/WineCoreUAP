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

VOID GenerateUniqueFileName( LPSTR buffer, SIZE_T bufferSize ) {
    UUID uuid;
    LPSTR str;

    UuidCreate(&uuid);
    UuidToStringA(&uuid, (RPC_CSTR*)&str);
    snprintf(buffer, bufferSize, "%s", str);

    RpcStringFreeA((RPC_CSTR*)&str);
}

LPCWSTR CharToLPCWSTR( LPSTR charString ) {
    INT len;
    INT stringLength = strlen(charString) + 1;
    LPCWSTR wideString;

    len = MultiByteToWideChar(CP_ACP, 0, charString, stringLength, 0, 0);
    wideString = (LPCWSTR)malloc(len * sizeof(LPCWSTR));

    MultiByteToWideChar(CP_ACP, 0, charString, stringLength, (LPWSTR)wideString, len);

    return wideString;
}

LPCSTR HStringToLPCSTR( HSTRING hString ) {
    UINT32 length = WindowsGetStringLen(hString);
    LPCWSTR rawBuffer = WindowsGetStringRawBuffer(hString, &length);
    INT bufferSize = WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, NULL, 0, NULL, NULL);
    LPSTR multiByteStr = (LPSTR)malloc(bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, multiByteStr, bufferSize, NULL, NULL);
    multiByteStr[bufferSize] = '\0';

    return multiByteStr;
}

VOID DeleteDirectoryRecursively(LPCSTR directoryPath)
{
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind;
    char searchPath[MAX_PATH];
    char fullPath[MAX_PATH];

    snprintf(searchPath, sizeof(searchPath), "%s\\*.*", directoryPath);

    hFind = FindFirstFileA(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        printf("Error: Cannot open directory: %s\n", directoryPath);
        return;
    }

    do 
    {
        if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) 
        {
            snprintf(fullPath, sizeof(fullPath), "%s\\%s", directoryPath, findFileData.cFileName);

            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
            {
                DeleteDirectoryRecursively(fullPath);

                RemoveDirectoryA(fullPath);
            } 
            else 
            {
                if (!DeleteFileA(fullPath)) 
                {
                    printf("Error: Unable to delete file: %s\n", fullPath);
                }
            }
        }
    } 
    while (FindNextFileA(hFind, &findFileData) != 0);

    FindClose(hFind);

    if (!RemoveDirectoryA(directoryPath)) 
    {
        printf("Error: Unable to delete directory: %s\n", directoryPath);
    }
}