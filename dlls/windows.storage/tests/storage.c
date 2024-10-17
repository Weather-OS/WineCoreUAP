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
#define WIDL_using_Windows_Storage_Streams
#include "windows.storage.streams.h"
#define WIDL_using_Windows_System
#include "windows.system.h"
#define WIDL_using_Windows_Storage_FileProperties
#include "windows.storage.fileproperties.h"
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
 * IAsyncActionCompletedHandler
 */

struct async_action_handler
{
    IAsyncActionCompletedHandler IAsyncActionCompletedHandler_iface;
    IAsyncAction *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct async_action_handler *impl_from_IAsyncActionCompletedHandler( IAsyncActionCompletedHandler *iface )
{
    return CONTAINING_RECORD( iface, struct async_action_handler, IAsyncActionCompletedHandler_iface );
}

static HRESULT WINAPI async_action_handler_QueryInterface( IAsyncActionCompletedHandler *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncActionCompletedHandler ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI async_action_handler_AddRef( IAsyncActionCompletedHandler *iface )
{
    return 2;
}

static ULONG WINAPI async_action_handler_Release( IAsyncActionCompletedHandler *iface )
{
    return 1;
}

static HRESULT WINAPI async_action_handler_Invoke( IAsyncActionCompletedHandler *iface,
                                                 IAsyncAction *async, AsyncStatus status )
{
    struct async_action_handler *impl = impl_from_IAsyncActionCompletedHandler( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncActionCompletedHandlerVtbl async_action_handler_vtbl =
{
    /*** IUnknown methods ***/
    async_action_handler_QueryInterface,
    async_action_handler_AddRef,
    async_action_handler_Release,
    /*** IAsyncActionCompletedHandler methods ***/
    async_action_handler_Invoke,
};

static struct async_action_handler default_async_action_handler = {{&async_action_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_HSTRING
 */

struct hstring_async_handler
{
    IAsyncOperationCompletedHandler_HSTRING IAsyncOperationCompletedHandler_HSTRING_iface;
    IAsyncOperation_HSTRING *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct hstring_async_handler *impl_from_IAsyncOperationCompletedHandler_HSTRING( IAsyncOperationCompletedHandler_HSTRING *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_async_handler, IAsyncOperationCompletedHandler_HSTRING_iface );
}

static HRESULT WINAPI hstring_async_handler_QueryInterface( IAsyncOperationCompletedHandler_HSTRING *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_HSTRING ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_async_handler_AddRef( IAsyncOperationCompletedHandler_HSTRING *iface )
{
    return 2;
}

static ULONG WINAPI hstring_async_handler_Release( IAsyncOperationCompletedHandler_HSTRING *iface )
{
    return 1;
}

static HRESULT WINAPI hstring_async_handler_Invoke( IAsyncOperationCompletedHandler_HSTRING *iface,
                                                 IAsyncOperation_HSTRING *async, AsyncStatus status )
{
    struct hstring_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_HSTRING( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_HSTRINGVtbl hstring_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    hstring_async_handler_QueryInterface,
    hstring_async_handler_AddRef,
    hstring_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<HSTRING> methods ***/
    hstring_async_handler_Invoke,
};

static struct hstring_async_handler default_hstring_async_handler = {{&hstring_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_IVector_HSTRING
 */

struct hstring_vector_async_handler
{
    IAsyncOperationCompletedHandler_IVector_HSTRING IAsyncOperationCompletedHandler_IVector_HSTRING_iface;
    IAsyncOperation_IVector_HSTRING *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct hstring_vector_async_handler *impl_from_IAsyncOperationCompletedHandler_IVector_HSTRING( IAsyncOperationCompletedHandler_IVector_HSTRING *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_vector_async_handler, IAsyncOperationCompletedHandler_IVector_HSTRING_iface );
}

static HRESULT WINAPI hstring_vector_async_handler_QueryInterface( IAsyncOperationCompletedHandler_IVector_HSTRING *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_IVector_HSTRING ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_vector_async_handler_AddRef( IAsyncOperationCompletedHandler_IVector_HSTRING *iface )
{
    return 2;
}

static ULONG WINAPI hstring_vector_async_handler_Release( IAsyncOperationCompletedHandler_IVector_HSTRING *iface )
{
    return 1;
}

static HRESULT WINAPI hstring_vector_async_handler_Invoke( IAsyncOperationCompletedHandler_IVector_HSTRING *iface,
                                                 IAsyncOperation_IVector_HSTRING *async, AsyncStatus status )
{
    struct hstring_vector_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_IVector_HSTRING( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_IVector_HSTRINGVtbl hstring_vector_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    hstring_vector_async_handler_QueryInterface,
    hstring_vector_async_handler_AddRef,
    hstring_vector_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<HSTRING> methods ***/
    hstring_vector_async_handler_Invoke,
};

static struct hstring_vector_async_handler default_hstring_vector_async_handler = {{&hstring_vector_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_IBuffer
 */

struct buffer_async_handler
{
    IAsyncOperationCompletedHandler_IBuffer IAsyncOperationCompletedHandler_IBuffer_iface;
    IAsyncOperation_IBuffer *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct buffer_async_handler *impl_from_IAsyncOperationCompletedHandler_IBuffer( IAsyncOperationCompletedHandler_IBuffer *iface )
{
    return CONTAINING_RECORD( iface, struct buffer_async_handler, IAsyncOperationCompletedHandler_IBuffer_iface );
}

static HRESULT WINAPI buffer_async_handler_QueryInterface( IAsyncOperationCompletedHandler_IBuffer *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_IBuffer ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI buffer_async_handler_AddRef( IAsyncOperationCompletedHandler_IBuffer *iface )
{
    return 2;
}

static ULONG WINAPI buffer_async_handler_Release( IAsyncOperationCompletedHandler_IBuffer *iface )
{
    return 1;
}

static HRESULT WINAPI buffer_async_handler_Invoke( IAsyncOperationCompletedHandler_IBuffer *iface,
                                                 IAsyncOperation_IBuffer *async, AsyncStatus status )
{
    struct buffer_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_IBuffer( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_IBufferVtbl buffer_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    buffer_async_handler_QueryInterface,
    buffer_async_handler_AddRef,
    buffer_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<IBuffer> methods ***/
    buffer_async_handler_Invoke,
};

static struct buffer_async_handler default_buffer_async_handler = {{&buffer_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_KnownFoldersAccessStatus
 */

struct known_folder_access_status_async_handler
{
    IAsyncOperationCompletedHandler_KnownFoldersAccessStatus IAsyncOperationCompletedHandler_KnownFoldersAccessStatus_iface;
    IAsyncOperation_KnownFoldersAccessStatus *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct known_folder_access_status_async_handler *impl_from_IAsyncOperationCompletedHandler_KnownFoldersAccessStatus( IAsyncOperationCompletedHandler_KnownFoldersAccessStatus *iface )
{
    return CONTAINING_RECORD( iface, struct known_folder_access_status_async_handler, IAsyncOperationCompletedHandler_KnownFoldersAccessStatus_iface );
}

static HRESULT WINAPI known_folder_access_status_async_handler_QueryInterface( IAsyncOperationCompletedHandler_KnownFoldersAccessStatus *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_KnownFoldersAccessStatus ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI known_folder_access_status_async_handler_AddRef( IAsyncOperationCompletedHandler_KnownFoldersAccessStatus *iface )
{
    return 2;
}

static ULONG WINAPI known_folder_access_status_async_handler_Release( IAsyncOperationCompletedHandler_KnownFoldersAccessStatus *iface )
{
    return 1;
}

static HRESULT WINAPI known_folder_access_status_async_handler_Invoke( IAsyncOperationCompletedHandler_KnownFoldersAccessStatus *iface,
                                                 IAsyncOperation_KnownFoldersAccessStatus *async, AsyncStatus status )
{
    struct known_folder_access_status_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_KnownFoldersAccessStatus( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_KnownFoldersAccessStatusVtbl known_folder_access_status_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    known_folder_access_status_async_handler_QueryInterface,
    known_folder_access_status_async_handler_AddRef,
    known_folder_access_status_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<KnownFoldersAccessStatus> methods ***/
    known_folder_access_status_async_handler_Invoke,
};

static struct known_folder_access_status_async_handler default_known_folder_access_status_async_handler = {{&known_folder_access_status_async_handler_vtbl}};

/**
 * IAsyncOperationCompletedHandler_BasicProperties
 */

struct basic_properties_async_handler
{
    IAsyncOperationCompletedHandler_BasicProperties IAsyncOperationCompletedHandler_BasicProperties_iface;
    IAsyncOperation_BasicProperties *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct basic_properties_async_handler *impl_from_IAsyncOperationCompletedHandler_BasicProperties( IAsyncOperationCompletedHandler_BasicProperties *iface )
{
    return CONTAINING_RECORD( iface, struct basic_properties_async_handler, IAsyncOperationCompletedHandler_BasicProperties_iface );
}

static HRESULT WINAPI basic_properties_async_handler_QueryInterface( IAsyncOperationCompletedHandler_BasicProperties *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationCompletedHandler_BasicProperties ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI basic_properties_async_handler_AddRef( IAsyncOperationCompletedHandler_BasicProperties *iface )
{
    return 2;
}

static ULONG WINAPI basic_properties_async_handler_Release( IAsyncOperationCompletedHandler_BasicProperties *iface )
{
    return 1;
}

static HRESULT WINAPI basic_properties_async_handler_Invoke( IAsyncOperationCompletedHandler_BasicProperties *iface,
                                                 IAsyncOperation_BasicProperties *async, AsyncStatus status )
{
    struct basic_properties_async_handler *impl = impl_from_IAsyncOperationCompletedHandler_BasicProperties( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncOperationCompletedHandler_BasicPropertiesVtbl basic_properties_async_handler_vtbl =
{
    /*** IUnknown methods ***/
    basic_properties_async_handler_QueryInterface,
    basic_properties_async_handler_AddRef,
    basic_properties_async_handler_Release,
    /*** IAsyncOperationCompletedHandler<BasicProperties> methods ***/
    basic_properties_async_handler_Invoke,
};

static struct basic_properties_async_handler default_basic_properties_async_handler = {{&basic_properties_async_handler_vtbl}};

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

static void test_StorageFolder( const wchar_t* path, IStorageItem **item, IStorageFile **file )
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
    struct basic_properties_async_handler basic_properties_async_handler;
    DateTime modifiedDate;
    DateTime createdDate;
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
    IStorageFolder2 *storageFolder2Results;
    IBasicProperties *basicPropertiesResults;
    IStorageFolderStatics *storage_folder_statics;
    IVectorView_IStorageItem *storageItemVectorResults;
    IVectorView_StorageFolder *storageFolderVectorResults;
    IVectorView_StorageFile *storageFileVectorResults;
    IAsyncOperation_StorageFile *storageFileOperation;
    IAsyncOperation_StorageFolder *storage_folder;
    IAsyncOperation_StorageFolder *storageFolderOperation;
    IAsyncOperation_IStorageItem *storageItemOperation;
    IAsyncOperation_BasicProperties *basicPropertiesOperation;
    IAsyncOperation_IVectorView_IStorageItem *storageItemVectorOperation;
    IAsyncOperation_IVectorView_StorageFolder *storageFolderVectorOperation;
    IAsyncOperation_IVectorView_StorageFile *storageFileVectorOperation;
    IAsyncOperationCompletedHandler_StorageFolder *storage_folder_handler;
    IAsyncOperationCompletedHandler_IStorageItem *storage_item_handler;
    IAsyncOperationCompletedHandler_StorageFile *storage_file_handler;
    IAsyncOperationCompletedHandler_BasicProperties *basicPropertiesHandler;
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
    UINT64 filesize;

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

    hr = IStorageItem_QueryInterface( storageItem, &IID_IStorageFolder, (void **)&storageFolderResults );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFolder_QueryInterface( storageFolderResults, &IID_IStorageItem, (void **)&storageItem);
    hr = IStorageItem_QueryInterface( storageItem, &IID_IStorageFolder, (void **)&storageFolderResults );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageItem_GetBasicPropertiesAsync( storageItem, &basicPropertiesOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
    
    check_interface( basicPropertiesOperation, &IID_IUnknown );
    check_interface( basicPropertiesOperation, &IID_IInspectable );
    check_interface( basicPropertiesOperation, &IID_IAgileObject );
    check_interface( basicPropertiesOperation, &IID_IAsyncInfo );
    check_interface( basicPropertiesOperation, &IID_IAsyncOperation_BasicProperties );

    hr = IAsyncOperation_BasicProperties_get_Completed( basicPropertiesOperation, &basicPropertiesHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( basicPropertiesHandler == NULL, "got handler %p\n", basicPropertiesHandler );
    
    basic_properties_async_handler = default_basic_properties_async_handler;
    basic_properties_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );
    
    hr = IAsyncOperation_BasicProperties_put_Completed( basicPropertiesOperation, &basic_properties_async_handler.IAsyncOperationCompletedHandler_BasicProperties_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );
   
    ret = WaitForSingleObject( basic_properties_async_handler.event, 100000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( basic_properties_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( basic_properties_async_handler.invoked, "handler not invoked\n" );
    ok( basic_properties_async_handler.async == basicPropertiesOperation, "got async %p\n", basic_properties_async_handler.async );
    ok( basic_properties_async_handler.status == Completed || broken( basic_properties_async_handler.status == Error ), "got status %u\n", basic_properties_async_handler.status );
    
    hr = IAsyncOperation_BasicProperties_GetResults( basicPropertiesOperation, &basicPropertiesResults );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IBasicProperties_get_Size( basicPropertiesResults, &filesize );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
    ok( filesize == 0llu, "File size received is NOT empty!");

    IStorageItem_get_DateCreated( storageItem, &createdDate );
    IBasicProperties_get_DateModified( basicPropertiesResults, &modifiedDate );
    ok( createdDate.UniversalTime == modifiedDate.UniversalTime, "File Creation date %lli, and Modification date %lli do not match!\n", createdDate.UniversalTime, modifiedDate.UniversalTime );

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
     * IStorageFolder2_TryGetItemAsync
    */

    IStorageFolder_QueryInterface( storageFolderResults, &IID_IStorageItem, (void **)&storageItem);
    hr = IStorageItem_QueryInterface( storageItem, &IID_IStorageFolder2, (void **)&storageFolder2Results );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IStorageFolder2_TryGetItemAsync( storageFolder2Results, nameString, &storageItemOperation );
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
   
    *file = storageFileResults3;
}

static void test_StorageItem( IStorageItem *customItem )
{
    static const WCHAR *name = L"TempTest";
    BOOLEAN isFolder;
    HSTRING renameName;
    HSTRING resultName;
    HRESULT hr;
    IAsyncAction *action;
    IAsyncActionCompletedHandler *actionHandler;
    DWORD ret;

    struct async_action_handler async_action_handler;

    WindowsCreateString( name, wcslen(name), &renameName );

    hr = IStorageItem_RenameAsyncOverloadDefaultOptions( customItem, renameName, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );


    IStorageItem_get_Name( customItem, &resultName );
    ok( !strcmp(HStringToLPCSTR(resultName), "TempTest"), "Error: Original name not returned. resultName %s, name %s\n", HStringToLPCSTR(resultName), "TempTest");

    IStorageItem_IsOfType( customItem, StorageItemTypes_Folder, &isFolder );
    ok( isFolder, "Error: Following path did not return as a folder.\n");

    hr = IStorageItem_DeleteAsyncOverloadDefaultOptions( customItem, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
}

static void test_KnownFolders( void )
{
    HRESULT hr;
    static const WCHAR *known_folders_statics_name = L"Windows.Storage.KnownFolders";
    IUser *user = NULL;
    IStorageFolder *documentsFolder;
    IStorageFolder *musicsFolder;
    IStorageItem *documentsFolderItem;
    IKnownFoldersStatics *knownFoldersStatics;
    IKnownFoldersStatics2 *knownFoldersStatics2;
    IKnownFoldersStatics3 *knownFoldersStatics3;
    IKnownFoldersStatics4 *knownFoldersStatics4;    
    KnownFoldersAccessStatus accessStatus;
    IAsyncOperation_StorageFolder *storageFolderOperation;
    IAsyncOperation_KnownFoldersAccessStatus *knownFoldersAccessStatusOperation;
    IAsyncOperationCompletedHandler_StorageFolder *storageFolderHandler;
    IAsyncOperationCompletedHandler_KnownFoldersAccessStatus *knownFoldersAccessStatusHandler;
    IActivationFactory *factory;
    HSTRING str;
    HSTRING path;
    HSTRING name;
    CHAR username[256];
    DWORD username_len = sizeof(username);
    CHAR pathStr[MAX_PATH] = "C:\\users\\";
    DWORD ret;

    struct storage_folder_async_handler storage_folder_async_handler;
    struct known_folder_access_status_async_handler known_folder_access_status_async_handler;

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

    //knownFoldersStatics1

    hr = IActivationFactory_QueryInterface( factory, &IID_IKnownFoldersStatics, (void **)&knownFoldersStatics );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IKnownFoldersStatics_get_DocumentsLibrary
     */

    hr = IKnownFoldersStatics_get_DocumentsLibrary( knownFoldersStatics, &documentsFolder );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFolder_QueryInterface( documentsFolder, &IID_IStorageItem, (void **)&documentsFolderItem );
    
    IStorageItem_get_Path( documentsFolderItem, &path );
    IStorageItem_get_Name( documentsFolderItem, &name );

    ok( !strcmp(HStringToLPCSTR(path), pathStr), "Error: Original path not returned. path %s, pathStr %s\n", HStringToLPCSTR(path), pathStr);
    ok( !strcmp(HStringToLPCSTR(name), "Documents"), "Error: Original name not returned. name %s, name %s\n", HStringToLPCSTR(name), "Documents");    
    
    /**
     * IKnownFoldersStatics_get_MusicLibrary
     */
    hr = IKnownFoldersStatics_get_MusicLibrary( knownFoldersStatics, &musicsFolder );
    ok( hr == E_ACCESSDENIED, "got hr %#lx.\n", hr );

    //knownFoldersStatics2

    hr = IActivationFactory_QueryInterface( factory, &IID_IKnownFoldersStatics2, (void **)&knownFoldersStatics2 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IKnownFoldersStatics2_get_Objects3D
     */

    hr = IKnownFoldersStatics2_get_Objects3D( knownFoldersStatics2, &documentsFolder );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    //knownFoldersStatics3
    hr = IActivationFactory_QueryInterface( factory, &IID_IKnownFoldersStatics3, (void **)&knownFoldersStatics3 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IKnownFoldersStatics3_GetFolderForUserAsync
     */
    hr = IKnownFoldersStatics3_GetFolderForUserAsync( knownFoldersStatics3, user, KnownFolderId_DocumentsLibrary, &storageFolderOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( storageFolderOperation, &IID_IUnknown );
    check_interface( storageFolderOperation, &IID_IInspectable );
    check_interface( storageFolderOperation, &IID_IAgileObject );
    check_interface( storageFolderOperation, &IID_IAsyncInfo );
    check_interface( storageFolderOperation, &IID_IAsyncOperation_StorageFolder );
    hr = IAsyncOperation_StorageFolder_get_Completed( storageFolderOperation, &storageFolderHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( storageFolderHandler == NULL, "got handler %p\n", storageFolderHandler );

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
    
    hr = IAsyncOperation_StorageFolder_GetResults( storageFolderOperation, &documentsFolder );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IStorageFolder_QueryInterface( documentsFolder, &IID_IStorageItem, (void **)&documentsFolderItem );
    
    IStorageItem_get_Path( documentsFolderItem, &path );
    IStorageItem_get_Name( documentsFolderItem, &name );

    ok( !strcmp(HStringToLPCSTR(path), pathStr), "Error: Original path not returned. path %s, pathStr %s\n", HStringToLPCSTR(path), pathStr);
    ok( !strcmp(HStringToLPCSTR(name), "Documents"), "Error: Original name not returned. name %s, name %s\n", HStringToLPCSTR(name), "Documents");

    //knownFoldersStatics4
    hr = IActivationFactory_QueryInterface( factory, &IID_IKnownFoldersStatics4, (void **)&knownFoldersStatics4 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );   

    /**
     * IKnownFoldersStatics4_RequestAccessAsync
     */

    hr = IKnownFoldersStatics4_RequestAccessAsync( knownFoldersStatics4, KnownFolderId_DocumentsLibrary, &knownFoldersAccessStatusOperation );

    check_interface( knownFoldersAccessStatusOperation, &IID_IUnknown );
    check_interface( knownFoldersAccessStatusOperation, &IID_IInspectable );
    check_interface( knownFoldersAccessStatusOperation, &IID_IAgileObject );
    check_interface( knownFoldersAccessStatusOperation, &IID_IAsyncInfo );
    check_interface( knownFoldersAccessStatusOperation, &IID_IAsyncOperation_KnownFoldersAccessStatus );

    hr = IAsyncOperation_KnownFoldersAccessStatus_GetResults ( knownFoldersAccessStatusOperation, &accessStatus );
    ok( hr == E_ILLEGAL_METHOD_CALL, "got hr %#lx.\n", hr );   
    ok( accessStatus == KnownFoldersAccessStatus_UserPromptRequired, "got accessStatus %#x.\n", accessStatus );

    hr = IAsyncOperation_KnownFoldersAccessStatus_get_Completed( knownFoldersAccessStatusOperation, &knownFoldersAccessStatusHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( knownFoldersAccessStatusHandler == NULL, "got handler %p\n", knownFoldersAccessStatusHandler );

    known_folder_access_status_async_handler = default_known_folder_access_status_async_handler;
    known_folder_access_status_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_KnownFoldersAccessStatus_put_Completed( knownFoldersAccessStatusOperation, &known_folder_access_status_async_handler.IAsyncOperationCompletedHandler_KnownFoldersAccessStatus_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );
   
    ret = WaitForSingleObject( known_folder_access_status_async_handler.event, INFINITE );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );
    
    ret = CloseHandle( known_folder_access_status_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( known_folder_access_status_async_handler.invoked, "handler not invoked\n" );
    ok( known_folder_access_status_async_handler.async == knownFoldersAccessStatusOperation, "got async %p\n", known_folder_access_status_async_handler.async );
    ok( known_folder_access_status_async_handler.status == Completed || broken( known_folder_access_status_async_handler.status == Error ), "got status %u\n", known_folder_access_status_async_handler.status );

    IAsyncOperation_KnownFoldersAccessStatus_GetResults ( knownFoldersAccessStatusOperation, &accessStatus );
    printf("User requested %#x\n", accessStatus);
}

static void test_Buffer( IBuffer **outBuffer )
{
    HRESULT hr = S_OK;
    static const WCHAR *buffer_statics_name = L"Windows.Storage.Streams.Buffer";
    IActivationFactory *factory;
    IBufferByteAccess *bufferByteAccess;
    IBufferFactory *bufferFactory;
    IBuffer *buffer;
    HSTRING str;
    UINT32 receivedCapacity;    
    LPCSTR testChar = "This is a test.";
    BYTE *bufferByte;
    BYTE *tmpBuffer;

    hr = WindowsCreateString( buffer_statics_name, wcslen( buffer_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( buffer_statics_name ) );
    }

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );

    hr = IActivationFactory_QueryInterface( factory, &IID_IBufferFactory, (void **)&bufferFactory );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IBufferFactory_Create
    */
    hr = IBufferFactory_Create( bufferFactory, 32u, &buffer );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IBuffer_get_Capacity
    */
    hr = IBuffer_get_Capacity( buffer, &receivedCapacity );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
    ok( receivedCapacity == 32u, "got receivedCapacity %d.\n", receivedCapacity );

    /**
     * IBufferByteAccess_get_Buffer
    */
    hr = IBuffer_QueryInterface( buffer, &IID_IBufferByteAccess, (void **)&bufferByteAccess );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IBufferByteAccess_get_Buffer( bufferByteAccess, &bufferByte );

    memcpy( bufferByte, testChar, 16 );

    IBufferByteAccess_get_Buffer( bufferByteAccess, &tmpBuffer );
    ok ( !strcmp( (LPCSTR)tmpBuffer, testChar ), "tmpBuffer not original! tmpBuffer %s\n", (LPCSTR)tmpBuffer );

    IBuffer_put_Length( buffer, 16 );

    *outBuffer = buffer;
}

static void test_FileIO( IStorageFile *file, HSTRING *returedPath, IBuffer *buffer )
{
    HRESULT hr = S_OK;
    static const WCHAR *file_io_statics_name = L"Windows.Storage.FileIO";
    static const WCHAR *text_to_write_1 = L"This is a ";
    static const WCHAR *text_to_write_2 = L"test.";
    static const WCHAR *text_to_conclude = L"This is a test.";
    IFileIOStatics *fileIOStatics;
    IBuffer *fileBuffer;
    IAsyncAction *action;
    IStorageItem *fileItem;
    IBufferByteAccess *fileBufferByteAccess;
    IVector_HSTRING *linesToRead;
    IVectorView_HSTRING *linesToReadView;
    IIterable_HSTRING *linesToReadIterable;
    IIterator_HSTRING *linesToReadIterator;
    IAsyncOperation_HSTRING *hstringOperation;
    IAsyncOperation_IBuffer *bufferOperation;
    IAsyncOperation_IVector_HSTRING *hstringVectorOperation;
    IAsyncActionCompletedHandler *actionHandler;
    IAsyncOperationCompletedHandler_HSTRING *hstringHandler;
    IAsyncOperationCompletedHandler_IVector_HSTRING *hstringVectorHandler;
    IAsyncOperationCompletedHandler_IBuffer *bufferHandler;
    IActivationFactory *factory; 
    HSTRING str;
    HSTRING textToWrite1;
    HSTRING textToWrite2;
    HSTRING textToConclude;
    HSTRING textToRead;
    UINT32 bufferLen;
    BYTE *fileBytes;
    DWORD ret;
    boolean next;
    INT comparisonResult;

    struct async_action_handler async_action_handler;
    struct buffer_async_handler buffer_async_handler;
    struct hstring_async_handler hstring_async_handler;
    struct hstring_vector_async_handler hstring_vector_async_handler;

    hr = WindowsCreateString( file_io_statics_name, wcslen( file_io_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( text_to_write_1, wcslen( text_to_write_1 ), &textToWrite1 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( text_to_write_2, wcslen( text_to_write_2 ), &textToWrite2 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( text_to_conclude, wcslen( text_to_conclude ), &textToConclude );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( file_io_statics_name ) );
    }

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );

    //fileIOStatics

    hr = IActivationFactory_QueryInterface( factory, &IID_IFileIOStatics, (void **)&fileIOStatics );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IFileIOStatics_WriteTextWithEncodingAsync
     */

    hr = IFileIOStatics_WriteTextWithEncodingAsync( fileIOStatics, file, textToWrite1, UnicodeEncoding_Utf8, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    /**
     * IFileIOStatics_AppendTextWithEncodingAsync
     */

    hr = IFileIOStatics_AppendTextWithEncodingAsync( fileIOStatics, file, textToWrite2, UnicodeEncoding_Utf8, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    /**
     * IFileIOStatics_ReadLinesWithEncodingAsync
     */

    hr = IFileIOStatics_ReadLinesWithEncodingAsync( fileIOStatics, file, UnicodeEncoding_Utf8, &hstringVectorOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( hstringVectorOperation, &IID_IUnknown );
    check_interface( hstringVectorOperation, &IID_IInspectable );
    check_interface( hstringVectorOperation, &IID_IAgileObject );
    check_interface( hstringVectorOperation, &IID_IAsyncInfo );
    check_interface( hstringVectorOperation, &IID_IAsyncOperation_IVector_HSTRING );

    hr = IAsyncOperation_IVector_HSTRING_get_Completed( hstringVectorOperation, &hstringVectorHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( hstringVectorHandler == NULL, "got handler %p\n", hstringVectorHandler );

    hstring_vector_async_handler = default_hstring_vector_async_handler;
    hstring_vector_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_IVector_HSTRING_put_Completed( hstringVectorOperation, &hstring_vector_async_handler.IAsyncOperationCompletedHandler_IVector_HSTRING_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( hstring_vector_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( hstring_vector_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( hstring_vector_async_handler.invoked, "handler not invoked\n" );
    ok( hstring_vector_async_handler.async == hstringVectorOperation, "got async %p\n", hstring_vector_async_handler.async );
    ok( hstring_vector_async_handler.status == Completed || broken( hstring_vector_async_handler.status == Error ), "got status %u\n", hstring_vector_async_handler.status );

    hr = IAsyncOperation_IVector_HSTRING_GetResults( hstringVectorOperation, &linesToRead );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IVector_HSTRING_GetView( linesToRead, &linesToReadView );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IVectorView_HSTRING_QueryInterface( linesToReadView, &IID_IIterable_HSTRING, (void **)&linesToReadIterable );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IIterable_HSTRING_First( linesToReadIterable, &linesToReadIterator );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IIterator_HSTRING_get_Current( linesToReadIterator, &textToRead );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IIterator_HSTRING_MoveNext( linesToReadIterator, &next );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
    ok( next == FALSE, "Next line shouldn't exist!. got next %d\n", next );

    hr = WindowsCompareStringOrdinal( textToRead, textToConclude, &comparisonResult );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    ok ( !comparisonResult, "textToRead (%s) is not equal to textToConclude (%s)!\n", HStringToLPCSTR( textToRead ), HStringToLPCSTR( textToConclude ) );

    /**
     * IFileIOStatics_WriteLinesWithEncodingAsync
     */

    hr = IFileIOStatics_WriteLinesWithEncodingAsync( fileIOStatics, file, linesToReadIterable, UnicodeEncoding_Utf8, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    /**
     * IFileIOStatics_ReadTextWithEncodingAsync
     */

    hr = IFileIOStatics_ReadTextWithEncodingAsync( fileIOStatics, file, UnicodeEncoding_Utf8, &hstringOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( hstringOperation, &IID_IUnknown );
    check_interface( hstringOperation, &IID_IInspectable );
    check_interface( hstringOperation, &IID_IAgileObject );
    check_interface( hstringOperation, &IID_IAsyncInfo );
    check_interface( hstringOperation, &IID_IAsyncOperation_HSTRING );

    hr = IAsyncOperation_HSTRING_get_Completed( hstringOperation, &hstringHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( hstringHandler == NULL, "got handler %p\n", hstringHandler );

    hstring_async_handler = default_hstring_async_handler;
    hstring_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_HSTRING_put_Completed( hstringOperation, &hstring_async_handler.IAsyncOperationCompletedHandler_HSTRING_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( hstring_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( hstring_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( hstring_async_handler.invoked, "handler not invoked\n" );
    ok( hstring_async_handler.async == hstringOperation, "got async %p\n", hstring_async_handler.async );
    ok( hstring_async_handler.status == Completed || broken( hstring_async_handler.status == Error ), "got status %u\n", hstring_async_handler.status );

    hr = IAsyncOperation_HSTRING_GetResults( hstringOperation, &textToRead );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCompareStringOrdinal( textToRead, textToConclude, &comparisonResult );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    ok ( !comparisonResult, "textToRead (%s) is not equal to textToConclude (%s)!\n", HStringToLPCSTR( textToRead ), HStringToLPCSTR( textToConclude ) );

    /**
     * IFileIOStatics_WriteBufferAsync
    */

    hr = IFileIOStatics_WriteBufferAsync( fileIOStatics, file, buffer, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    IBuffer_QueryInterface( buffer, &IID_IBufferByteAccess, (void **)&fileBufferByteAccess );
    IBufferByteAccess_get_Buffer( fileBufferByteAccess, &fileBytes );
    IBuffer_get_Length( buffer, &bufferLen );

    /**
     * IFileIOStatics_WriteBufferAsync
    */

    hr = IFileIOStatics_WriteBytesAsync( fileIOStatics, file, bufferLen, fileBytes, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    /**
     * IFileIOStatics_ReadBufferAsync
     */

    hr = IFileIOStatics_ReadBufferAsync( fileIOStatics, file, &bufferOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( bufferOperation, &IID_IUnknown );
    check_interface( bufferOperation, &IID_IInspectable );
    check_interface( bufferOperation, &IID_IAgileObject );
    check_interface( bufferOperation, &IID_IAsyncInfo );
    check_interface( bufferOperation, &IID_IAsyncOperation_IBuffer );

    hr = IAsyncOperation_IBuffer_get_Completed( bufferOperation, &bufferHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( bufferHandler == NULL, "got handler %p\n", bufferHandler );

    buffer_async_handler = default_buffer_async_handler;
    buffer_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_IBuffer_put_Completed( bufferOperation, &buffer_async_handler.IAsyncOperationCompletedHandler_IBuffer_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( buffer_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( buffer_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( buffer_async_handler.invoked, "handler not invoked\n" );
    ok( buffer_async_handler.async == bufferOperation, "got async %p\n", buffer_async_handler.async );
    ok( buffer_async_handler.status == Completed || broken( buffer_async_handler.status == Error ), "got status %u\n", buffer_async_handler.status );

    hr = IAsyncOperation_IBuffer_GetResults( bufferOperation, &fileBuffer );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IBuffer_QueryInterface( fileBuffer, &IID_IBufferByteAccess, (void **)&fileBufferByteAccess );
    IBufferByteAccess_get_Buffer( fileBufferByteAccess, &fileBytes );

    ok ( !strcmp( (LPCSTR)fileBytes, "This is a test." ), "fileBytes not original! fileBytes %s\n", (LPCSTR)fileBytes );

    IStorageFile_QueryInterface( file, &IID_IStorageItem, (void **)&fileItem );
    IStorageItem_get_Path( fileItem, returedPath );
}

static void test_PathIO( HSTRING path )
{
    HRESULT hr = S_OK;
    static const WCHAR *path_io_statics_name = L"Windows.Storage.PathIO";
    static const WCHAR *text_to_write_1 = L"This is a ";
    static const WCHAR *text_to_write_2 = L"test.";
    static const WCHAR *text_to_conclude = L"This is a test.";
    IPathIOStatics *pathIOStatics;
    IAsyncAction *action;
    IVector_HSTRING *linesToRead;
    IVectorView_HSTRING *linesToReadView;
    IIterable_HSTRING *linesToReadIterable;
    IIterator_HSTRING *linesToReadIterator;
    IAsyncOperation_HSTRING *hstringOperation;
    IAsyncOperation_IVector_HSTRING *hstringVectorOperation;
    IAsyncActionCompletedHandler *actionHandler;
    IAsyncOperationCompletedHandler_HSTRING *hstringHandler;
    IAsyncOperationCompletedHandler_IVector_HSTRING *hstringVectorHandler;
    IActivationFactory *factory; 
    HSTRING str;
    HSTRING textToWrite1;
    HSTRING textToWrite2;
    HSTRING textToConclude;
    HSTRING textToRead;
    DWORD ret;
    boolean next;
    INT comparisonResult;

    struct async_action_handler async_action_handler;
    struct hstring_async_handler hstring_async_handler;
    struct hstring_vector_async_handler hstring_vector_async_handler;

    hr = WindowsCreateString( path_io_statics_name, wcslen( path_io_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( text_to_write_1, wcslen( text_to_write_1 ), &textToWrite1 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( text_to_write_2, wcslen( text_to_write_2 ), &textToWrite2 );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( text_to_conclude, wcslen( text_to_conclude ), &textToConclude );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( path_io_statics_name ) );
    }

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );

    //pathIOStatics

    hr = IActivationFactory_QueryInterface( factory, &IID_IPathIOStatics, (void **)&pathIOStatics );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IPathIOStatics_WriteTextWithEncodingAsync
     */

    hr = IPathIOStatics_WriteTextWithEncodingAsync( pathIOStatics, path, textToWrite1, UnicodeEncoding_Utf8, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    /**
     * IPathIOStatics_AppendTextWithEncodingAsync
     */

    hr = IPathIOStatics_AppendTextWithEncodingAsync( pathIOStatics, path, textToWrite2, UnicodeEncoding_Utf8, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    /**
     * IPathIOStatics_ReadLinesWithEncodingAsync
     */

    hr = IPathIOStatics_ReadLinesWithEncodingAsync( pathIOStatics, path, UnicodeEncoding_Utf8, &hstringVectorOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( hstringVectorOperation, &IID_IUnknown );
    check_interface( hstringVectorOperation, &IID_IInspectable );
    check_interface( hstringVectorOperation, &IID_IAgileObject );
    check_interface( hstringVectorOperation, &IID_IAsyncInfo );
    check_interface( hstringVectorOperation, &IID_IAsyncOperation_IVector_HSTRING );

    hr = IAsyncOperation_IVector_HSTRING_get_Completed( hstringVectorOperation, &hstringVectorHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( hstringVectorHandler == NULL, "got handler %p\n", hstringVectorHandler );

    hstring_vector_async_handler = default_hstring_vector_async_handler;
    hstring_vector_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_IVector_HSTRING_put_Completed( hstringVectorOperation, &hstring_vector_async_handler.IAsyncOperationCompletedHandler_IVector_HSTRING_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( hstring_vector_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( hstring_vector_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( hstring_vector_async_handler.invoked, "handler not invoked\n" );
    ok( hstring_vector_async_handler.async == hstringVectorOperation, "got async %p\n", hstring_vector_async_handler.async );
    ok( hstring_vector_async_handler.status == Completed || broken( hstring_vector_async_handler.status == Error ), "got status %u\n", hstring_vector_async_handler.status );

    hr = IAsyncOperation_IVector_HSTRING_GetResults( hstringVectorOperation, &linesToRead );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IVector_HSTRING_GetView( linesToRead, &linesToReadView );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IVectorView_HSTRING_QueryInterface( linesToReadView, &IID_IIterable_HSTRING, (void **)&linesToReadIterable );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IIterable_HSTRING_First( linesToReadIterable, &linesToReadIterator );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IIterator_HSTRING_get_Current( linesToReadIterator, &textToRead );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IIterator_HSTRING_MoveNext( linesToReadIterator, &next );
    ok( hr == S_OK, "got hr %#lx.\n", hr );
    ok( next == FALSE, "Next line shouldn't exist!. got next %d\n", next );

    hr = WindowsCompareStringOrdinal( textToRead, textToConclude, &comparisonResult );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    ok ( !comparisonResult, "textToRead (%s) is not equal to textToConclude (%s)!\n", HStringToLPCSTR( textToRead ), HStringToLPCSTR( textToConclude ) );

    /**
     * IPathIOStatics_WriteLinesWithEncodingAsync
     */

    hr = IPathIOStatics_WriteLinesWithEncodingAsync( pathIOStatics, path, linesToReadIterable, UnicodeEncoding_Utf8, &action );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( action, &IID_IUnknown );
    check_interface( action, &IID_IInspectable );
    check_interface( action, &IID_IAgileObject );
    check_interface( action, &IID_IAsyncInfo );
    check_interface( action, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( action, &actionHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( actionHandler == NULL, "got handler %p\n", actionHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( action, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == action, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    /**
     * IPathIOStatics_ReadTextWithEncodingAsync
     */

    hr = IPathIOStatics_ReadTextWithEncodingAsync( pathIOStatics, path, UnicodeEncoding_Utf8, &hstringOperation );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( hstringOperation, &IID_IUnknown );
    check_interface( hstringOperation, &IID_IInspectable );
    check_interface( hstringOperation, &IID_IAgileObject );
    check_interface( hstringOperation, &IID_IAsyncInfo );
    check_interface( hstringOperation, &IID_IAsyncOperation_HSTRING );

    hr = IAsyncOperation_HSTRING_get_Completed( hstringOperation, &hstringHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( hstringHandler == NULL, "got handler %p\n", hstringHandler );

    hstring_async_handler = default_hstring_async_handler;
    hstring_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_HSTRING_put_Completed( hstringOperation, &hstring_async_handler.IAsyncOperationCompletedHandler_HSTRING_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( hstring_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( hstring_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( hstring_async_handler.invoked, "handler not invoked\n" );
    ok( hstring_async_handler.async == hstringOperation, "got async %p\n", hstring_async_handler.async );
    ok( hstring_async_handler.status == Completed || broken( hstring_async_handler.status == Error ), "got status %u\n", hstring_async_handler.status );

    hr = IAsyncOperation_HSTRING_GetResults( hstringOperation, &textToRead );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCompareStringOrdinal( textToRead, textToConclude, &comparisonResult );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    ok ( !comparisonResult, "textToRead (%s) is not equal to textToConclude (%s)!\n", HStringToLPCSTR( textToRead ), HStringToLPCSTR( textToConclude ) );
}

static void test_DownloadsFolder( void )
{
    HRESULT hr = S_OK;
    static const WCHAR *downloads_folders_statics_name = L"Windows.Storage.DownloadsFolder";
    static const WCHAR *desiredFileNameStr = L"NewFile.txt";
    IStorageFile *file;
    IStorageItem *fileItem;
    IDownloadsFolderStatics *downloadsFolderStatics;
    IAsyncOperation_StorageFile *fileOperation;
    IAsyncOperationCompletedHandler_StorageFile *fileHandler;
    DWORD ret;
    HSTRING str;
    HSTRING desiredFileName;
    HSTRING fileName;
    IActivationFactory *factory;

    struct storage_file_async_handler storage_file_async_handler;

    hr = WindowsCreateString( downloads_folders_statics_name, wcslen( downloads_folders_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( desiredFileNameStr, wcslen( desiredFileNameStr ), &desiredFileName );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( downloads_folders_statics_name ) );
    }

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );

    //downloadsFolderStatics

    hr = IActivationFactory_QueryInterface( factory, &IID_IDownloadsFolderStatics, (void **)&downloadsFolderStatics );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IDownloadsFolderStatics_CreateFileWithCollisionOptionAsync
     */

    IDownloadsFolderStatics_CreateFileWithCollisionOptionAsync( downloadsFolderStatics, desiredFileName, CreationCollisionOption_ReplaceExisting, &fileOperation );

    check_interface( fileOperation, &IID_IUnknown );
    check_interface( fileOperation, &IID_IInspectable );
    check_interface( fileOperation, &IID_IAgileObject );
    check_interface( fileOperation, &IID_IAsyncInfo );
    check_interface( fileOperation, &IID_IAsyncOperation_StorageFile );

    hr = IAsyncOperation_StorageFile_get_Completed( fileOperation, &fileHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( fileHandler == NULL, "got handler %p\n", fileHandler );

    storage_file_async_handler = default_storage_file_async_handler;
    storage_file_async_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncOperation_StorageFile_put_Completed( fileOperation, &storage_file_async_handler.IAsyncOperationCompletedHandler_StorageFile_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );
   
    ret = WaitForSingleObject( storage_file_async_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( storage_file_async_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( storage_file_async_handler.invoked, "handler not invoked\n" );
    ok( storage_file_async_handler.async == fileOperation, "got async %p\n", storage_file_async_handler.async );
    ok( storage_file_async_handler.status == Completed || broken( storage_file_async_handler.status == Error ), "got status %u\n", storage_file_async_handler.status );

    hr = IAsyncOperation_StorageFile_GetResults( fileOperation, &file );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IStorageFile_QueryInterface( file, &IID_IStorageItem, (void **)&fileItem );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IStorageItem_get_Name( fileItem, &fileName );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    ok( !strcmp(HStringToLPCSTR(fileName), "NewFile.txt"), "Error: Original name not returned. fileName %s, name %s\n", HStringToLPCSTR(fileName), "NewFile.txt");
}

START_TEST(storage)
{
    HRESULT hr;
    const wchar_t* apppath;
    IStorageItem *returnedItem = NULL;
    IStorageFile *returnedFile = NULL;
    IBuffer *buffer = NULL;
    HSTRING returnedFilePath = NULL;

    hr = RoInitialize( RO_INIT_MULTITHREADED );

    CoInitialize(NULL);

    ok( hr == S_OK, "RoInitialize failed, hr %#lx\n", hr );

    test_AppDataPathsStatics(&apppath);
    test_StorageFolder(apppath, &returnedItem, &returnedFile);
    test_Buffer(&buffer);
    test_FileIO(returnedFile, &returnedFilePath, buffer);
    test_PathIO(returnedFilePath);
    test_StorageItem(returnedItem);
    test_KnownFolders();
    test_DownloadsFolder();

    RoUninitialize();
}
