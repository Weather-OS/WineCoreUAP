/* WinRT Windows.Storage.StorageFile Implementation
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
#include "StorageFileInternal.h"
#include "StorageItemInternal.h"

#define BUFFER_SIZE 512

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

HRESULT WINAPI storage_file_AssignFile ( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status;
    HSTRING path;
    FILE *fileData;
    BYTE buffer[BUFFER_SIZE];
    DWORD bytesRead;
    LPWSTR pwsMimeOut;
    BOOLEAN isFile;
    char * fileExtension;
    struct storage_file *file;

    bytesRead = 0;
    pwsMimeOut = NULL;

    TRACE( "iface %p, value %p\n", invoker, result );
    
    if (!result) return E_INVALIDARG;
    if (!(file = calloc( 1, sizeof(*file) ))) return E_OUTOFMEMORY;

    file->IStorageFile_iface.lpVtbl = &storage_file_vtbl;
    file->IStorageItem_iface.lpVtbl = &storage_item_vtbl;
    file->ref = 1;

    WindowsDuplicateString( (HSTRING)param, &path );

    status = storage_item_Internal_CreateNew( path, &file->IStorageItem_iface );

    IStorageItem_IsOfType( &file->IStorageItem_iface, StorageItemTypes_File, &isFile );
    if ( !isFile )
        status = E_INVALIDARG;

    if ( SUCCEEDED( status ) )
    {
        fileExtension = strrchr( HStringToLPCSTR( path ), '.' );
        if ( fileExtension && fileExtension != HStringToLPCSTR( path ) )
        {
            WindowsCreateString( CharToLPCWSTR( fileExtension ), wcslen( CharToLPCWSTR( fileExtension ) ), &file->FileType );
        }

        fileData = fopen( HStringToLPCSTR( path ), "rb" );
        bytesRead = (DWORD)fread( buffer, 1, BUFFER_SIZE, fileData );
        fclose( fileData );

        FindMimeFromData ( NULL, WindowsGetStringRawBuffer( path, NULL ), buffer, bytesRead, NULL, 0, &pwsMimeOut, 0 );
        WindowsCreateString( pwsMimeOut, wcslen( pwsMimeOut ), &file->ContentType );

        CoTaskMemFree( pwsMimeOut );
    }

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->ppunkVal = (IUnknown **)&file->IStorageFile_iface;
    }

    return status;
}