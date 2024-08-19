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

int64_t FileTimeToUnixTime(const FILETIME *ft) {
    ULARGE_INTEGER ull;

    // Copy the FILETIME (which is a 64-bit value) to a ULARGE_INTEGER.
    ull.LowPart = ft->dwLowDateTime;
    ull.HighPart = ft->dwHighDateTime;

    // Convert from Windows epoch (1601-01-01) to Unix epoch (1970-01-01)
    return (ull.QuadPart / WINDOWS_TICK) - SEC_TO_UNIX_EPOCH;
}

void GenerateUniqueFileName(char* buffer, size_t bufferSize) {
    UUID uuid;
    char * str;

    UuidCreate(&uuid);
    UuidToStringA(&uuid, (RPC_CSTR*)&str);
    snprintf(buffer, bufferSize, "%s", str);

    RpcStringFreeA((RPC_CSTR*)&str);
}

LPCWSTR CharToLPCWSTR(char * charString) {
    int len;
    int stringLength = strlen(charString) + 1;
    LPCWSTR wideString;

    len = MultiByteToWideChar(CP_ACP, 0, charString, stringLength, 0, 0);
    wideString = (LPCWSTR)malloc(len * sizeof(wchar_t));

    MultiByteToWideChar(CP_ACP, 0, charString, stringLength, (LPWSTR)wideString, len);

    return wideString;
}

LPCSTR HStringToLPCSTR( HSTRING hString ) {
    UINT32 length = WindowsGetStringLen(hString);
    const wchar_t * rawBuffer = WindowsGetStringRawBuffer(hString, &length);
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, NULL, 0, NULL, NULL);
    char * multiByteStr = (char*)malloc(bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, multiByteStr, bufferSize, NULL, NULL);
    multiByteStr[bufferSize] = '\0';

    return multiByteStr;
}
