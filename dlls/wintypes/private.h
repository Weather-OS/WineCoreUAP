/*
 * Copyright 2022-2024 Zhiyi Zhang for CodeWeavers
 * Copyright 2025 Jactry Zeng for CodeWeavers
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

#ifndef __WINE_WINETYPES_H
#define __WINE_WINETYPES_H

#include <stdarg.h>

#define COBJMACROS
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "wine/debug.h"
#include "objbase.h"
#include "roerrorapi.h"
#include "restrictederrorinfo.h"

#include "activation.h"
#include "rometadataresolution.h"

#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#include "windows.foundation.h"
#define WIDL_using_Windows_Foundation_Metadata
#include "windows.foundation.metadata.h"
#define WIDL_using_Windows_Storage
#define WIDL_using_Windows_Storage_Streams
#include "windows.media.h"
#include "windows.storage.streams.h"
#include "wintypes_private.h"

#include "robuffer.h"

#include "provider.h"

extern IActivationFactory *property_set_factory;
extern IActivationFactory *data_reader_factory;
extern IActivationFactory *data_writer_factory;
extern IActivationFactory *buffer_factory;

#define _ENABLE_DEBUGGING_ WINE_DEFAULT_DEBUG_CHANNEL(wintypes);

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

//All of this fits together to form IPropertySet and IValueSet
struct property_set
{
    IPropertySet IPropertySet_iface;
    //Inheritence tree
        IObservableMap_HSTRING_IInspectable *map;
    LONG ref;
};

typedef HRESULT (WINAPI *async_operation_callback)( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

typedef HRESULT (WINAPI *observable_hstring_map_callback)( IObservableMap_HSTRING_IInspectable *invoker, IUnknown *data, IMapChangedEventArgs_HSTRING *args );

extern HRESULT async_info_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, 
    IInspectable *outer, IWineAsyncInfoImpl **out );
extern HRESULT async_operation_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, const struct async_operation_iids iids,
    IAsyncOperation_IInspectable **out );
extern HRESULT async_operation_uint32_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, const struct async_operation_iids iids,
    IAsyncOperation_UINT32 **out );

extern HRESULT vector_create( const struct vector_iids *iids, void **out );

extern HRESULT observable_vector_create( const struct vector_iids *iids, void **out );

extern HRESULT hstring_map_create( const struct map_iids *iids, void **out );

extern HRESULT observable_hstring_map_create( const struct map_iids *iids, void **out );

extern HRESULT hstring_map_event_handler_create( observable_hstring_map_callback callback, OPTIONAL IUnknown *data, IMapChangedEventHandler_HSTRING_IInspectable **out );

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

#define DEFINE_IUNKNOWN( pfx, iface_type, impl_type, base_iface )                                  \
    DEFINE_IUNKNOWN_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, &impl->base_iface )
#define DEFINE_IINSPECTABLE_OUTER( pfx, iface_type, impl_type, outer_iface )                       \
    DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, impl->outer_iface )

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