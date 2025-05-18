/* WinRT Windows.Storage Implementation
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

/**
 * Developer notes: Do not waste your time coding asynchronous tasks for node updates.
 * These are almost never used in a WinRT setting, These include, but are not limited to:
 *  - IStorageQueryResultBase::ContentsChanged
 *  - IStorageFolder::ContentsChanged
 */

#ifndef __WINE_WINDOWS_STORAGE_PRIVATE_H
#define __WINE_WINDOWS_STORAGE_PRIVATE_H

#include <stdarg.h>

#define COBJMACROS
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "shellapi.h"
#include "roapi.h"
#include "roerrorapi.h"
#include "aclapi.h"
#include "accctrl.h"
#include "restrictederrorinfo.h"

#include "resource.h"

#include "activation.h"

//The specific order in which header files are included here is extremely important.
#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#define WIDL_using_Windows_Foundation_Numerics
#include "windows.foundation.h"
#define WIDL_using_Windows_Storage_Streams
#include "windows.storage.streams.h"
#define WIDL_using_Windows_Storage_FileProperties
#include "windows.storage.fileproperties.h"
#define WIDL_using_Windows_System
#include "windows.system.h"
#define WIDL_using_Windows_Storage
#include "windows.storage.h"
#define WIDL_using_Windows_Storage_Search
#include "windows.storage.search.h"

#include "robuffer.h"

#include "provider.h"

#define WINDOWS_TICK 10000000
#define SEC_TO_UNIX_EPOCH 11644473600LL

#define _ENABLE_DEBUGGING_ WINE_DEFAULT_DEBUG_CHANNEL(winrt_storage);

struct vector_iids
{
    const GUID *observableVector;
    const GUID *vector;
    const GUID *view;
    const GUID *iterable;
    const GUID *iterator;
};

struct map_iids
{
    const GUID *observableMap;
    const GUID *map;
    const GUID *view;
    const GUID *iterable;
    const GUID *iterator;
};

struct async_operation_iids
{
    const GUID *operation;
};

extern IActivationFactory *app_data_paths_factory;
extern IActivationFactory *user_data_paths_factory;
extern IActivationFactory *storage_folder_factory;
extern IActivationFactory *storage_file_factory;
extern IActivationFactory *known_folders_factory;
extern IActivationFactory *basic_properties_factory;
extern IActivationFactory *storage_item_content_properties_factory;
extern IActivationFactory *downloads_folder_factory;
extern IActivationFactory *file_io_factory;
extern IActivationFactory *path_io_factory;
extern IActivationFactory *system_properties_factory;
extern IActivationFactory *system_data_paths_factory;
extern IActivationFactory *storage_library_statics_factory;
extern IActivationFactory *query_options_activatable_factory;

extern HRESULT SetLastRestrictedErrorWithMessageW( HRESULT status, LPCWSTR message );
extern HRESULT SetLastRestrictedErrorWithMessageFormattedW( HRESULT status, LPCWSTR format, ... );
extern LPCWSTR GetResourceW( INT resourceId );
extern LPCSTR GetResourceA( INT resourceId );

typedef HRESULT (WINAPI *async_operation_callback)( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

typedef HRESULT (WINAPI *async_operation_with_progress_callback)( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress );

typedef HRESULT (WINAPI *observable_hstring_map_callback)( IObservableMap_HSTRING_IInspectable *invoker, IUnknown *data, IMapChangedEventArgs_HSTRING *args );

extern HRESULT async_info_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, 
                                              IInspectable *outer, IWineAsyncInfoImpl **out );
extern HRESULT async_info_with_progress_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback,
                                              IInspectable *outer, IWineAsyncInfoWithProgressImpl **out );
extern HRESULT async_operation_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperation_IInspectable **out );
extern HRESULT async_operation_boolean_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_boolean **out );
extern HRESULT async_operation_with_progress_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperationWithProgress_IInspectable_UINT64 **out );
extern HRESULT async_operation_with_progress_uint32_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperationWithProgress_UINT32_UINT32 **out );
extern HRESULT async_operation_with_progress_uint64_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperationWithProgress_UINT64_UINT64 **out );
extern HRESULT async_operation_hstring_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_HSTRING **out );
extern HRESULT async_action_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, 
                                              IAsyncAction **ret);
extern HRESULT async_operation_uint32_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperation_UINT32 **out );
extern HRESULT async_operation_indexed_state_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IndexedState **out );
extern HRESULT vector_create( const struct vector_iids *iids, void **out );

