/* WinRT Windows.Storage.StorageItem Implementation
 *
 * Written by Weather
 *
 * This is an implementation of Microsoft's OneCoreUAP binaries.
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

#include <initguid.h>
#include <propkey.h>
#include <shobjidl.h>

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

HRESULT WINAPI storage_item_Internal_CreateNew( HSTRING itemPath, IStorageItem ** result ) 
{
    FILETIME itemFileCreatedTime;
    HRESULT status; 
    LPCWSTR itemPathStr;
    HANDLE itemFile;
    WCHAR itemName[MAX_PATH];
    DWORD attributes;
    
    struct storage_item *item = NULL;

    TRACE( "iface %p, value %p\n", itemPath, result );

    itemPathStr = WindowsGetStringRawBuffer( itemPath, NULL );

    if ( PathIsURLW( itemPathStr ) )
        return E_INVALIDARG;

    if ( WindowsIsStringEmpty( itemPath ) )
    {
        status = E_INVALIDARG;
        if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_PATHISSHORT ) ) ) )
            return E_UNEXPECTED;
        return status;
    }

    attributes = GetFileAttributesW( itemPathStr );
    if ( attributes == INVALID_FILE_ATTRIBUTES ) 
    {
        status = HRESULT_FROM_WIN32( GetLastError() );
        switch ( GetLastError() )
        {
            case ERROR_FILE_NOT_FOUND:
                if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_FILENOTFOUND ), itemPathStr ) ) )
                    return E_UNEXPECTED;
                return status;
            
            case ERROR_INVALID_NAME:
            case ERROR_BAD_PATHNAME:
                if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_INVALIDPATH ), itemPathStr ) ) )
                    return E_UNEXPECTED;
                return status;

            case ERROR_FILENAME_EXCED_RANGE:
                if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_PATHTOOLONG ), itemPathStr ) ) )
                    return E_UNEXPECTED;
                return status;

            default: 
                return status;
        }
    } else 
    {    
        if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;

        item->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
        item->IStorageItem2_iface.lpVtbl = &storage_item2_vtbl;
        //File Time
        itemFile = CreateFileW( itemPathStr, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
        if (itemFile == INVALID_HANDLE_VALUE) 
        {
            status = E_ABORT;
            goto _CLEANUP;
        }

        if ( !GetFileTime( itemFile, &itemFileCreatedTime, NULL, NULL ) ) 
        {
            status = HRESULT_FROM_WIN32( GetLastError() );
            goto _CLEANUP;
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
        wcscpy( itemName, wcsrchr( itemPathStr, '\\' ) );
        WindowsCreateString( itemName + 1, wcslen( itemName + 1 ), &item->Name );

        CloseHandle( itemFile );

        status = S_OK;    
        
        *result = &item->IStorageItem_iface;
    }

_CLEANUP:
    if ( FAILED( status ) )
        if ( item )
            free( item );

    return status;
}

HRESULT WINAPI storage_item_Rename( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    BOOLEAN isFolder = TRUE;
    HRESULT status = S_OK;
    HSTRING itemPath;
    DWORD attributes;
    WCHAR newItemPath[MAX_PATH];
    WCHAR uuidName[MAX_PATH];

    IStorageFolder *reassignedFolder = NULL;
    IStorageFolder *folderToReassign = NULL;
    IStorageFile *reassignedFile = NULL;
    IStorageFile *fileToReassign = NULL;

    struct storage_item_rename_options *rename_options = (struct storage_item_rename_options *)param;

    //Parameters
    NameCollisionOption collisionOption = rename_options->option;
    HSTRING name = rename_options->name;

    TRACE( "iface %p, value %p\n", invoker, name );
    if (!name) return E_INVALIDARG;

    IStorageItem_get_Path( (IStorageItem *)invoker, &itemPath );

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
                    return E_INVALIDARG;
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

        IStorageItem_IsOfType( (IStorageItem *)invoker, StorageItemTypes_Folder, &isFolder );

        if ( !MoveFileW( WindowsGetStringRawBuffer( itemPath, NULL ), newItemPath ) )
        {
            status = HRESULT_FROM_WIN32( GetLastError() );
            if ( isFolder )
                if ( FAILED( SetLastRestrictedErrorWithMessageFormattedW( status, GetResourceW( IDS_RENAMEDELNOTALLOWED ), WindowsGetStringRawBuffer( itemPath, NULL ) ) ) )
                    return E_UNEXPECTED;
        }

        //The behavior calls for reassigning properties.
        if ( SUCCEEDED( status ) )
        {
            WindowsCreateString( newItemPath, wcslen( newItemPath ), &itemPath );
            if ( isFolder )
            {
                status = storage_folder_AssignFolder( itemPath, &reassignedFolder );
                if ( FAILED( status ) ) return status;
                IStorageItem_QueryInterface( (IStorageItem *)invoker, &IID_IStorageFolder, (void **)&folderToReassign );
                memcpy( folderToReassign, reassignedFolder, sizeof(struct storage_folder) );
                IStorageFolder_Release( reassignedFolder );
            } else
            {
                status = storage_file_AssignFile( itemPath, &reassignedFile );
                IStorageItem_QueryInterface( (IStorageItem *)invoker, &IID_IStorageFile, (void **)&fileToReassign );
                memcpy( fileToReassign, reassignedFile, sizeof(struct storage_file) );
                IStorageFile_Release( reassignedFile );
            }
        }
    }

    WindowsDeleteString( itemPath );

    return status;
}

HRESULT WINAPI storage_item_Delete( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    //The delete operation doesn't release the file/folder.
    HRESULT status = S_OK;
    HSTRING itemPath;
    DWORD attributes;

    StorageDeleteOption deleteOption = (StorageDeleteOption)param;

    TRACE( "iface %p\n", invoker );

    IStorageItem_get_Path( (IStorageItem *)invoker, &itemPath );

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

    WindowsDeleteString( itemPath );

    return status;
}

HRESULT WINAPI storage_item_GetProperties( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    FILETIME lastWrite;
    HSTRING itemPath;
    HRESULT status = S_OK;
    HANDLE file;
    DWORD fileSize;
    
    struct basic_properties *properties;

    TRACE( "iface %p, value %p\n", invoker, result );
   
    IStorageItem_get_Path( (IStorageItem *)invoker, &itemPath );

    file = CreateFileW( WindowsGetStringRawBuffer( itemPath, NULL ), GENERIC_READ, FILE_SHARE_READ, NULL,
                    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );

    if ( file == INVALID_HANDLE_VALUE )
    {
        return E_ABORT;
    }

    fileSize = GetFileSize( file, NULL );

    if ( fileSize == INVALID_FILE_SIZE )
    {
        CloseHandle( file );
        return E_UNEXPECTED;
    }

    GetFileTime(file, NULL, NULL, &lastWrite);

    if (!(properties = calloc( 1, sizeof(*properties) ))) return E_OUTOFMEMORY;

    properties->IActivationFactory_iface.lpVtbl = basic_properties_factory->lpVtbl;
    properties->IBasicProperties_iface.lpVtbl = &basic_properties_vtbl;
    properties->IStorageItemExtraProperties_iface.lpVtbl = &storage_item_extra_properties_vtbl;

    properties->DateModified.UniversalTime = FileTimeToUnixTime( &lastWrite );
    IStorageItem_get_DateCreated( (IStorageItem *)invoker, &properties->ItemDate );
    properties->size = fileSize;
    properties->ref = 1;

    properties->Item = (IStorageItem *)invoker;

    // To be fetched
    // properties->Properties = storage_item_extra_properties_CacheProperties();

    result->vt = VT_UNKNOWN;
    result->punkVal = (IUnknown *)&properties->IBasicProperties_iface;
    
    CloseHandle( file );
    WindowsDeleteString( itemPath );

    return status;

}

HRESULT WINAPI storage_item_GetType( IStorageItem * iface, StorageItemTypes * type )
{    
    HRESULT status = S_OK;
    HSTRING itemPath;
    DWORD attributes;

    TRACE( "iface %p, type %p\n", iface, type );
    
    IStorageItem_get_Path( iface, &itemPath );

    attributes = GetFileAttributesW( WindowsGetStringRawBuffer( itemPath, NULL ) );
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

    WindowsDeleteString( itemPath );
    return status;
}

HRESULT WINAPI storage_item_properties_AssignProperties( IStorageItem* iface, IStorageItemProperties ** result )
{
    SHFILEINFOW shFileInfo = { 0 };    
    HRESULT status = S_OK;
    HSTRING itemPath;
    LPWSTR pathParts[MAX_PATH];
    LPWSTR token;
    WCHAR id[ 23 + MAX_PATH ];
    INT partCount = 0;

    struct storage_item_properties *properties;

    TRACE( "iface %p, result %p\n", iface, result );

    if (!(properties = calloc( 1, sizeof(*properties) ))) return E_OUTOFMEMORY;

    properties->IStorageItemProperties_iface.lpVtbl = &storage_item_properties_vtbl;
    properties->IStorageItemPropertiesWithProvider_iface.lpVtbl = &storage_item_properties_with_provider_vtbl;

    //Item name is used for DisplayName.
    IStorageItem_get_Name( iface, &properties->DisplayName );

    IStorageItem_get_Path( iface, &itemPath );
    if ( SHGetFileInfoW( WindowsGetStringRawBuffer( itemPath, NULL ), 0, &shFileInfo, sizeof( shFileInfo ), SHGFI_TYPENAME ) ) {
        WindowsCreateString( shFileInfo.szTypeName, wcslen( shFileInfo.szTypeName ), &properties->DisplayType );
    }

    wcscpy( id, WindowsGetStringRawBuffer( itemPath, NULL ) );

    token = wcstok( id, L"\\", NULL );
    while ( token != NULL ) 
    {
        pathParts[partCount++] = token;
        token = wcstok( NULL, L"\\", NULL );
    }

    if ( partCount >= 2 ) 
        wnsprintfW( id, 23 + MAX_PATH, L"System.ItemPathDisplay:%s\\%s", pathParts[partCount - 2], pathParts[partCount - 1] );

    WindowsCreateString( id, wcslen( id ), &properties->FolderRelativeId );

    *result = &properties->IStorageItemProperties_iface;

    free( token );

    return status;
}

HRESULT WINAPI storage_item_properties_with_provider_GetProvider( IStorageItemPropertiesWithProvider *iface, IStorageProvider **value )
{
    HRESULT status = S_OK;
    HSTRING itemPath;
    PROPVARIANT prop;
    IShellItem2 *shellItem = NULL;

    IStorageItem *item = NULL;

    struct storage_provider *provider;

    TRACE( "iface %p, value %p\n", iface, value );

    IStorageItemPropertiesWithProvider_QueryInterface( iface, &IID_IStorageItem, (void **)&item );
    IStorageItem_get_Path( item, &itemPath );

    if (!(provider = calloc( 1, sizeof(*provider) ))) return E_OUTOFMEMORY;

    provider->IStorageProvider_iface.lpVtbl = &storage_provider_vtbl;

    status = SHCreateItemFromParsingName( WindowsGetStringRawBuffer( itemPath , NULL ), NULL, &IID_IShellItem2, (void **)&shellItem );
    if ( SUCCEEDED( status ) ) 
    {
        PropVariantInit( &prop );

        status = IShellItem2_GetProperty( shellItem, &PKEY_StorageProviderId, &prop );
        if ( SUCCEEDED( status ) )
            WindowsCreateString( prop.pwszVal, wcslen( prop.pwszVal ), &provider->Id );
        else
        {
            //ShellItem2_GetProperty is stubbed
            if ( status != E_NOTIMPL )
            {
                PropVariantClear( &prop );
                IShellItem2_Release( shellItem );
                IStorageItem_Release( item );
                WindowsDeleteString( itemPath );
                if ( status == E_ACCESSDENIED )
                {
                    if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_NOPROVIDERACCESS ) ) ) )
                        return E_UNEXPECTED;
                    return status;
                } else {
                    if ( FAILED( SetLastRestrictedErrorWithMessageW( status, GetResourceW( IDS_PROVIDERFAILED ) ) ) )
                        return E_UNEXPECTED;
                    return status;
                }
            }
            else
            {
                //"computer" is used instead until the functionality is implemented
                WindowsCreateString( L"computer", wcslen( L"computer" ), &provider->Id );
            }
        }
    }

    PropVariantClear( &prop );
    IShellItem2_Release( shellItem );
    IStorageItem_Release( item );
    WindowsDeleteString( itemPath );

    // "This PC" is used for Provider Name
    WindowsCreateString( L"This PC", wcslen( L"This PC" ), &provider->DisplayName );

    *value = &provider->IStorageProvider_iface;

    return status;
}
