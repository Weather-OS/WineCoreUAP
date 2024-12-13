/* WinRT Windows.Storage.StorageItem Implementation
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

#include "StorageItemInternal.h"

_ENABLE_DEBUGGING_

extern struct IActivationFactoryVtbl factory_vtbl;

static VOID DeleteDirectoryRecursively( LPCWSTR directoryPath )
{
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind;
    WCHAR searchPath[MAX_PATH];
    WCHAR fullPath[MAX_PATH];

    swprintf( searchPath, sizeof(searchPath), L"%s\\*.*", directoryPath );

    hFind = FindFirstFileW( searchPath, &findFileData );
    if (hFind == INVALID_HANDLE_VALUE) 
    {
        return;
    }

    do 
    {
        if ( wcscmp( findFileData.cFileName, L"." ) != 0 && wcscmp( findFileData.cFileName, L".." ) != 0 ) 
        {
            swprintf( fullPath, sizeof(fullPath), L"%s\\%s", directoryPath, findFileData.cFileName );

            if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
            {
                DeleteDirectoryRecursively( fullPath );

                RemoveDirectoryW( fullPath );
            } 
            else 
            {
                if ( !DeleteFileW( fullPath ) ) 
                {
                    return;
                }
            }
        }
    } 
    while ( FindNextFileW( hFind, &findFileData ) != 0 );

    FindClose( hFind );

    if ( !RemoveDirectoryW( directoryPath ) ) 
    {
        return;
    }
}

static VOID GenerateUniqueFileName( LPWSTR buffer, SIZE_T bufferSize ) {
    UUID uuid;
    LPWSTR str;

    UuidCreate( &uuid );
    UuidToStringW( &uuid, (RPC_WSTR*)&str );
    swprintf( buffer, bufferSize, L"%s", str );

    RpcStringFreeW( (RPC_WSTR*)&str );
}

static INT64 FileTimeToUnixTime( const FILETIME *ft ) {
    ULARGE_INTEGER ull;

    ull.LowPart = ft->dwLowDateTime;
    ull.HighPart = ft->dwHighDateTime;

    return ( ull.QuadPart / WINDOWS_TICK ) - SEC_TO_UNIX_EPOCH;
}

HRESULT WINAPI storage_item_Internal_CreateNew( HSTRING itemPath, IStorageItem * result ) 
{
    WCHAR itemName[MAX_PATH];
    DWORD attributes;
    HANDLE itemFile;
    HSTRING tempName;
    HRESULT status;
    FILETIME itemFileCreatedTime;

    struct storage_item *item;

    TRACE( "iface %p, value %p\n", itemPath, result );
    if (!result) return E_INVALIDARG;
    if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;

    item = impl_from_IStorageItem( result );

    if ( PathIsURLW( WindowsGetStringRawBuffer( itemPath, NULL ) ) )
        status = E_INVALIDARG;
    
    attributes = GetFileAttributesW( WindowsGetStringRawBuffer( itemPath, NULL ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES ) 
    {
        status = E_INVALIDARG;
    } else 
    {
        //File Time
        itemFile = CreateFileW( WindowsGetStringRawBuffer( itemPath, NULL ), GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
        if (itemFile == INVALID_HANDLE_VALUE) 
        {
            return E_ABORT;
        }

        if ( !GetFileTime( itemFile, &itemFileCreatedTime, NULL, NULL ) ) 
        {
            CloseHandle( itemFile );
            return E_ABORT;
        }

        item->DateCreated.UniversalTime = FileTimeToUnixTime( &itemFileCreatedTime );

        //File Attributes
        switch ( attributes )
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

        //File Path
        WindowsDuplicateString( itemPath, &item->Path );

        //File Name
        wcscpy( itemName, wcsrchr( WindowsGetStringRawBuffer( itemPath, NULL ), '\\' ) );
        WindowsCreateString( itemName + 1, wcslen( itemName + 1 ), &tempName );
        WindowsDuplicateString( tempName, &item->Name );

        CloseHandle( itemFile );

        status = S_OK;
    }

    return status;
}

HRESULT WINAPI storage_item_Rename( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    DWORD attributes;
    HRESULT status = S_OK;
    HSTRING itemPath;
    WCHAR newItemPath[MAX_PATH];
    WCHAR uuidName[MAX_PATH];

    struct storage_item *item;
    struct storage_item_rename_options *rename_options = (struct storage_item_rename_options *)param;
    NameCollisionOption collisionOption = rename_options->option;
    HSTRING name = rename_options->name;

    TRACE( "iface %p, value %p\n", invoker, name );
    if (!name) return E_INVALIDARG;

    item = impl_from_IStorageItem( (IStorageItem *)invoker );
    WindowsDuplicateString( item->Path, &itemPath );

    wcscpy( newItemPath, WindowsGetStringRawBuffer( itemPath, NULL ) );
    *wcsrchr( newItemPath, L'\\' ) = '\0';

    //Perform rename
    attributes = GetFileAttributesW( WindowsGetStringRawBuffer( itemPath, NULL ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES ) 
    {
        status = E_INVALIDARG;
    } else 
    {
        switch (collisionOption)
        {
            case NameCollisionOption_FailIfExists:
                PathAppendW( newItemPath, WindowsGetStringRawBuffer( name, NULL ) );

                attributes = GetFileAttributesW( newItemPath );
                if ( attributes != INVALID_FILE_ATTRIBUTES ) 
                    status = E_INVALIDARG;
                else
                    status = S_OK;
                break;

            case NameCollisionOption_GenerateUniqueName:
                GenerateUniqueFileName( uuidName, sizeof(uuidName) );
                PathAppendW( newItemPath, uuidName );

                attributes = GetFileAttributesW( newItemPath );
                if ( attributes != INVALID_FILE_ATTRIBUTES ) 
                    return E_ABORT;
                else
                    status = S_OK;
                break;

            case NameCollisionOption_ReplaceExisting:
                PathAppendW( newItemPath, WindowsGetStringRawBuffer( name, NULL ) );

                attributes = GetFileAttributesW( newItemPath );
                if ( attributes != INVALID_FILE_ATTRIBUTES ) 
                {
                    if ( !DeleteFileW( newItemPath ) )
                    {
                        return E_ABORT;
                    }
                }
                else
                    status = S_OK;
                break;
        }
    }

    if ( !MoveFileW( WindowsGetStringRawBuffer( item->Path, NULL ), newItemPath ) )
    {
        status = E_ABORT;
    }

    if ( SUCCEEDED( status ) )
    {
        WindowsCreateString( newItemPath, wcslen( newItemPath ), &item->Path );
        WindowsDuplicateString( name, &item->Name );
    }
    return status;
}

HRESULT WINAPI storage_item_Delete( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    DWORD attributes;
    HRESULT status = S_OK;
    HSTRING itemPath;

    struct storage_item *item;

    StorageDeleteOption deleteOption = (StorageDeleteOption)param;

    TRACE( "iface %p\n", invoker );

    item = impl_from_IStorageItem( (IStorageItem *)invoker );
    WindowsDuplicateString( item->Path, &itemPath );

    //Perform delete
    attributes = GetFileAttributesW( WindowsGetStringRawBuffer( itemPath, NULL ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES ) 
    {
        status = E_INVALIDARG;
    } else 
    {
        switch (deleteOption)
        {
            case StorageDeleteOption_Default:
            case StorageDeleteOption_PermanentDelete:
                if ( attributes == FILE_ATTRIBUTE_DIRECTORY )
                {
                    DeleteDirectoryRecursively( WindowsGetStringRawBuffer( itemPath, NULL ) );
                } else
                {
                    if ( !DeleteFileW( WindowsGetStringRawBuffer( itemPath, NULL ) ) )
                    {
                        status = E_ABORT;
                    }
                }
        }
    }

    if ( SUCCEEDED( status ) )
    {
        free( item );
    }
    return status;
}

HRESULT WINAPI storage_item_GetProperties( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;
    HANDLE file;
    FILETIME lastWrite;
    DWORD fileSize;
    
    struct basic_properties *properties;
    struct storage_item *item;
    
    item = impl_from_IStorageItem( (IStorageItem *)invoker );

    file = CreateFileW( WindowsGetStringRawBuffer( item->Path, NULL ), GENERIC_READ, FILE_SHARE_READ, NULL,
                    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

    fileSize = GetFileSize( file, NULL );

    GetFileTime(file, NULL, NULL, &lastWrite);

    TRACE( "iface %p, value %p\n", invoker, result );
    if (!result) return E_INVALIDARG;
    if (!(properties = calloc( 1, sizeof(*properties) ))) return E_OUTOFMEMORY;

    properties->IActivationFactory_iface.lpVtbl = basic_properties_factory->lpVtbl;
    properties->IBasicProperties_iface.lpVtbl = &basic_properties_vtbl;

    properties->DateModified.UniversalTime = FileTimeToUnixTime( &lastWrite );
    properties->ItemDate = item->DateCreated;
    properties->size = fileSize;
    properties->ref = 1;

    result->vt = VT_UNKNOWN;
    result->punkVal = (IUnknown *)&properties->IBasicProperties_iface;
    
    return status;

}

HRESULT WINAPI storage_item_GetType( IStorageItem * iface, StorageItemTypes * type )
{
    DWORD attributes;
    HRESULT status = S_OK;
    HSTRING itemPath;
    WCHAR path[MAX_PATH];

    struct storage_item *item;

    TRACE( "iface %p, type %p\n", iface, type );
    
    item = impl_from_IStorageItem( iface );
    WindowsDuplicateString( item->Path, &itemPath );
    
    wcscpy( path, WindowsGetStringRawBuffer( itemPath, NULL ) );

    attributes = GetFileAttributesW( path );
    if ( attributes == INVALID_FILE_ATTRIBUTES )
    {
        status = E_INVALIDARG;
    } else
    {
        if ( attributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            *type = StorageItemTypes_Folder;
        } else
        {
            *type = StorageItemTypes_File;
        }
    }

    return status;
}

HRESULT WINAPI storage_item_properties_AssignProperties( IStorageItem* iface, IStorageItemProperties *result )
{
    LPWSTR pathParts[MAX_PATH];
    WCHAR id[MAX_PATH];
    INT partCount = 0;
    LPWSTR token;
    HRESULT status = S_OK;
    SHFILEINFOW shFileInfo = { 0 };

    struct storage_item_properties *properties = impl_from_IStorageItemProperties( result );

    TRACE( "iface %p, result %p\n", iface, result );

    properties->IStorageItemProperties_iface.lpVtbl = &storage_item_properties_vtbl;
    properties->IStorageItemPropertiesWithProvider_iface.lpVtbl = &storage_item_properties_with_provider_vtbl;

    WindowsDuplicateString( impl_from_IStorageItem( iface )->Name, &properties->DisplayName );

    if ( SHGetFileInfoW( WindowsGetStringRawBuffer( impl_from_IStorageItem( iface )->Path, NULL ), 0, &shFileInfo, sizeof( shFileInfo ), SHGFI_TYPENAME ) ) {
        WindowsCreateString( shFileInfo.szTypeName, wcslen( shFileInfo.szTypeName ), &properties->DisplayType );
    }

    wcscpy( id, WindowsGetStringRawBuffer( impl_from_IStorageItem( iface )->Path, NULL ) );

    token = wcstok( id, L"\\", NULL );
    while ( token != NULL ) 
    {
        pathParts[partCount++] = token;
        token = wcstok( NULL, L"\\", NULL );
    }

    if ( partCount >= 2 ) 
    {
        wnsprintfW( id, 23 + MAX_PATH, L"System.ItemPathDisplay:%s\\%s", pathParts[partCount - 2], pathParts[partCount - 1] );
    }

    WindowsCreateString( id, wcslen( id ), &properties->FolderRelativeId );

    return status;
}

HRESULT WINAPI storage_item_properties_with_provider_GetProvider( IStorageItemPropertiesWithProvider *iface, IStorageProvider **value )
{
    HRESULT status = S_OK;
    IStorageItem *storageItem;

    struct storage_item *item;
    struct storage_provider *provider;

    TRACE( "iface %p, value %p\n", iface, value );

    if (!(provider = calloc( 1, sizeof(*provider) ))) return E_OUTOFMEMORY;

    IStorageItemPropertiesWithProvider_QueryInterface( iface, &IID_IStorageItem, (void **)&storageItem );
    item = impl_from_IStorageItem( storageItem );

    provider->IStorageProvider_iface.lpVtbl = &storage_provider_vtbl;

    WindowsDuplicateString( item->Name, &provider->DisplayName );
    WindowsCreateString( L"Local", wcslen( L"Local" ), &provider->Id );

    *value = &provider->IStorageProvider_iface;

    return status;
}
