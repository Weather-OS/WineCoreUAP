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
#include "util.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

extern struct IStorageItemVtbl storage_item_vtbl;
extern struct IStorageFolderVtbl storage_folder_vtbl;

HRESULT WINAPI storage_item_Internal_CreateNew( HSTRING itemPath, IStorageItem * result ) 
{
    CHAR itemName[MAX_PATH];
    DWORD attributes;
    HANDLE itemFile;
    HRESULT status;
    FILETIME itemFileCreatedTime;
    struct storage_item *item;

    TRACE( "iface %p, value %p\n", itemPath, result );
    if (!result) return E_INVALIDARG;
    if (!(item = calloc( 1, sizeof(*item) ))) return E_OUTOFMEMORY;

    item = impl_from_IStorageItem( result );

    WindowsDuplicateString( itemPath, &item->Path );
    
    attributes = GetFileAttributesA( HStringToLPCSTR( itemPath ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES ) 
    {
        status = E_INVALIDARG;
    } else 
    {
        //File Time
        itemFile = CreateFileA( HStringToLPCSTR( itemPath ), GENERIC_READ, FILE_SHARE_READ, NULL,
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
        strcpy( itemName, strrchr( HStringToLPCSTR( itemPath ), '\\' ) );
        WindowsCreateString( CharToLPCWSTR( itemName + 1 ), strlen(itemName), &item->Name );

        CloseHandle( itemFile );

        status = S_OK;
    }

    return status;
}

HRESULT WINAPI storage_item_Rename( IStorageItem * iface, NameCollisionOption collisionOption, HSTRING name )
{
    DWORD attributes;
    HRESULT status = S_OK;
    HSTRING itemPath;
    struct storage_item *item;
    char newItemPath[MAX_PATH];
    char uuidName[MAX_PATH];

    TRACE( "iface %p, value %p\n", iface, name );
    if (!name) return E_INVALIDARG;

    item = impl_from_IStorageItem( iface );
    WindowsDuplicateString( item->Path, &itemPath );

    strcpy( newItemPath, HStringToLPCSTR( itemPath ) );
    *strrchr( newItemPath, '\\' ) = '\0';

    //Perform rename
    attributes = GetFileAttributesA( HStringToLPCSTR( itemPath ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES ) 
    {
        status = E_INVALIDARG;
    } else 
    {
        switch (collisionOption)
        {
            case NameCollisionOption_FailIfExists:
                PathAppendA( newItemPath, HStringToLPCSTR( name ) );

                attributes = GetFileAttributesA( newItemPath );
                if ( attributes != INVALID_FILE_ATTRIBUTES ) 
                    status = E_INVALIDARG;
                else
                    status = S_OK;
                break;

            case NameCollisionOption_GenerateUniqueName:
                GenerateUniqueFileName( uuidName, sizeof(uuidName) );
                PathAppendA( newItemPath, uuidName );

                attributes = GetFileAttributesA( newItemPath );
                if ( attributes != INVALID_FILE_ATTRIBUTES ) 
                    return E_ABORT;
                else
                    status = S_OK;
                break;

            case NameCollisionOption_ReplaceExisting:
                PathAppendA( newItemPath, HStringToLPCSTR( name ) );

                attributes = GetFileAttributesA( newItemPath );
                if ( attributes != INVALID_FILE_ATTRIBUTES ) 
                {
                    if ( !DeleteFileA( newItemPath ) )
                    {
                        return E_ABORT;
                    }
                }
                else
                    status = S_OK;
                break;
        }
    }

    if ( !MoveFileA( HStringToLPCSTR( item->Path ), newItemPath ) )
    {
        printf("Failed to move file %s to %s. Error code: %lu\n", HStringToLPCSTR( item->Path ), newItemPath, GetLastError() );
        status = E_ABORT;
    }

    if ( SUCCEEDED( status ) )
    {
        WindowsCreateString( CharToLPCWSTR( newItemPath ), wcslen( CharToLPCWSTR( newItemPath ) ), &item->Path );
        WindowsDuplicateString( name, &item->Name );
    }
    return status;
}

HRESULT WINAPI storage_item_Delete( IStorageItem * iface, StorageDeleteOption deleteOption )
{
    DWORD attributes;
    HRESULT status = S_OK;
    HSTRING itemPath;
    struct storage_item *item;

    TRACE( "iface %p\n", iface );

    item = impl_from_IStorageItem( iface );
    WindowsDuplicateString( item->Path, &itemPath );

    //Perform delete
    attributes = GetFileAttributesA( HStringToLPCSTR( itemPath ) );
    if ( attributes == INVALID_FILE_ATTRIBUTES ) 
    {
        status = E_INVALIDARG;
    } else 
    {
        switch (deleteOption)
        {
            case StorageDeleteOption_Default:
            case StorageDeleteOption_PermanentDelete:
                if ( !DeleteFileA( HStringToLPCSTR( itemPath ) ) )
                {
                    status = E_ABORT;
                }
        }
    }

    if ( SUCCEEDED( status ) )
    {
        free( item );
    }
    return status;
}