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
#include <string.h>

#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "shlwapi.h"

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
    LPSTR multiByteStr = (LPSTR)malloc(bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, rawBuffer, length, multiByteStr, bufferSize, NULL, NULL);
    multiByteStr[bufferSize] = '\0';

    return multiByteStr;
}

/**
 * IAsyncOperationCompletedHandler_StorageFolder
 */

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
    /*** IAsyncOperationCompletedHandler<StorageFolder> methods ***/
    storage_folder_async_handler_Invoke,
};

static struct storage_folder_async_handler default_storage_folder_async_handler = {{&storage_folder_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_StorageFile
 */

struct storage_file_async_handler
{
    IAsyncOperationCompletedHandler_StorageFile IAsyncOperationCompletedHandler_StorageFile_iface;
    IAsyncOperation_StorageFile *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct storage_file_async_handler *impl_from_IAsyncOperationCompletedHandler_StorageFile( IAsyncOperationCompletedHandler_StorageFile *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file_async_handler, IAsyncOperationCompletedHandler_StorageFile_iface );
}

static HRESULT WINAPI storage_file_async_handler_QueryInterface( IAsyncOperationCompletedHandler_StorageFile *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_StorageFile ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_file_async_handler_AddRef( IAsyncOperationCompletedHandler_StorageFile *iface )
{
    return 2;
}

static ULONG WINAPI storage_file_async_handler_Release( IAsyncOperationCompletedHandler_StorageFile *iface )
{
    return 1;
}

static HRESULT WINAPI storage_file_async_handler_Invoke( IAsyncOperationCompletedHandler_StorageFile *iface,
                                                 IAsyncOperation_StorageFile *async, AsyncStatus status )
{
    struct storage_file_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_StorageFile( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_StorageFileVtbl storage_file_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    storage_file_async_handler_QueryInterface,
    storage_file_async_handler_AddRef,
    storage_file_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<StorageFile> methods ***/
    storage_file_async_handler_Invoke,
};

static struct storage_file_async_handler default_storage_file_async_handler = {{&storage_file_async_handler_vtbl}};


/**
 * IAsyncOperationCompletedHandler_IStorageItem
 */

struct storage_item_async_handler
{
    IAsyncOperationCompletedHandler_IStorageItem IAsyncOperationCompletedHandler_IStorageItem_iface;
    IAsyncOperation_IStorageItem *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct storage_item_async_handler *impl_from_IAsyncOperationCompletedHandler_IStorageItem( IAsyncOperationCompletedHandler_IStorageItem *iface )
{
    return CONTAINING_RECORD( iface, struct storage_item_async_handler, IAsyncOperationCompletedHandler_IStorageItem_iface );
}

static HRESULT WINAPI storage_item_async_handler_QueryInterface( IAsyncOperationCompletedHandler_IStorageItem *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_IStorageItem ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_item_async_handler_AddRef( IAsyncOperationCompletedHandler_IStorageItem *iface )
{
    return 2;
}

static ULONG WINAPI storage_item_async_handler_Release( IAsyncOperationCompletedHandler_IStorageItem *iface )
{
    return 1;
}

static HRESULT WINAPI storage_item_async_handler_Invoke( IAsyncOperationCompletedHandler_IStorageItem *iface,
                                                 IAsyncOperation_IStorageItem *async, AsyncStatus status )
{
    struct storage_item_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_IStorageItem( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_IStorageItemVtbl storage_item_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    storage_item_async_handler_QueryInterface,
    storage_item_async_handler_AddRef,
    storage_item_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<IStorageItem> methods ***/
    storage_item_async_handler_Invoke,
};

static struct storage_item_async_handler default_storage_item_async_handler = {{&storage_item_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_IVectorView_IStorageItem
 */

struct storage_item_vector_view_async_handler
{
    IAsyncOperationCompletedHandler_IVectorView_IStorageItem IAsyncOperationCompletedHandler_IVectorView_IStorageItem_iface;
    IAsyncOperation_IVectorView_IStorageItem *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct storage_item_vector_view_async_handler *impl_from_IAsyncOperationCompletedHandler_IVectorView_IStorageItem( IAsyncOperationCompletedHandler_IVectorView_IStorageItem *iface )
{
    return CONTAINING_RECORD( iface, struct storage_item_vector_view_async_handler, IAsyncOperationCompletedHandler_IVectorView_IStorageItem_iface );
}

static HRESULT WINAPI storage_item_vector_view_async_handler_QueryInterface( IAsyncOperationCompletedHandler_IVectorView_IStorageItem *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_IVectorView_IStorageItem ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_item_vector_view_async_handler_AddRef( IAsyncOperationCompletedHandler_IVectorView_IStorageItem *iface )
{
    return 2;
}

static ULONG WINAPI storage_item_vector_view_async_handler_Release( IAsyncOperationCompletedHandler_IVectorView_IStorageItem *iface )
{
    return 1;
}

static HRESULT WINAPI storage_item_vector_view_async_handler_Invoke( IAsyncOperationCompletedHandler_IVectorView_IStorageItem *iface,
                                                 IAsyncOperation_IVectorView_IStorageItem *async, AsyncStatus status )
{
    struct storage_item_vector_view_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_IVectorView_IStorageItem( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_IVectorView_IStorageItemVtbl storage_item_vector_view_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    storage_item_vector_view_async_handler_QueryInterface,
    storage_item_vector_view_async_handler_AddRef,
    storage_item_vector_view_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<IVectorView<IStorageItem *>> methods ***/
    storage_item_vector_view_async_handler_Invoke,
};

static struct storage_item_vector_view_async_handler default_storage_item_vector_view_async_handler = {{&storage_item_vector_view_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_IVectorView_StorageFolder
 */

struct storage_folder_vector_view_async_handler
{
    IAsyncOperationCompletedHandler_IVectorView_StorageFolder IAsyncOperationCompletedHandler_IVectorView_StorageFolder_iface;
    IAsyncOperation_IVectorView_StorageFolder *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct storage_folder_vector_view_async_handler *impl_from_IAsyncOperationCompletedHandler_IVectorView_StorageFolder( IAsyncOperationCompletedHandler_IVectorView_StorageFolder *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder_vector_view_async_handler, IAsyncOperationCompletedHandler_IVectorView_StorageFolder_iface );
}

static HRESULT WINAPI storage_folder_vector_view_async_handler_QueryInterface( IAsyncOperationCompletedHandler_IVectorView_StorageFolder *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_IVectorView_StorageFolder ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_folder_vector_view_async_handler_AddRef( IAsyncOperationCompletedHandler_IVectorView_StorageFolder *iface )
{
    return 2;
}

static ULONG WINAPI storage_folder_vector_view_async_handler_Release( IAsyncOperationCompletedHandler_IVectorView_StorageFolder *iface )
{
    return 1;
}

static HRESULT WINAPI storage_folder_vector_view_async_handler_Invoke( IAsyncOperationCompletedHandler_IVectorView_StorageFolder *iface,
                                                 IAsyncOperation_IVectorView_StorageFolder *async, AsyncStatus status )
{
    struct storage_folder_vector_view_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_IVectorView_StorageFolder( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_IVectorView_StorageFolderVtbl storage_folder_vector_view_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    storage_folder_vector_view_async_handler_QueryInterface,
    storage_folder_vector_view_async_handler_AddRef,
    storage_folder_vector_view_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<IVectorView<StorageFolder *>> methods ***/
    storage_folder_vector_view_async_handler_Invoke,
};

static struct storage_folder_vector_view_async_handler default_storage_folder_vector_view_async_handler = {{&storage_folder_vector_view_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_IVectorView_StorageFile
 */

struct storage_file_vector_view_async_handler
{
    IAsyncOperationCompletedHandler_IVectorView_StorageFile IAsyncOperationCompletedHandler_IVectorView_StorageFile_iface;
    IAsyncOperation_IVectorView_StorageFile *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct storage_file_vector_view_async_handler *impl_from_IAsyncOperationCompletedHandler_IVectorView_StorageFile( IAsyncOperationCompletedHandler_IVectorView_StorageFile *iface )
{
    return CONTAINING_RECORD( iface, struct storage_file_vector_view_async_handler, IAsyncOperationCompletedHandler_IVectorView_StorageFile_iface );
}

static HRESULT WINAPI storage_file_vector_view_async_handler_QueryInterface( IAsyncOperationCompletedHandler_IVectorView_StorageFile *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_IVectorView_StorageFile ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_file_vector_view_async_handler_AddRef( IAsyncOperationCompletedHandler_IVectorView_StorageFile *iface )
{
    return 2;
}

static ULONG WINAPI storage_file_vector_view_async_handler_Release( IAsyncOperationCompletedHandler_IVectorView_StorageFile *iface )
{
    return 1;
}

static HRESULT WINAPI storage_file_vector_view_async_handler_Invoke( IAsyncOperationCompletedHandler_IVectorView_StorageFile *iface,
                                                 IAsyncOperation_IVectorView_StorageFile *async, AsyncStatus status )
{
    struct storage_file_vector_view_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_IVectorView_StorageFile( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_IVectorView_StorageFileVtbl storage_file_vector_view_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    storage_file_vector_view_async_handler_QueryInterface,
    storage_file_vector_view_async_handler_AddRef,
    storage_file_vector_view_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<IVectorView<StorageFile *>> methods ***/
    storage_file_vector_view_async_handler_Invoke,
};

static struct storage_file_vector_view_async_handler default_storage_file_vector_view_async_handler = {{&storage_file_vector_view_async_handler_vtbl}};


//************************************************************************ */

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

static void test_AppDataPathsStatics(const wchar_t** pathStr)
{
    static const WCHAR *app_data_paths_statics_name = L"Windows.Storage.AppDataPaths";
    IAppDataPathsStatics *app_data_paths_statics;
    IAppDataPaths *app_data_paths = NULL;
    const wchar_t* wstr;
    IActivationFactory *factory;
    HSTRING str;
    HSTRING localAppDataPath;
    HRESULT hr;
    LONG ref;

    localAppDataPath = NULL;
    hr = WindowsCreateString( app_data_paths_statics_name, wcslen( app_data_paths_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( app_data_paths_statics_name ) );
    }
    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );
    
    hr = IActivationFactory_QueryInterface( factory, &IID_IAppDataPathsStatics, (void **)&app_data_paths_statics );
    
    hr = IAppDataPathsStatics_GetDefault( app_data_paths_statics, NULL );
    ok( hr == E_INVALIDARG, "got hr %#lx.\n", hr );
    hr = IAppDataPathsStatics_GetDefault( app_data_paths_statics, &app_data_paths );

    IAppDataPaths_get_LocalAppData( app_data_paths, &localAppDataPath );
    wstr = WindowsGetStringRawBuffer(localAppDataPath, NULL);
    printf("Path responded is: ");
    wprintf(wstr);
    printf("\n");

    if (app_data_paths) IAppDataPaths_Release( app_data_paths );
    ref = IAppDataPathsStatics_Release( app_data_paths_statics );
    ok( ref == 2, "got ref %ld.\n", ref );
    ref = IActivationFactory_Release( factory );
    ok( ref == 1, "got ref %ld.\n", ref );
    *pathStr = wstr;
}

static void test_StorageFolder( const wchar_t* path, IStorageItem **item )
{
    //This assumes test_AppDataPathsStatics passes every test.
    static const WCHAR *storage_folder_statics_name = L"Windows.Storage.StorageFolder";
    static const WCHAR *name = L"Temp";
    static const WCHAR *fileName = L"TempFile";
    struct storage_folder_async_handler storage_folder_async_handler;
    struct storage_item_async_handler storage_item_async_handler;
    struct storage_file_async_handler storage_file_async_handler;
    struct storage_item_vector_view_async_handler storage_item_vector_view_async_handler;
    struct storage_folder_vector_view_async_handler storage_folder_vector_view_async_handler;
    struct storage_file_vector_view_async_handler storage_file_vector_view_async_handler;
    IStorageItem *storageItem;
    IStorageItem *storageItemResults;
    IStorageItem *storageItemResults2;
    IStorageItem *storageItemResults3;
    IStorageItem *storageItemResults4;
    IStorageItem *storageItemResults5;
    IStorageItem *storageItemResults6;
    IStorageItem *storageItemResults7;
    IStorageFile *storageFileResults;
    IStorageFile *storageFileResults2;
    IStorageFile *storageFileResults3;
    IStorageFolder *storageFolderResults;
    IStorageFolder *storageFolderResults2;
    IStorageFolder *storageFolderResults3;
    IStorageFolder *storageFolderResults4;
    IStorageFolderStatics *storage_folder_statics;
    IVectorView_IStorageItem *storageItemVectorResults;
    IVectorView_StorageFolder *storageFolderVectorResults;
    IVectorView_StorageFile *storageFileVectorResults;
    IAsyncOperation_StorageFile *storageFileOperation;
    IAsyncOperation_StorageFolder *storage_folder;
    IAsyncOperation_StorageFolder *storageFolderOperation;
    IAsyncOperation_IStorageItem *storageItemOperation;
    IAsyncOperation_IVectorView_IStorageItem *storageItemVectorOperation;
    IAsyncOperation_IVectorView_StorageFolder *storageFolderVectorOperation;
    IAsyncOperation_IVectorView_StorageFile *storageFileVectorOperation;
    IAsyncOperationCompletedHandler_StorageFolder *storage_folder_handler;
    IAsyncOperationCompletedHandler_IStorageItem *storage_item_handler;
    IAsyncOperationCompletedHandler_StorageFile *storage_file_handler;
    IAsyncOperationCompletedHandler_IVectorView_IStorageItem *storage_item_vector_view_handler;
    IAsyncOperationCompletedHandler_IVectorView_StorageFolder *storage_folder_vector_view_handler;
    IAsyncOperationCompletedHandler_IVectorView_StorageFile *storage_file_vector_view_handler;
    IActivationFactory *factory;
    HSTRING str;
    HSTRING pathString;
    HSTRING nameString;
    HSTRING fileString;
    HSTRING Path;
    HSTRING SecondPath;
    HSTRING SecondName;
    HSTRING ThirdPath;
    HSTRING ThirdName;
    HSTRING FourthPath;
    HSTRING FourthName;
    HSTRING FifthPath; 
    HSTRING FifthName;
    HSTRING SixthPath;
    HSTRING SixthName;
    HSTRING SeventhPath;
    HSTRING SeventhName;
    HSTRING EightthPath;
    HSTRING EightthName;
    HSTRING NinethPath;
    HSTRING NinethName;
    HRESULT hr;
    DWORD ret;
    CHAR pathtest[MAX_PATH];

    WindowsCreateString( path, wcslen( path ), &pathString );
    WindowsCreateString( name, wcslen( name ), &nameString );
    WindowsCreateString( fileName, wcslen( fileName ), &fileString );

    hr = WindowsCreateString( storage_folder_statics_name, wcslen( storage_folder_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( storage_folder_statics_name ) );
    }

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );
    hr = IActivationFactory_QueryInterface( factory, &IID_IStorageFolderStatics, (void **)&storage_folder_statics );
    
    /**
     * IStorageFolderStatics_GetFolderFromPathAsync
    */

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
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFolder_QueryInterface( storageFolderResults, &IID_IStorageItem, (void **)&storageItem);
    IStorageItem_get_Path( storageItem, &Path );
    ok( pathString == Path, "Error: Original path not returned. Path %s\n", HStringToLPCSTR(Path));

    /**
     * IStorageFolder_CreateFolderAsync
    */

    hr = IStorageFolder_CreateFolderAsync( storageFolderResults, nameString, CreationCollisionOption_ReplaceExisting, &storageFolderOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageFolderOperation, &IID_IUnknown );
    check_interface( storageFolderOperation, &IID_IInspectable );
    check_interface( storageFolderOperation, &IID_IAgileObject );
    check_interface( storageFolderOperation, &IID_IAsyncInfo );
    check_interface( storageFolderOperation, &IID_IAsyncOperation_StorageFolder );
    hr = IAsyncOperation_StorageFolder_get_Completed( storageFolderOperation, &storage_folder_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_folder_handler == NULL, "got handler %p\n", storage_folder_handler );

    storage_folder_async_handler = default_storage_folder_async_handler;
    storage_folder_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_StorageFolder_put_Completed( storageFolderOperation, &storage_folder_async_handler.IAsyncOperationCompletedHandler_StorageFolder_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );
   
    ret = WaitForSingleObject( storage_folder_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );
    
    ret = CloseHandle( storage_folder_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_folder_async_handler.invoked, "handler not invoked\n" );
    ok( storage_folder_async_handler.async == storageFolderOperation, "got async %p\n", storage_folder_async_handler.async );
    ok( storage_folder_async_handler.status == Completed || broken( storage_folder_async_handler.status == Error ), "got status %u\n", storage_folder_async_handler.status );
    
    hr = IAsyncOperation_StorageFolder_GetResults( storageFolderOperation, &storageFolderResults2 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
    
    IStorageFolder_QueryInterface( storageFolderResults2, &IID_IStorageItem, (void **)&storageItemResults);
    
    IStorageItem_get_Path( storageItemResults, &SecondPath );
    IStorageItem_get_Name( storageItemResults, &SecondName );
    
    strcpy(pathtest, HStringToLPCSTR(Path));
    strcat(pathtest, "\\Temp");

    ok( !strcmp(HStringToLPCSTR(SecondPath), pathtest), "Error: Original path not returned. SecondPath %s, pathtest %s\n", HStringToLPCSTR(SecondPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(SecondName), "Temp"), "Error: Original name not returned. SecondName %s, name %s\n", HStringToLPCSTR(SecondName), "Test");

    /**
     * IStorageFolder_GetItemAsync
    */

    hr = IStorageFolder_GetItemAsync( storageFolderResults, nameString, &storageItemOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageItemOperation, &IID_IUnknown );
    check_interface( storageItemOperation, &IID_IInspectable );
    check_interface( storageItemOperation, &IID_IAgileObject );
    check_interface( storageItemOperation, &IID_IAsyncInfo );
    check_interface( storageItemOperation, &IID_IAsyncOperation_IStorageItem );

    hr = IAsyncOperation_IStorageItem_get_Completed( storageItemOperation, &storage_item_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_item_handler == NULL, "got handler %p\n", storage_item_handler );

    storage_item_async_handler = default_storage_item_async_handler;
    storage_item_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_IStorageItem_put_Completed( storageItemOperation, &storage_item_async_handler.IAsyncOperationCompletedHandler_IStorageItem_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( storage_item_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( storage_item_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_item_async_handler.invoked, "handler not invoked\n" );
    ok( storage_item_async_handler.async == storageItemOperation, "got async %p\n", storage_item_async_handler.async );
    ok( storage_item_async_handler.status == Completed || broken( storage_item_async_handler.status == Error ), "got status %u\n", storage_item_async_handler.status );

    hr = IAsyncOperation_IStorageItem_GetResults( storageItemOperation, &storageItemResults2 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageItem_get_Path( storageItemResults2, &ThirdPath );
    IStorageItem_get_Name( storageItemResults2, &ThirdName );

    ok( !strcmp(HStringToLPCSTR(ThirdPath), pathtest), "Error: Original path not returned. ThirdPath %s, pathtest %s\n", HStringToLPCSTR(ThirdPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(ThirdName), "Temp"), "Error: Original name not returned. ThirdName %s, name %s\n", HStringToLPCSTR(ThirdName), "Test");

    /**
     * IStorageFolder_GetFolderAsync
    */

    hr = IStorageFolder_GetFolderAsync( storageFolderResults, nameString, &storageFolderOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageFolderOperation, &IID_IUnknown );
    check_interface( storageFolderOperation, &IID_IInspectable );
    check_interface( storageFolderOperation, &IID_IAgileObject );
    check_interface( storageFolderOperation, &IID_IAsyncInfo );
    check_interface( storageFolderOperation, &IID_IAsyncOperation_StorageFolder );

    hr = IAsyncOperation_StorageFolder_get_Completed( storageFolderOperation, &storage_folder_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_folder_handler == NULL, "got handler %p\n", storage_folder_handler );

    storage_folder_async_handler = default_storage_folder_async_handler;
    storage_folder_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_StorageFolder_put_Completed( storageFolderOperation, &storage_folder_async_handler.IAsyncOperationCompletedHandler_StorageFolder_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( storage_folder_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( storage_folder_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_folder_async_handler.invoked, "handler not invoked\n" );
    ok( storage_folder_async_handler.async == storageFolderOperation, "got async %p\n", storage_folder_async_handler.async );
    ok( storage_folder_async_handler.status == Completed || broken( storage_folder_async_handler.status == Error ), "got status %u\n", storage_folder_async_handler.status );

    hr = IAsyncOperation_StorageFolder_GetResults( storageFolderOperation, &storageFolderResults3 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFolder_QueryInterface( storageFolderResults3, &IID_IStorageItem, (void **)&storageItemResults);

    IStorageItem_get_Path( storageItemResults, &FourthPath );
    IStorageItem_get_Name( storageItemResults, &FourthName );

    ok( !strcmp(HStringToLPCSTR(FourthPath), pathtest), "Error: Original path not returned. FourthPath %s, pathtest %s\n", HStringToLPCSTR(FourthPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(FourthName), "Temp"), "Error: Original name not returned. FourthName %s, name %s\n", HStringToLPCSTR(FourthName), "Test");

    /**
     * IStorageFolder_GetItemsAsyncOverloadDefaultStartAndCount
    */

    hr = IStorageFolder_GetItemsAsyncOverloadDefaultStartAndCount( storageFolderResults, &storageItemVectorOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageItemVectorOperation, &IID_IUnknown );
    check_interface( storageItemVectorOperation, &IID_IInspectable );
    check_interface( storageItemVectorOperation, &IID_IAgileObject );
    check_interface( storageItemVectorOperation, &IID_IAsyncInfo );
    check_interface( storageItemVectorOperation, &IID_IAsyncOperation_IVectorView_IStorageItem );

    hr = IAsyncOperation_IVectorView_IStorageItem_get_Completed( storageItemVectorOperation, &storage_item_vector_view_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_item_vector_view_handler == NULL, "got handler %p\n", storage_item_vector_view_handler );

    storage_item_vector_view_async_handler = default_storage_item_vector_view_async_handler;
    storage_item_vector_view_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_IVectorView_IStorageItem_put_Completed( storageItemVectorOperation, &storage_item_vector_view_async_handler.IAsyncOperationCompletedHandler_IVectorView_IStorageItem_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( storage_item_vector_view_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( storage_item_vector_view_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_item_vector_view_async_handler.invoked, "handler not invoked\n" );
    ok( storage_item_vector_view_async_handler.async == storageItemVectorOperation, "got async %p\n", storage_item_vector_view_async_handler.async );
    ok( storage_item_vector_view_async_handler.status == Completed || broken( storage_item_vector_view_async_handler.status == Error ), "got status %u\n", storage_item_vector_view_async_handler.status );    

    hr = IAsyncOperation_IVectorView_IStorageItem_GetResults( storageItemVectorOperation, &storageItemVectorResults );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IVectorView_IStorageItem_GetAt( storageItemVectorResults, 0, &storageItemResults3 );

    IStorageItem_get_Path( storageItemResults3, &FifthPath );
    IStorageItem_get_Name( storageItemResults3, &FifthName );
    
    ok( !strcmp(HStringToLPCSTR(FifthPath), pathtest), "Error: Original path not returned. FifthPath %s, pathtest %s\n", HStringToLPCSTR(FifthPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(FifthName), "Temp"), "Error: Original name not returned. FifthName %s, name %s\n", HStringToLPCSTR(FifthName), "Test");

    *item = storageItemResults3;

    /**
     * IStorageFolder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount
    */

    hr = IStorageFolder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount( storageFolderResults, &storageFolderVectorOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageFolderVectorOperation, &IID_IUnknown );
    check_interface( storageFolderVectorOperation, &IID_IInspectable );
    check_interface( storageFolderVectorOperation, &IID_IAgileObject );
    check_interface( storageFolderVectorOperation, &IID_IAsyncInfo );
    check_interface( storageFolderVectorOperation, &IID_IAsyncOperation_IVectorView_StorageFolder );

    hr = IAsyncOperation_IVectorView_StorageFolder_get_Completed( storageFolderVectorOperation, &storage_folder_vector_view_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_folder_vector_view_handler == NULL, "got handler %p\n", storage_folder_vector_view_handler );

    storage_folder_vector_view_async_handler = default_storage_folder_vector_view_async_handler;
    storage_folder_vector_view_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_IVectorView_StorageFolder_put_Completed( storageFolderVectorOperation, &storage_folder_vector_view_async_handler.IAsyncOperationCompletedHandler_IVectorView_StorageFolder_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( storage_folder_vector_view_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( storage_folder_vector_view_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_folder_vector_view_async_handler.invoked, "handler not invoked\n" );
    ok( storage_folder_vector_view_async_handler.async == storageFolderVectorOperation, "got async %p\n", storage_folder_vector_view_async_handler.async );
    ok( storage_folder_vector_view_async_handler.status == Completed || broken( storage_folder_vector_view_async_handler.status == Error ), "got status %u\n", storage_folder_vector_view_async_handler.status );    

    hr = IAsyncOperation_IVectorView_StorageFolder_GetResults( storageFolderVectorOperation, &storageFolderVectorResults );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IVectorView_StorageFolder_GetAt( storageFolderVectorResults, 0, &storageFolderResults4 );

    IStorageFolder_QueryInterface( storageFolderResults4, &IID_IStorageItem, (void **)&storageItemResults4 );

    IStorageItem_get_Path( storageItemResults4, &SixthPath );
    IStorageItem_get_Name( storageItemResults4, &SixthName );
    
    ok( !strcmp(HStringToLPCSTR(SixthPath), pathtest), "Error: Original path not returned. SixthPath %s, pathtest %s\n", HStringToLPCSTR(SixthPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(SixthName), "Temp"), "Error: Original name not returned. SixthName %s, name %s\n", HStringToLPCSTR(SixthName), "Test");

    /**
     * IStorageFolder_CreateFileAsync
     */

    hr = IStorageFolder_CreateFileAsync( storageFolderResults4, fileString, CreationCollisionOption_ReplaceExisting, &storageFileOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageFileOperation, &IID_IUnknown );
    check_interface( storageFileOperation, &IID_IInspectable );
    check_interface( storageFileOperation, &IID_IAgileObject );
    check_interface( storageFileOperation, &IID_IAsyncInfo );
    check_interface( storageFileOperation, &IID_IAsyncOperation_StorageFile );
    hr = IAsyncOperation_StorageFile_get_Completed( storageFileOperation, &storage_file_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_file_handler == NULL, "got handler %p\n", storage_file_handler );

    storage_file_async_handler = default_storage_file_async_handler;
    storage_file_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_StorageFile_put_Completed( storageFileOperation, &storage_file_async_handler.IAsyncOperationCompletedHandler_StorageFile_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( storage_file_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );
    
    ret = CloseHandle( storage_file_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_file_async_handler.invoked, "handler not invoked\n" );
    ok( storage_file_async_handler.async == storageFileOperation, "got async %p\n", storage_file_async_handler.async );
    ok( storage_file_async_handler.status == Completed || broken( storage_file_async_handler.status == Error ), "got status %u\n", storage_file_async_handler.status );

    hr = IAsyncOperation_StorageFile_GetResults( storageFileOperation, &storageFileResults );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFile_QueryInterface( storageFileResults, &IID_IStorageItem, (void **)&storageItemResults5 );
    
    IStorageItem_get_Name( storageItemResults5, &SeventhName );
    IStorageItem_get_Path( storageItemResults5, &SeventhPath );
    
    strcpy(pathtest, HStringToLPCSTR(SixthPath));
    strcat(pathtest, "\\TempFile");
    
    ok( !strcmp(HStringToLPCSTR(SeventhPath), pathtest), "Error: Original path not returned. SeventhPath %s, pathtest %s\n", HStringToLPCSTR(SeventhPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(SeventhName), "TempFile"), "Error: Original name not returned. SeventhName %s, name %s\n", HStringToLPCSTR(SeventhName), "TempFile");

    /**
     * IStorageFolder_GetFileAsync
     */

    hr = IStorageFolder_GetFileAsync( storageFolderResults4, fileString, &storageFileOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageFileOperation, &IID_IUnknown );
    check_interface( storageFileOperation, &IID_IInspectable );
    check_interface( storageFileOperation, &IID_IAgileObject );
    check_interface( storageFileOperation, &IID_IAsyncInfo );
    check_interface( storageFileOperation, &IID_IAsyncOperation_StorageFile );

    hr = IAsyncOperation_StorageFile_get_Completed( storageFileOperation, &storage_file_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_file_handler == NULL, "got handler %p\n", storage_file_handler );

    storage_file_async_handler = default_storage_file_async_handler;
    storage_file_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_StorageFile_put_Completed( storageFileOperation, &storage_file_async_handler.IAsyncOperationCompletedHandler_StorageFile_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( storage_file_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( storage_file_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_file_async_handler.invoked, "handler not invoked\n" );
    ok( storage_file_async_handler.async == storageFileOperation, "got async %p\n", storage_file_async_handler.async );
    ok( storage_item_async_handler.status == Completed || broken( storage_file_async_handler.status == Error ), "got status %u\n", storage_file_async_handler.status );

    hr = IAsyncOperation_StorageFile_GetResults( storageFileOperation, &storageFileResults2 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFile_QueryInterface( storageFileResults2, &IID_IStorageItem, (void **)&storageItemResults6 );

    IStorageItem_get_Path( storageItemResults6, &EightthPath );
    IStorageItem_get_Name( storageItemResults6, &EightthName );

    ok( !strcmp(HStringToLPCSTR(EightthPath), pathtest), "Error: Original path not returned. EightthPath %s, pathtest %s\n", HStringToLPCSTR(EightthPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(EightthName), "TempFile"), "Error: Original name not returned. EightthName %s, name %s\n", HStringToLPCSTR(EightthName), "TempFile");

    /**
     * IStorageFolder_GetFilesAsyncOverloadDefaultOptionsStartAndCount
    */

    hr = IStorageFolder_GetFilesAsyncOverloadDefaultOptionsStartAndCount( storageFolderResults4, &storageFileVectorOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageFileVectorOperation, &IID_IUnknown );
    check_interface( storageFileVectorOperation, &IID_IInspectable );
    check_interface( storageFileVectorOperation, &IID_IAgileObject );
    check_interface( storageFileVectorOperation, &IID_IAsyncInfo );
    check_interface( storageFileVectorOperation, &IID_IAsyncOperation_IVectorView_StorageFile );

    hr = IAsyncOperation_IVectorView_StorageFile_get_Completed( storageFileVectorOperation, &storage_file_vector_view_handler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storage_file_vector_view_handler == NULL, "got handler %p\n", storage_file_vector_view_handler );

    storage_file_vector_view_async_handler = default_storage_file_vector_view_async_handler;
    storage_file_vector_view_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_IVectorView_StorageFile_put_Completed( storageFileVectorOperation, &storage_file_vector_view_async_handler.IAsyncOperationCompletedHandler_IVectorView_StorageFile_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( storage_file_vector_view_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( storage_file_vector_view_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_file_vector_view_async_handler.invoked, "handler not invoked\n" );
    ok( storage_file_vector_view_async_handler.async == storageFileVectorOperation, "got async %p\n", storage_file_vector_view_async_handler.async );
    ok( storage_file_vector_view_async_handler.status == Completed || broken( storage_file_vector_view_async_handler.status == Error ), "got status %u\n", storage_file_vector_view_async_handler.status );

    hr = IAsyncOperation_IVectorView_StorageFile_GetResults( storageFileVectorOperation, &storageFileVectorResults );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IVectorView_StorageFile_GetAt( storageFileVectorResults, 0, &storageFileResults3 );

    IStorageFile_QueryInterface( storageFileResults3, &IID_IStorageItem, (void **)&storageItemResults7 );

    IStorageItem_get_Path( storageItemResults7, &NinethPath );
    IStorageItem_get_Name( storageItemResults7, &NinethName );
    
    ok( !strcmp(HStringToLPCSTR(NinethPath), pathtest), "Error: Original path not returned. NinethPath %s, pathtest %s\n", HStringToLPCSTR(NinethPath), pathtest);
    ok( !strcmp(HStringToLPCSTR(NinethName), "TempFile"), "Error: Original name not returned. NinethName %s, name %s\n", HStringToLPCSTR(NinethName), "TempFile");
}

static void test_StorageItem( IStorageItem *customItem )
{
    static const WCHAR *name = L"TempTest";
    BOOLEAN isFolder;
    HSTRING renameName;
    HSTRING resultName;
    HRESULT hr;
    IAsyncAction *tempAction;

    WindowsCreateString( name, wcslen(name), &renameName );

    hr = IStorageItem_RenameAsyncOverloadDefaultOptions( customItem, renameName, &tempAction );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageItem_get_Name( customItem, &resultName );
    ok( !strcmp(HStringToLPCSTR(resultName), "TempTest"), "Error: Original name not returned. resultName %s, name %s\n", HStringToLPCSTR(resultName), "TempTest");

    IStorageItem_IsOfType( customItem, StorageItemTypes_Folder, &isFolder );
    ok( isFolder, "Error: Following path did not return as a folder.\n");

    hr = IStorageItem_DeleteAsyncOverloadDefaultOptions( customItem, &tempAction );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
}

static void test_KnownFolders( void )
{
    HRESULT hr;
    static const WCHAR *known_folders_statics_name = L"Windows.Storage.KnownFolders";
    IStorageFolder *documentsFolder;
    IStorageFolder *musicsFolder;
    IStorageItem *documentsFolderItem;
    IKnownFoldersStatics *knownFoldersStatics;
    IActivationFactory *factory;
    HSTRING str;
    HSTRING path;
    HSTRING name;
    CHAR username[256];
    DWORD username_len = sizeof(username);
    CHAR pathStr[MAX_PATH] = "C:\\users\\";

    if (!GetUserNameA( username, &username_len )) {
        return;
    }

    PathAppendA( pathStr, username );
    PathAppendA( pathStr, "Documents" );

    hr = WindowsCreateString( known_folders_statics_name, wcslen( known_folders_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( known_folders_statics_name ) );
    }

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );
    hr = IActivationFactory_QueryInterface( factory, &IID_IKnownFoldersStatics, (void **)&knownFoldersStatics );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IKnownFoldersStatics_get_DocumentsLibrary( knownFoldersStatics, &documentsFolder );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFolder_QueryInterface( documentsFolder, &IID_IStorageItem, (void **)&documentsFolderItem );
    
    IStorageItem_get_Path( documentsFolderItem, &path );
    IStorageItem_get_Name( documentsFolderItem, &name );

    ok( !strcmp(HStringToLPCSTR(path), pathStr), "Error: Original path not returned. path %s, pathStr %s\n", HStringToLPCSTR(path), pathStr);
    ok( !strcmp(HStringToLPCSTR(name), "Documents"), "Error: Original name not returned. name %s, name %s\n", HStringToLPCSTR(name), "Documents");    
    
    hr = IKnownFoldersStatics_get_MusicLibrary( knownFoldersStatics, &musicsFolder );
    ok( hr == E_ACCESSDENIED, "got hr %#lx.\n", hr );
}

START_TEST(storage)
{
    HRESULT hr;
    const wchar_t* apppath;
    IStorageItem *returnedItem = NULL;

    hr = RoInitialize( RO_INIT_MULTITHREADED );

    CoInitialize(NULL);

    ok( hr == S_OK, "RoInitialize failed, hr %#lx\n", hr );

    test_AppDataPathsStatics(&apppath);
    test_StorageFolder(apppath, &returnedItem);
    test_StorageItem(returnedItem);
    test_KnownFolders();

    RoUninitialize();
}
