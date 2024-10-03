/* WinRT Windows.Storage Implementation
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

#include "initguid.h"
#include "private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

HRESULT WINAPI DllGetClassObject( REFCLSID clsid, REFIID riid, void **out )
{
    FIXME( "clsid %s, riid %s, out %p stub!\n", debugstr_guid( clsid ), debugstr_guid( riid ), out );
    return CLASS_E_CLASSNOTAVAILABLE;
}

HRESULT WINAPI DllGetActivationFactory( HSTRING classid, IActivationFactory **factory )
{
    const WCHAR *buffer = WindowsGetStringRawBuffer( classid, NULL );

    TRACE( "class %s, factory %p.\n", debugstr_hstring( classid ), factory );

    *factory = NULL;

    /**
     * Windows.Storage
     */
    if (!wcscmp( buffer, RuntimeClass_Windows_Storage_AppDataPaths ))
        IActivationFactory_QueryInterface( app_data_paths_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_Storage_StorageFolder ))
        IActivationFactory_QueryInterface( storage_folder_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_Storage_StorageFile ))
        IActivationFactory_QueryInterface( storage_file_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_Storage_KnownFolders ))
        IActivationFactory_QueryInterface( known_folders_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_Storage_DownloadsFolder ))
        IActivationFactory_QueryInterface( downloads_folder_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_Storage_FileIO ))
        IActivationFactory_QueryInterface( file_io_factory, &IID_IActivationFactory, (void **)factory );

    if (!wcscmp( buffer, RuntimeClass_Windows_Storage_PathIO ))
        IActivationFactory_QueryInterface( path_io_factory, &IID_IActivationFactory, (void **)factory );

    /**
     * Windows.Storage.FileProperties
     */
    //Runtime class not meant to be registered. Please refrain from registring it.
    //if (!wcscmp( buffer, RuntimeClass_Windows_Storage_FileProperties_BasicProperties ))
    //    IActivationFactory_QueryInterface( basic_properties_factory, &IID_IActivationFactory, (void **)factory );
    
    if (*factory) return S_OK;
    return CLASS_E_CLASSNOTAVAILABLE;
}