extern HRESULT observable_vector_create( const struct vector_iids *iids, void **out );

extern HRESULT hstring_vector_create( IVector_HSTRING **out );

extern HRESULT hstring_map_create( const struct map_iids *iids, void **out );

extern HRESULT observable_hstring_map_create( const struct map_iids *iids, void **out );

extern HRESULT hstring_map_event_handler_create( observable_hstring_map_callback callback, OPTIONAL IUnknown *data, IMapChangedEventHandler_HSTRING_IInspectable **out );

extern HRESULT property_set_create( const struct map_iids *iids, IPropertySet **out );

extern HRESULT async_operation_basic_properties_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_BasicProperties **out );

#define DEFINE_VECTOR_IIDS( interface ) \
    struct vector_iids interface##_iids = {.iterable = &IID_IIterable_##interface, .iterator = &IID_IIterator_##interface, .vector = &IID_IVector_##interface, .view = &IID_IVectorView_##interface, .observableVector = &IID_IObservableVector_##interface };

#define DEFINE_HSTRING_MAP_IIDS( interface ) \
    struct map_iids interface##_iids = {.iterable = &IID_IIterable_IKeyValuePair_HSTRING_##interface, .iterator = &IID_IIterator_IKeyValuePair_HSTRING_##interface, .map = &IID_IMap_HSTRING_##interface, .view = &IID_IMapView_HSTRING_##interface, .observableMap = &IID_IObservableMap_HSTRING_##interface };

#define DEFINE_IUNKNOWN_( pfx, iface_type, impl_type, impl_from, iface_mem, expr )                 \
    static inline impl_type *impl_from( iface_type *iface )                                        \
    {                                                                                              \
        return CONTAINING_RECORD( iface, impl_type, iface_mem );                                   \
    }                                                                                              \
    static HRESULT WINAPI pfx##_QueryInterface( iface_type *iface, REFIID iid, void **out )        \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_QueryInterface( (IInspectable *)(expr), iid, out );                    \
    }                                                                                              \
    static ULONG WINAPI pfx##_AddRef( iface_type *iface )                                          \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_AddRef( (IInspectable *)(expr) );                                      \
    }                                                                                              \
    static ULONG WINAPI pfx##_Release( iface_type *iface )                                         \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_Release( (IInspectable *)(expr) );                                     \
    }

#define DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from, iface_mem, expr )             \
    impl_type *impl_from( iface_type *iface )                                        \
    {                                                                                              \
        return CONTAINING_RECORD( iface, impl_type, iface_mem );                                   \
    }                                                                                              \
    static HRESULT WINAPI pfx##_QueryInterface( iface_type *iface, REFIID iid, void **out )        \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_QueryInterface( (IInspectable *)(expr), iid, out );                    \
    }                                                                                              \
    static ULONG WINAPI pfx##_AddRef( iface_type *iface )                                          \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_AddRef( (IInspectable *)(expr) );                                      \
    }                                                                                              \
    static ULONG WINAPI pfx##_Release( iface_type *iface )                                         \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_Release( (IInspectable *)(expr) );                                     \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetIids( iface_type *iface, ULONG *iid_count, IID **iids )         \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetIids( (IInspectable *)(expr), iid_count, iids );                    \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetRuntimeClassName( iface_type *iface, HSTRING *class_name )      \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetRuntimeClassName( (IInspectable *)(expr), class_name );             \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetTrustLevel( iface_type *iface, TrustLevel *trust_level )        \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetTrustLevel( (IInspectable *)(expr), trust_level );                  \
    }

#define DEFINE_IUNKNOWN( pfx, iface_type, impl_type, base_iface )                                  \
    DEFINE_IUNKNOWN_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, &impl->base_iface )
#define DEFINE_IINSPECTABLE( pfx, iface_type, impl_type, base_iface )                              \
    DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, &impl->base_iface )
#define DEFINE_IINSPECTABLE_OUTER( pfx, iface_type, impl_type, outer_iface )                       \
    DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, impl->outer_iface )


