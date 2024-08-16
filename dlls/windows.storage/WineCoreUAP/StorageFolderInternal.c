/* WinRT Windows.Storage.StorageFolder Implementation
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

#include "StorageFolderInternal.h"
#include "StorageItemInternal.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

extern struct IStorageFolderVtbl storage_folder_vtbl;
extern struct IStorageItemVtbl storage_item_vtbl;

int64_t FileTimeToUnixTime(const FILETIME *ft) {
    ULARGE_INTEGER ull;

    // Copy the FILETIME (which is a 64-bit value) to a ULARGE_INTEGER.
    ull.LowPart = ft->dwLowDateTime;
    ull.HighPart = ft->dwHighDateTime;

    // Convert from Windows epoch (1601-01-01) to Unix epoch (1970-01-01)
    return (ull.QuadPart / WINDOWS_TICK) - SEC_TO_UNIX_EPOCH;
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
    const wchar_t* rawBuffer = WindowsGetStringRawBuffer(hString, &length);
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, NULL, 0, NULL, NULL);
    char* multiByteStr = (char*)malloc(bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, multiByteStr, bufferSize, NULL, NULL);
    multiByteStr[bufferSize] = '\0';

    return multiByteStr;
}

HRESULT WINAPI storage_folder_AssignFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT hr;
    DWORD attrib;
    struct storage_folder *folder;
    
    TRACE( "iface %p, value %p\n", invoker, result );
    if (!result) return E_INVALIDARG;
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;
    
    attrib = GetFileAttributesA(HStringToLPCSTR((HSTRING)param));
    if (attrib == INVALID_FILE_ATTRIBUTES) {
        hr = E_INVALIDARG;
    } else {
        hr = S_OK;
        impl_from_IStorageItem(&folder->IStorageItem_iface)->Path = (HSTRING)param;
    }

    result->vt = VT_UNKNOWN;
    if (SUCCEEDED(hr))
    {
        result->ppunkVal = (IUnknown **)&folder->IStorageFolder_iface;
    }

    return hr;
}

HRESULT storage_folder_FetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HANDLE hFile;
    FILETIME ftCreate;
    HRESULT hr;
    DWORD attrib;
    HSTRING Path;
    char * fullPath;
    struct storage_item *item;
    struct storage_folder *folder;
    size_t length;

    folder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem(&folder->IStorageItem_iface)->Path;

    length = strlen(HStringToLPCSTR(Path)) + 1; 
    fullPath = (char*)malloc(length);
    strcpy(fullPath, HStringToLPCSTR(Path));

    TRACE( "iface %p, value %p\n", invoker, result );
    if (!result) return E_INVALIDARG;
    if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;

    item->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    item->ref = 1;

    PathAppendA(fullPath, HStringToLPCSTR((HSTRING)param));
    
    attrib = GetFileAttributesA(fullPath);
    if (attrib == INVALID_FILE_ATTRIBUTES) {
        hr = E_INVALIDARG;
    } else {
        switch (attrib)
        {
            case FILE_ATTRIBUTE_NORMAL:
                item->Attributes = FileAttributes_Normal;
                break;
            case FILE_ATTRIBUTE_READONLY:
                item->Attributes = FileAttributes_ReadOnly;
                break;
            case FILE_ATTRIBUTE_DIRECTORY:
                item->Attributes = FileAttributes_Directory;
                break;
            case FILE_ATTRIBUTE_ARCHIVE:
                item->Attributes = FileAttributes_Archive;
                break;
            case FILE_ATTRIBUTE_TEMPORARY:
                item->Attributes = FileAttributes_Temporary;
                break;
        }

        hFile = CreateFileA(fullPath, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            MessageBoxW(NULL, L"Couldn't open a file for READ.", L"WineCoreUAP", MB_ICONERROR);
            return E_ABORT;
        }

        if (!GetFileTime(hFile, &ftCreate, NULL, NULL)) {
            MessageBoxW(NULL, L"Failed to get file time.", L"WineCoreUAP", MB_ICONERROR);
            CloseHandle(hFile);
            return E_ABORT;
        }

        item->DateCreated.UniversalTime = FileTimeToUnixTime(&ftCreate);
        item->Name = (HSTRING)param;
        WindowsCreateString(CharToLPCWSTR(fullPath), wcslen(CharToLPCWSTR(fullPath)), &item->Path);

        hr = S_OK;
    }
    
    result->vt = VT_UNKNOWN;
    if (SUCCEEDED(hr))
    {
        result->ppunkVal = (IUnknown **)&item->IStorageItem_iface;
    }

    return hr;
}