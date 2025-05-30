/* CryptoWinRT Implementation
 *
 * Copyright 2022 Bernhard Kölbl for CodeWeavers
 * Copyright 2022 Rémi Bernon for CodeWeavers
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

#include "../../private.h"
#include "provider.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(wineasync);

#define Closed 4
#define HANDLER_NOT_SET ((void *)~(ULONG_PTR)0)

struct async_info
{
    IWineAsyncInfoImpl IWineAsyncInfoImpl_iface;
    IAsyncInfo IAsyncInfo_iface;
    IInspectable *IInspectable_outer;
    LONG ref;

    async_operation_callback callback;
    TP_WORK *async_run_work;
    IUnknown *invoker;
    IUnknown *param;

    CRITICAL_SECTION cs;
    IErrorInfo *errorInfo; //A necessary hack to preserve IRestrictedErrorInfo
    IWineAsyncOperationCompletedHandler *handler;
    PROPVARIANT result;
    AsyncStatus status;
    HRESULT hr;
};

static inline struct async_info *impl_from_IWineAsyncInfoImpl( IWineAsyncInfoImpl *iface )
{
    return CONTAINING_RECORD( iface, struct async_info, IWineAsyncInfoImpl_iface );
}

static HRESULT WINAPI async_impl_QueryInterface( IWineAsyncInfoImpl *iface, REFIID iid, void **out )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IWineAsyncInfoImpl ))
    {
        IInspectable_AddRef( (*out = &impl->IWineAsyncInfoImpl_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IAsyncInfo ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncInfo_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI async_impl_AddRef( IWineAsyncInfoImpl *iface )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_impl_Release( IWineAsyncInfoImpl *iface )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
    {
        if (impl->handler && impl->handler != HANDLER_NOT_SET) IWineAsyncOperationCompletedHandler_Release( impl->handler );
        IAsyncInfo_Close( &impl->IAsyncInfo_iface );
        if (impl->param) IUnknown_Release( impl->param );
        if (impl->invoker) IUnknown_Release( impl->invoker );
        impl->cs.DebugInfo->Spare[0] = 0;
        DeleteCriticalSection( &impl->cs );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI async_impl_put_Completed( IWineAsyncInfoImpl *iface, IWineAsyncOperationCompletedHandler *handler )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p, handler %p.\n", iface, handler );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) hr = E_ILLEGAL_METHOD_CALL;
    else if (impl->handler != HANDLER_NOT_SET) hr = E_ILLEGAL_DELEGATE_ASSIGNMENT;
    else if ((impl->handler = handler))
    {
        IWineAsyncOperationCompletedHandler_AddRef( impl->handler );

        if (impl->status > Started)
        {
            IInspectable *operation = impl->IInspectable_outer;
            AsyncStatus status = impl->status;
            impl->handler = NULL; /* Prevent concurrent invoke. */
            LeaveCriticalSection( &impl->cs );

            IWineAsyncOperationCompletedHandler_Invoke( handler, operation, status );
            IWineAsyncOperationCompletedHandler_Release( handler );

            return S_OK;
        }
    }
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_impl_get_Completed( IWineAsyncInfoImpl *iface, IWineAsyncOperationCompletedHandler **handler )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p, handler %p.\n", iface, handler );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) hr = E_ILLEGAL_METHOD_CALL;
    if (impl->handler == NULL || impl->handler == HANDLER_NOT_SET) *handler = NULL;
    else IWineAsyncOperationCompletedHandler_AddRef( (*handler = impl->handler) );
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_impl_get_Result( IWineAsyncInfoImpl *iface, PROPVARIANT *result )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );
    HRESULT hr = E_ILLEGAL_METHOD_CALL;

    TRACE( "iface %p, result %p.\n", iface, result );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Completed || impl->status == Error)
    {
        PropVariantCopy( result, &impl->result );
        hr = impl->hr;
    }
    // This is where we resubmit the IRestrictedErrorInfo that we received upon AsyncStatus::Error
    if (impl->status == Error && impl->errorInfo)
        SetErrorInfo( 0, impl->errorInfo );

    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_impl_Start( IWineAsyncInfoImpl *iface )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );

    TRACE( "iface %p.\n", iface );

    /* keep the async alive in the callback */
    IInspectable_AddRef( impl->IInspectable_outer );
    SubmitThreadpoolWork( impl->async_run_work );

    return S_OK;
}