#define DEFINE_ASYNC_COMPLETED_HANDLER( name, iface_type, async_type )                              \
    struct name                                                                                     \
    {                                                                                               \
        iface_type iface_type##_iface;                                                              \
        LONG refcount;                                                                              \
        BOOL invoked;                                                                               \
        HANDLE event;                                                                               \
    };                                                                                              \
                                                                                                    \
    static HRESULT WINAPI name##_QueryInterface( iface_type *iface, REFIID iid, void **out )        \
    {                                                                                               \
        if (IsEqualGUID( iid, &IID_IUnknown ) || IsEqualGUID( iid, &IID_IAgileObject ) ||           \
            IsEqualGUID( iid, &IID_##iface_type ))                                                  \
        {                                                                                           \
            IInspectable_AddRef( (IInspectable *)iface );                                           \
            *out = iface;                                                                           \
            return S_OK;                                                                            \
        }                                                                                           \
                                                                                                    \
        *out = NULL;                                                                                \
        return E_NOINTERFACE;                                                                       \
    }                                                                                               \
                                                                                                    \
    static ULONG WINAPI name##_AddRef( iface_type *iface )                                          \
    {                                                                                               \
        struct name *impl = CONTAINING_RECORD( iface, struct name, iface_type##_iface );            \
        return InterlockedIncrement( &impl->refcount );                                             \
    }                                                                                               \
                                                                                                    \
    static ULONG WINAPI name##_Release( iface_type *iface )                                         \
    {                                                                                               \
        struct name *impl = CONTAINING_RECORD( iface, struct name, iface_type##_iface );            \
        ULONG ref = InterlockedDecrement( &impl->refcount );                                        \
        if (!ref) free( impl );                                                                     \
        return ref;                                                                                 \
    }                                                                                               \
                                                                                                    \
    static HRESULT WINAPI name##_Invoke( iface_type *iface, async_type *async, AsyncStatus status ) \
    {                                                                                               \
        struct name *impl = CONTAINING_RECORD( iface, struct name, iface_type##_iface );            \
        impl->invoked = TRUE;                                                                       \
        if (impl->event) SetEvent( impl->event );                                                   \
        return S_OK;                                                                                \
    }                                                                                               \
                                                                                                    \
    static iface_type##Vtbl name##_vtbl =                                                           \
    {                                                                                               \
        name##_QueryInterface,                                                                      \
        name##_AddRef,                                                                              \
        name##_Release,                                                                             \
        name##_Invoke,                                                                              \
    };                                                                                              \
                                                                                                    \
    static iface_type *name##_create( HANDLE event )                                                \
    {                                                                                               \
        struct name *impl;                                                                          \
                                                                                                    \
        if (!(impl = calloc( 1, sizeof(*impl) ))) return NULL;                                      \
        impl->iface_type##_iface.lpVtbl = &name##_vtbl;                                             \
        impl->event = event;                                                                        \
        impl->refcount = 1;                                                                         \
                                                                                                    \
        return &impl->iface_type##_iface;                                                           \
    }                                                                                               \
                                                                                                    \
    static DWORD await_##async_type( async_type *async, DWORD timeout )                             \
    {                                                                                               \
        iface_type *handler;                                                                        \
        HANDLE event;                                                                               \
        DWORD ret;                                                                                  \
                                                                                                    \
        event = CreateEventW( NULL, FALSE, FALSE, NULL );                                           \
        handler = name##_create( event );                                                           \
        async_type##_put_Completed( async, handler );                                               \
        ret = WaitForSingleObject( event, timeout );                                                \
        CloseHandle( event );                                                                       \
        iface_type##_Release( handler );                                                            \
                                                                                                    \
        return ret;                                                                                 \
    }

#define ACTIVATE_INSTANCE( instance_name, instance_object, instance_iid )                                               \
    HSTRING _str;                                                                                                       \
    HRESULT _hr;                                                                                                        \
    _hr = WindowsCreateString( instance_name, wcslen( instance_name ), &_str );                                         \
                                                                                                                        \
    _hr = RoGetActivationFactory( _str, &instance_iid, (void **)&instance_object );                                     \
    WindowsDeleteString( _str );                                                                                        \
    if (_hr == REGDB_E_CLASSNOTREG) return E_ABORT;                                                                     \
    
#define CHECK_LAST_RESTRICTED_ERROR()                                                               \
{                                                                                                   \
    HRESULT _hr;                                                                                    \
    IRestrictedErrorInfo *_restricted_error = NULL;                                                 \
    _hr = GetRestrictedErrorInfo( &_restricted_error );                                             \
    if ( _hr != S_FALSE )                                                                           \
    {                                                                                               \
        IRestrictedErrorInfo_GetErrorDetails( _restricted_error, NULL, &_hr, NULL, NULL );          \
        SetRestrictedErrorInfo( _restricted_error );                                                \
        return _hr;                                                                                 \
    }                                                                                               \
}
#endif
