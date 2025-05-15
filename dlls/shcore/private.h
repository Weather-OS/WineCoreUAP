/* shcore WinRT implementations
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

#ifndef __WINE_SHCORE_WINDOWS_STORAGE_PRIVATE_H
#define __WINE_SHCORE_WINDOWS_STORAGE_PRIVATE_H

#define COBJMACROS
#include "stdarg.h"
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "roapi.h"
#include "roerrorapi.h"
#include "aclapi.h"
#include "accctrl.h"
#include "restrictederrorinfo.h"

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

#define _ENABLE_DEBUGGING_ WINE_DEFAULT_DEBUG_CHANNEL(winrt_storage);

struct async_operation_iids
{
    const GUID *operation;
};

extern IActivationFactory *random_access_stream_factory;
extern IActivationFactory *random_access_stream_reference_factory;

typedef HRESULT (WINAPI *async_operation_callback)( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

typedef HRESULT (WINAPI *async_operation_with_progress_callback)( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress );

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
extern HRESULT async_operation_uint32_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperation_UINT32 **out );

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
