/*
 * Written by Weather
 *
 * This is a reverse engineered implementation of Microsoft's OneCoreUAP binaries.
 *
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
#define COBJMACROS
#include "initguid.h"
#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "winstring.h"

#include "roapi.h"

#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#include "windows.foundation.h"
#define WIDL_using_Windows_Storage
#include "windows.storage.h"

#include "wine/test.h"

LPCSTR HStringToLPCSTR( HSTRING hString ) {
    UINT32 length = WindowsGetStringLen(hString);
    const wchar_t* rawBuffer = WindowsGetStringRawBuffer(hString, &length);
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, NULL, 0, NULL, NULL);
    char* multiByteStr = (char*)malloc(bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, multiByteStr, bufferSize, NULL, NULL);
    multiByteStr[bufferSize] = '\0';

    return multiByteStr;
}

struct storage_folder_async_handler
{
    IAsyncOperationCompletedHandler_StorageFolder IAsyncOperationCompletedHandler_StorageFolder_iface;
    IAsyncOperation_StorageFolder *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct storage_folder_async_handler *impl_from_IAsyncOperationCompletedHandler_StorageFolder( IAsyncOperationCompletedHandler_StorageFolder *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder_async_handler, IAsyncOperationCompletedHandler_StorageFolder_iface );
}

static HRESULT WINAPI storage_folder_async_handler_QueryInterface( IAsyncOperationCompletedHandler_StorageFolder *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_StorageFolder ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_folder_async_handler_AddRef( IAsyncOperationCompletedHandler_StorageFolder *iface )
{
    return 2;
}

static ULONG WINAPI storage_folder_async_handler_Release( IAsyncOperationCompletedHandler_StorageFolder *iface )
{
    return 1;
}

static HRESULT WINAPI storage_folder_async_handler_Invoke( IAsyncOperationCompletedHandler_StorageFolder *iface,
                                                 IAsyncOperation_StorageFolder *async, AsyncStatus status )
{
    struct storage_folder_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_StorageFolder( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_StorageFolderVtbl storage_folder_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    storage_folder_async_handler_QueryInterface,
    storage_folder_async_handler_AddRef,
    storage_folder_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<boolean> methods ***/
    storage_folder_async_handler_Invoke,
};

static struct storage_folder_async_handler default_storage_folder_async_handler = {{&storage_folder_async_handler_vtbl}};

#define check_interface( obj, iid ) check_interface_( __LINE__, obj, iid )
static void check_interface_( unsigned int line, void *obj, const IID *iid )
{
    IUnknown *iface = obj;
    IUnknown *unk;
    HRESULT hr;

    hr = IUnknown_QueryInterface( iface, iid, (void **)&unk );
    ok_(__FILE__, line)( hr == S_OK, "got hr %#lx.\n", hr );
    IUnknown_Release( unk );
}

static void test_AppDataPathsStatics(void)
{
    static const WCHAR *app_data_paths_statics_name = L"Windows.Storage.AppDataPaths";
    IAppDataPathsStatics *app_data_paths_statics;
    IAppDataPaths *app_data_paths = NULL;
    const wchar_t* wstr;
    IActivationFactory *factory;
    HSTRING str;
    HSTRING cookiesString;
    HRESULT hr;
    LONG ref;

    cookiesString = NULL;
    hr = WindowsCreateString( app_data_paths_statics_name, wcslen( app_data_paths_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( app_data_paths_statics_name ) );
        return;
    }
    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );
    
    hr = IActivationFactory_QueryInterface( factory, &IID_IAppDataPathsStatics, (void **)&app_data_paths_statics );
    
    hr = IAppDataPathsStatics_GetDefault( app_data_paths_statics, NULL );
    ok( hr == E_INVALIDARG, "got hr %#lx.\n", hr );
    hr = IAppDataPathsStatics_GetDefault( app_data_paths_statics, &app_data_paths );

    IAppDataPaths_get_Cookies( app_data_paths, &cookiesString );
    wstr = WindowsGetStringRawBuffer(cookiesString, NULL);
    wprintf(L"Cookies Path: %s\n", wstr);


    if (app_data_paths) IAppDataPaths_Release( app_data_paths );
    ref = IAppDataPathsStatics_Release( app_data_paths_statics );
    ok( ref == 2, "got ref %ld.\n", ref );
    ref = IActivationFactory_Release( factory );
    ok( ref == 1, "got ref %ld.\n", ref );
}

static void test_StorageFolder(void)
{
    static const WCHAR *storage_folder_statics_name = L"Windows.Storage.StorageFolder";
    static const WCHAR *path = L"C:\\";
    struct storage_folder_async_handler storage_folder_async_handler;
    IStorageItem *storageItem;
    IStorageFolder *storageFolderResults = NULL;
    IStorageFolderStatics *storage_folder_statics;
    IAsyncOperation_StorageFolder *storage_folder = NULL;
    IAsyncOperationCompletedHandler_StorageFolder *storage_folder_handler;
    IActivationFactory *factory;
    HSTRING str;
    HSTRING pathString;
    HSTRING Path;
    HRESULT hr;
    DWORD ret;

    WindowsCreateString( path, wcslen( path ), &pathString );

    hr = WindowsCreateString( storage_folder_statics_name, wcslen( storage_folder_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( storage_folder_statics_name ) );
        return;
    }

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );
    hr = IActivationFactory_QueryInterface( factory, &IID_IStorageFolderStatics, (void **)&storage_folder_statics );
    
    hr = IStorageFolderStatics_GetFolderFromPathAsync( storage_folder_statics, pathString, &storage_folder );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
    
    check_interface( storage_folder, &IID_IUnknown );
    check_interface( storage_folder, &IID_IInspectable );
    check_interface( storage_folder, &IID_IAgileObject );
    check_interface( storage_folder, &IID_IAsyncInfo );
    check_interface( storage_folder, &IID_IAsyncOperation_StorageFolder );

    hr = IAsyncOperation_StorageFolder_get_Completed( storage_folder, &storage_folder_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_folder_handler == NULL, "got handler %p\n", storage_folder_handler );
    
    storage_folder_async_handler = default_storage_folder_async_handler;
    storage_folder_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );
    
    hr = IAsyncOperation_StorageFolder_put_Completed( storage_folder, &storage_folder_async_handler.IAsyncOperationCompletedHandler_StorageFolder_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );
    ret = WaitForSingleObject( storage_folder_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );
    ret = CloseHandle( storage_folder_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_folder_async_handler.invoked, "handler not invoked\n" );
    ok( storage_folder_async_handler.async == storage_folder, "got async %p\n", storage_folder_async_handler.async );
    ok( storage_folder_async_handler.status == Completed || broken( storage_folder_async_handler.status == Error ), "got status %u\n", storage_folder_async_handler.status );
    hr = IAsyncOperation_StorageFolder_GetResults( storage_folder, &storageFolderResults );
    IStorageFolder_QueryInterface( storageFolderResults, &IID_IStorageItem, (void **)&storageItem);
    IStorageItem_get_Path( storageItem, &Path );
    ok( pathString == Path, "Error: Original path not returned. Path %s\n", HStringToLPCSTR(Path));
}

START_TEST(storage)
{
    HRESULT hr;

    hr = RoInitialize( RO_INIT_MULTITHREADED );
    ok( hr == S_OK, "RoInitialize failed, hr %#lx\n", hr );

    test_StorageFolder();
    //test_AppDataPathsStatics();

    RoUninitialize();
}
