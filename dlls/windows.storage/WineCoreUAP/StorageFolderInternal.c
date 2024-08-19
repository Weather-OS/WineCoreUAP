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
#include "VectorView/StorageItemVectorView.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

extern struct IStorageFolderVtbl storage_folder_vtbl;
extern struct IStorageItemVtbl storage_item_vtbl;
extern struct IVectorView_IStorageItemVtbl storage_item_vector_view_vtbl;

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

HRESULT WINAPI storage_folder_AssignFolder( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    HRESULT hr;
    DWORD attrib;
    HANDLE hFile;
    FILETIME ftCreate;
    HSTRING FolderPath;
    struct storage_folder *folder;
    struct storage_item * folderItem;
    char * folderName = malloc(sizeof(char *));
    
    TRACE( "iface %p, value %p\n", invoker, result );
    if (!result) return E_INVALIDARG;
    if (!(folder = calloc( 1, sizeof(*folder) ))) return E_OUTOFMEMORY;

    folder->IStorageFolder_iface.lpVtbl = &storage_folder_vtbl;
    folder->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    folder->ref = 1;

    folderItem = impl_from_IStorageItem(&folder->IStorageItem_iface);

    WindowsDuplicateString( (HSTRING)param, &FolderPath );
    
    attrib = GetFileAttributesA( HStringToLPCSTR( FolderPath ) );
    if (attrib == INVALID_FILE_ATTRIBUTES) 
    {
        hr = E_INVALIDARG;
    } else 
    {
        hFile = CreateFileA( HStringToLPCSTR(FolderPath), GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
        if (hFile == INVALID_HANDLE_VALUE) 
        {
            return E_ABORT;
        }

        if ( !GetFileTime( hFile, &ftCreate, NULL, NULL ) ) 
        {
            CloseHandle(hFile);
            return E_ABORT;
        }
        WindowsDuplicateString(FolderPath, &folderItem->Path);
        folderItem->DateCreated.UniversalTime = FileTimeToUnixTime(&ftCreate);
        switch (attrib)
        {
            case FILE_ATTRIBUTE_NORMAL:
                folderItem->Attributes = FileAttributes_Normal;
                break;
            case FILE_ATTRIBUTE_READONLY:
                folderItem->Attributes = FileAttributes_ReadOnly;
                break;
            case FILE_ATTRIBUTE_DIRECTORY:
                folderItem->Attributes = FileAttributes_Directory;
                break;
            case FILE_ATTRIBUTE_ARCHIVE:
                folderItem->Attributes = FileAttributes_Archive;
                break;
            case FILE_ATTRIBUTE_TEMPORARY:
                folderItem->Attributes = FileAttributes_Temporary;
                break;
            default:
                folderItem->Attributes = FileAttributes_Normal;
        }
        strcpy( folderName, strrchr( HStringToLPCSTR(FolderPath), '\\' ) );
        WindowsCreateString( CharToLPCWSTR(folderName + 1), strlen(folderName), &folderItem->Name );
        hr = S_OK;
    }

    result->vt = VT_UNKNOWN;
    if (SUCCEEDED(hr))
    {
        result->ppunkVal = (IUnknown **)&folder->IStorageFolder_iface;
    }

    return hr;
}

HRESULT WINAPI storage_folder_FetchItem( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
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
    Path = impl_from_IStorageItem( &folder->IStorageItem_iface )->Path;

    length = strlen(HStringToLPCSTR(Path)) + 1; 
    fullPath = (char*)malloc(length);
    strcpy(fullPath, HStringToLPCSTR(Path));

    TRACE( "iface %p, value %p\n", invoker, result );
    if (!result) return E_INVALIDARG;
    if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;

    item->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    item->ref = 1;

    PathAppendA( fullPath, HStringToLPCSTR( (HSTRING)param ) );
    
    attrib = GetFileAttributesA(fullPath);
    if (attrib == INVALID_FILE_ATTRIBUTES) 
    {
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
            default:
                item->Attributes = FileAttributes_Normal;
        }

        hFile = CreateFileA( fullPath, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
        if (hFile == INVALID_HANDLE_VALUE) 
        {
            return E_ABORT;
        }

        if ( !GetFileTime( hFile, &ftCreate, NULL, NULL ) ) 
        {
            CloseHandle(hFile);
            return E_ABORT;
        }

        item->DateCreated.UniversalTime = FileTimeToUnixTime(&ftCreate);
        WindowsDuplicateString( (HSTRING)param, &item->Name );
        WindowsCreateString( CharToLPCWSTR(fullPath), wcslen(CharToLPCWSTR(fullPath)), &item->Path );

        hr = S_OK;
    }
    
    result->vt = VT_UNKNOWN;
    if (SUCCEEDED(hr))
    {
        result->ppunkVal = (IUnknown **)&item->IStorageItem_iface;
    }

    return hr;
}

HRESULT WINAPI storage_folder_CreateFolder( IStorageFolder* folder, CreationCollisionOption collisionOption, HSTRING Name, HSTRING *OutPath )
{
    HRESULT hr = S_OK;
    HSTRING Path;
    DWORD attrib;
    BOOL Exists = FALSE;
    BOOL Replace = FALSE;
    struct storage_folder *invokerFolder;
    char * fullPath;
    char uuidName[MAX_PATH];
    size_t length;

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)folder );
    Path = impl_from_IStorageItem(&invokerFolder->IStorageItem_iface)->Path;

    length = strlen(HStringToLPCSTR(Path)) + 1; 
    fullPath = (char*)malloc(length);
    strcpy(fullPath, HStringToLPCSTR(Path));

    TRACE( "iface %p, value %p\n", folder, OutPath );
    switch ( collisionOption )
    {
        case CreationCollisionOption_FailIfExists:
            PathAppendA( fullPath, HStringToLPCSTR( Name ));
            attrib = GetFileAttributesA(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                hr = E_INVALIDARG;
            else 
                hr = S_OK;
            break;

        case CreationCollisionOption_GenerateUniqueName:
            GenerateUniqueFileName( uuidName, sizeof(uuidName) );
            PathAppendA( fullPath, uuidName );

            hr = S_OK;
            break;
        
        case CreationCollisionOption_OpenIfExists:
            PathAppendA( fullPath, HStringToLPCSTR( Name ));
            attrib = GetFileAttributesA(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Exists = TRUE;
            
            hr = S_OK;
            break;
        
        case CreationCollisionOption_ReplaceExisting:
            PathAppendA( fullPath, HStringToLPCSTR( Name ));
            attrib = GetFileAttributesA(fullPath);
            if (attrib != INVALID_FILE_ATTRIBUTES)
                Replace = TRUE;

            hr = S_OK;
            break;
    }
    if (SUCCEEDED(hr))
    {
        if (Replace)
        {
            if ( !RemoveDirectoryA( fullPath ) )
            {
                return E_ABORT;
            }
        }
        if (!Exists)
        {
            if ( !CreateDirectoryA( fullPath, NULL ) )
            {
                return E_ABORT;
            }
            hr = S_OK;
        }
        hr = WindowsCreateString( CharToLPCWSTR(fullPath), wcslen(CharToLPCWSTR(fullPath)), OutPath );
    }
    return hr;
}

HRESULT WINAPI storage_folder_FetchItemsAndCount( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    HANDLE hFile;
    HRESULT hr = S_OK;
    HSTRING Path;
    CHAR searchPath[MAX_PATH];
    FILETIME ftCreate;
    struct storage_folder *invokerFolder;
    struct storage_item_vector_view *itemVector;
    struct storage_item *item;
    char fullItemPath[MAX_PATH];

    invokerFolder = impl_from_IStorageFolder( (IStorageFolder *)invoker );
    Path = impl_from_IStorageItem(&invokerFolder->IStorageItem_iface)->Path;

    if (!(itemVector = calloc( 1, sizeof(*itemVector) ))) return E_OUTOFMEMORY;
    
    itemVector->IVectorView_IStorageItem_iface.lpVtbl = &storage_item_vector_view_vtbl;
    itemVector->ref = 1;
    itemVector->size = 0;

    snprintf(searchPath, MAX_PATH, "%s\\*.*", HStringToLPCSTR(Path));
    PathAppendA(fullItemPath, HStringToLPCSTR(Path));

    if (param == NULL)
    {
        hFind = FindFirstFileA(searchPath, &findFileData);

        if (hFind == INVALID_HANDLE_VALUE) 
        {
            return E_ABORT;
        } 

        while (FindNextFileA(hFind, &findFileData) != 0) 
        {
            if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) 
            {
                if (!(itemVector->elements[itemVector->size] = calloc( 1, sizeof(*itemVector->elements[itemVector->size]) ))) return E_OUTOFMEMORY;
                itemVector->elements[itemVector->size]->lpVtbl = &storage_item_vtbl;
                item = impl_from_IStorageItem( itemVector->elements[itemVector->size] );
                PathAppendA(fullItemPath, findFileData.cFileName);

                WindowsCreateString(
                    CharToLPCWSTR(fullItemPath),
                    strlen(fullItemPath),
                    &item->Path
                );

                WindowsCreateString(
                    CharToLPCWSTR(findFileData.cFileName),
                    strlen(findFileData.cFileName),
                    &item->Name
                );

                switch (findFileData.dwFileAttributes)
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
                    default:
                        item->Attributes = FileAttributes_Normal;
                }

                hFile = CreateFileA( fullItemPath, GENERIC_READ, FILE_SHARE_READ, NULL,
                                   OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
                if (hFile == INVALID_HANDLE_VALUE) 
                {
                    printf("fullitempath was %s\n", fullItemPath);
                    return E_ABORT;
                }

                if ( !GetFileTime( hFile, &ftCreate, NULL, NULL ) ) 
                {
                    CloseHandle(hFile);
                    return E_ABORT;
                }

                item->DateCreated.UniversalTime = FileTimeToUnixTime(&ftCreate);

                itemVector->size++;
            }
        }
    }

    result->vt = VT_UNKNOWN;
    if (SUCCEEDED(hr))
    {
        result->ppunkVal = (IUnknown **)&itemVector->IVectorView_IStorageItem_iface;
    }

    return hr;
}