static const struct IWineAsyncInfoImplVtbl async_impl_vtbl =
{
    /* IUnknown methods */
    async_impl_QueryInterface,
    async_impl_AddRef,
    async_impl_Release,
    /* IWineAsyncInfoImpl */
    async_impl_put_Completed,
    async_impl_get_Completed,
    async_impl_get_Result,
    async_impl_Start,
};

DEFINE_IINSPECTABLE_OUTER( async_info, IAsyncInfo, struct async_info, IInspectable_outer )

static HRESULT WINAPI async_info_get_Id( IAsyncInfo *iface, UINT32 *id )
{
    struct async_info *impl = impl_from_IAsyncInfo( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p, id %p.\n", iface, id );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) hr = E_ILLEGAL_METHOD_CALL;
    *id = 1;
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_info_get_Status( IAsyncInfo *iface, AsyncStatus *status )
{
    struct async_info *impl = impl_from_IAsyncInfo( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p, status %p.\n", iface, status );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) hr = E_ILLEGAL_METHOD_CALL;
    *status = impl->status;
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_info_get_ErrorCode( IAsyncInfo *iface, HRESULT *error_code )
{
    struct async_info *impl = impl_from_IAsyncInfo( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p, error_code %p.\n", iface, error_code );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) *error_code = hr = E_ILLEGAL_METHOD_CALL;
    else *error_code = impl->hr;
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_info_Cancel( IAsyncInfo *iface )
{
    struct async_info *impl = impl_from_IAsyncInfo( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p.\n", iface );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) hr = E_ILLEGAL_METHOD_CALL;
    else if (impl->status == Started) impl->status = Canceled;
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_info_Close( IAsyncInfo *iface )
{
    struct async_info *impl = impl_from_IAsyncInfo( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p.\n", iface );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Started)
        hr = E_ILLEGAL_STATE_CHANGE;
    else if (impl->status != Closed)
    {
        CloseThreadpoolWork( impl->async_run_work );
        impl->async_run_work = NULL;
        impl->status = Closed;
    }
    LeaveCriticalSection( &impl->cs );

    return hr;
}

const struct IAsyncInfoVtbl async_info_vtbl =
{
    /* IUnknown methods */
    async_info_QueryInterface,
    async_info_AddRef,
    async_info_Release,
    /* IInspectable methods */
    async_info_GetIids,
    async_info_GetRuntimeClassName,
    async_info_GetTrustLevel,
    /* IAsyncInfo */
    async_info_get_Id,
    async_info_get_Status,
    async_info_get_ErrorCode,
    async_info_Cancel,
    async_info_Close,
};

static void CALLBACK async_info_callback( TP_CALLBACK_INSTANCE *instance, void *iface, TP_WORK *work )
{
    struct async_info *impl = impl_from_IWineAsyncInfoImpl( iface );
    IInspectable *operation = impl->IInspectable_outer;
    PROPVARIANT result;
    HRESULT hr;

    hr = impl->callback( impl->invoker, impl->param, &result );

    EnterCriticalSection( &impl->cs );
    if (impl->status != Closed) impl->status = FAILED(hr) ? Error : Completed;
    if ( impl->status == Error )
        GetErrorInfo( 0, &impl->errorInfo );

    PropVariantCopy( &impl->result, &result );
    impl->hr = hr;

    if (impl->handler != NULL && impl->handler != HANDLER_NOT_SET)
    {
        IWineAsyncOperationCompletedHandler *handler = impl->handler;
        AsyncStatus status = impl->status;
        impl->handler = NULL; /* Prevent concurrent invoke. */
        LeaveCriticalSection( &impl->cs );

        IWineAsyncOperationCompletedHandler_Invoke( handler, operation, status );
        IWineAsyncOperationCompletedHandler_Release( handler );
    }
    else LeaveCriticalSection( &impl->cs );

    /* release refcount acquired in Start */
    IInspectable_Release( operation );

    PropVariantClear( &result );
}

HRESULT async_info_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                  IInspectable *outer, IWineAsyncInfoImpl **out )
{
    struct async_info *impl;
    HRESULT hr;

    if (!(impl = calloc( 1, sizeof(struct async_info) ))) return E_OUTOFMEMORY;
    impl->IWineAsyncInfoImpl_iface.lpVtbl = &async_impl_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_info_vtbl;
    impl->IInspectable_outer = outer;
    impl->ref = 1;

    impl->callback = callback;
    impl->handler = HANDLER_NOT_SET;
    impl->status = Started;

    if (!(impl->async_run_work = CreateThreadpoolWork( async_info_callback, &impl->IWineAsyncInfoImpl_iface, NULL )))
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
        free( impl );
        return hr;
    }

    if ((impl->invoker = invoker)) IUnknown_AddRef( impl->invoker );
    //Broken. if ((impl->param = param)) IUnknown_AddRef( impl->param ); 
    impl->param = param;

    InitializeCriticalSectionEx( &impl->cs, 0, RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO );
    impl->cs.DebugInfo->Spare[0] = (DWORD_PTR)( __FILE__ ": async_info.cs" );
    
    *out = &impl->IWineAsyncInfoImpl_iface;
    return S_OK;
}

struct async_bool
{
    IAsyncOperation_boolean IAsyncOperation_boolean_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    LONG ref;
};

static inline struct async_bool *impl_from_IAsyncOperation_boolean( IAsyncOperation_boolean *iface )
{
    return CONTAINING_RECORD( iface, struct async_bool, IAsyncOperation_boolean_iface );
}

static HRESULT WINAPI async_bool_QueryInterface( IAsyncOperation_boolean *iface, REFIID iid, void **out )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_boolean ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_boolean_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI async_bool_AddRef( IAsyncOperation_boolean *iface )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_bool_Release( IAsyncOperation_boolean *iface )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI async_bool_GetIids( IAsyncOperation_boolean *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_bool_GetRuntimeClassName( IAsyncOperation_boolean *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Boolean>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Boolean>"),
                                class_name );
}

static HRESULT WINAPI async_bool_GetTrustLevel( IAsyncOperation_boolean *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_bool_put_Completed( IAsyncOperation_boolean *iface, IAsyncOperationCompletedHandler_boolean *bool_handler )
{
    IWineAsyncOperationCompletedHandler *handler = (IWineAsyncOperationCompletedHandler *)bool_handler;
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI async_bool_get_Completed( IAsyncOperation_boolean *iface, IAsyncOperationCompletedHandler_boolean **bool_handler )
{
    IWineAsyncOperationCompletedHandler **handler = (IWineAsyncOperationCompletedHandler **)bool_handler;
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI async_bool_GetResults( IAsyncOperation_boolean *iface, BOOLEAN *results )
{
    struct async_bool *impl = impl_from_IAsyncOperation_boolean( iface );
    PROPVARIANT result = {.vt = VT_BOOL};
    HRESULT hr;

    TRACE( "iface %p, results %p.\n", iface, results );

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    *results = result.boolVal;
    PropVariantClear( &result );
    return hr;
}

static const struct IAsyncOperation_booleanVtbl async_bool_vtbl =
{
    /* IUnknown methods */
    async_bool_QueryInterface,
    async_bool_AddRef,
    async_bool_Release,
    /* IInspectable methods */
    async_bool_GetIids,
    async_bool_GetRuntimeClassName,
    async_bool_GetTrustLevel,
    /* IAsyncOperation<boolean> */
    async_bool_put_Completed,
    async_bool_get_Completed,
    async_bool_GetResults,
};

struct async_inspectable
{
    IAsyncOperation_IInspectable IAsyncOperation_IInspectable_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    struct async_operation_iids iids;
    LONG ref;
};

static inline struct async_inspectable *impl_from_IAsyncOperation_IInspectable( IAsyncOperation_IInspectable *iface )
{
    return CONTAINING_RECORD( iface, struct async_inspectable, IAsyncOperation_IInspectable_iface );
}

static HRESULT WINAPI async_inspectable_QueryInterface( IAsyncOperation_IInspectable *iface, REFIID iid, void **out )
{
    struct async_inspectable *impl = impl_from_IAsyncOperation_IInspectable( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_IInspectable ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_IInspectable_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, impl->iids.operation ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_IInspectable_iface) );
        return S_OK;   
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI async_inspectable_AddRef( IAsyncOperation_IInspectable *iface )
{
    struct async_inspectable *impl = impl_from_IAsyncOperation_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_inspectable_Release( IAsyncOperation_IInspectable *iface )
{
    struct async_inspectable *impl = impl_from_IAsyncOperation_IInspectable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI async_inspectable_GetIids( IAsyncOperation_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_inspectable_GetRuntimeClassName( IAsyncOperation_IInspectable *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<IInspectable>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<IInspectable>"),
                                class_name );
}

static HRESULT WINAPI async_inspectable_GetTrustLevel( IAsyncOperation_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_inspectable_put_Completed( IAsyncOperation_IInspectable *iface, IAsyncOperationCompletedHandler_IInspectable *inspectable_handler )
{
    IWineAsyncOperationCompletedHandler *handler = (IWineAsyncOperationCompletedHandler *)inspectable_handler;
    struct async_inspectable *impl = impl_from_IAsyncOperation_IInspectable( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI async_inspectable_get_Completed( IAsyncOperation_IInspectable *iface, IAsyncOperationCompletedHandler_IInspectable **inspectable_handler )
{
    IWineAsyncOperationCompletedHandler **handler = (IWineAsyncOperationCompletedHandler **)inspectable_handler;
    struct async_inspectable *impl = impl_from_IAsyncOperation_IInspectable( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI async_inspectable_GetResults( IAsyncOperation_IInspectable *iface, IInspectable **results )
{
    struct async_inspectable *impl = impl_from_IAsyncOperation_IInspectable( iface );
    PROPVARIANT result = {.vt = VT_UNKNOWN};
    HRESULT hr;

    TRACE( "iface %p, results %p.\n", iface, results );

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    *results = (IInspectable *)result.punkVal;
    PropVariantClear( &result );
    return hr;
}

static const struct IAsyncOperation_IInspectableVtbl async_inspectable_vtbl =
{
    /* IUnknown methods */
    async_inspectable_QueryInterface,
    async_inspectable_AddRef,
    async_inspectable_Release,
    /* IInspectable methods */
    async_inspectable_GetIids,
    async_inspectable_GetRuntimeClassName,
    async_inspectable_GetTrustLevel,
    /* IAsyncOperation<boolean> */
    async_inspectable_put_Completed,
    async_inspectable_get_Completed,
    async_inspectable_GetResults,
};

HRESULT async_operation_boolean_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                        IAsyncOperation_boolean **out )
{
    struct async_bool *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_boolean_iface.lpVtbl = &async_bool_vtbl;
    impl->ref = 1;

    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_boolean_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }

    *out = &impl->IAsyncOperation_boolean_iface;
    TRACE( "created IAsyncOperation_boolean %p\n", *out );
    return S_OK;
}

HRESULT async_operation_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, const struct async_operation_iids iids,
                                        IAsyncOperation_IInspectable **out )
{
    struct async_inspectable *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_IInspectable_iface.lpVtbl = &async_inspectable_vtbl;
    impl->iids = iids;
    impl->ref = 1;

    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_IInspectable_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }

    *out = &impl->IAsyncOperation_IInspectable_iface;
    TRACE( "created IAsyncOperation_IInspectable %p\n", *out );
    return S_OK;
}