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

HRESULT WINAPI storage_item_Internal_CreateNew ( HSTRING itemPath, IStorageItem * result ) 
{
    CHAR itemName[MAX_PATH];
    DWORD attributes;
    HANDLE itemFile;
    HRESULT status;
    FILETIME itemFileCreatedTime;
    struct storage_item *item;

    TRACE( "iface %p, value %p\n", itemPath, result );
    if (!result) return E_INVALIDARG;

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

        status = S_OK;
    }

    return status;
}