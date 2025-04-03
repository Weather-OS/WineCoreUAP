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

extern const struct IAsyncInfoVtbl async_info_vtbl;

#define Closed 4
#define HANDLER_NOT_SET ((void *)~(ULONG_PTR)0)

struct async_info_with_progress
{
    IWineAsyncInfoWithProgressImpl IWineAsyncInfoWithProgressImpl_iface;
    IAsyncInfo IAsyncInfo_iface;
    IInspectable *IInspectable_outer;
    LONG ref;

    async_operation_with_progress_callback callback;
    TP_WORK *async_run_work;
    IUnknown *invoker;
    IUnknown *param;

    CRITICAL_SECTION cs;
    IWineAsyncOperationCompletedHandler *handler;
    IWineAsyncOperationProgressHandler *progress;
    PROPVARIANT result;
    UINT64 progressReport;
    AsyncStatus status;
    HRESULT hr;
};

static inline struct async_info_with_progress *impl_from_IWineAsyncInfoWithProgressImpl( IWineAsyncInfoWithProgressImpl *iface )
{
    return CONTAINING_RECORD( iface, struct async_info_with_progress, IWineAsyncInfoWithProgressImpl_iface );
}

static HRESULT WINAPI async_impl_QueryInterface( IWineAsyncInfoWithProgressImpl *iface, REFIID iid, void **out )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IWineAsyncInfoWithProgressImpl ))
    {
        IInspectable_AddRef( (*out = &impl->IWineAsyncInfoWithProgressImpl_iface) );
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

static ULONG WINAPI async_impl_AddRef( IWineAsyncInfoWithProgressImpl *iface )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_impl_Release( IWineAsyncInfoWithProgressImpl *iface )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
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

static HRESULT WINAPI async_impl_put_Completed( IWineAsyncInfoWithProgressImpl *iface, IWineAsyncOperationCompletedHandler *handler )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
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

static HRESULT WINAPI async_impl_get_Completed( IWineAsyncInfoWithProgressImpl *iface, IWineAsyncOperationCompletedHandler **handler )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p, handler %p.\n", iface, handler );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) hr = E_ILLEGAL_METHOD_CALL;
    if (impl->handler == NULL || impl->handler == HANDLER_NOT_SET) *handler = NULL;
    else IWineAsyncOperationCompletedHandler_AddRef( (*handler = impl->handler) );
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_impl_get_Result( IWineAsyncInfoWithProgressImpl *iface, PROPVARIANT *result )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
    HRESULT hr = E_ILLEGAL_METHOD_CALL;

    TRACE( "iface %p, result %p.\n", iface, result );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Completed || impl->status == Error)
    {
        PropVariantCopy( result, &impl->result );
        hr = impl->hr;
    }
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_impl_put_Progress( IWineAsyncInfoWithProgressImpl *iface, IWineAsyncOperationProgressHandler *progress )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
    HRESULT hr = S_OK;

    TRACE( "iface %p, progress %p.\n", iface, progress );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Closed) hr = E_ILLEGAL_METHOD_CALL;
    else if ( impl->progress ) hr = E_ILLEGAL_DELEGATE_ASSIGNMENT;
    else
    {
        impl->progress = progress;
    }
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_impl_get_Progress( IWineAsyncInfoWithProgressImpl *iface, IWineAsyncOperationProgressHandler **progress )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
    HRESULT hr = E_ILLEGAL_METHOD_CALL;

    TRACE( "iface %p, result %p.\n", iface, progress );

    EnterCriticalSection( &impl->cs );
    if (impl->status == Completed || impl->status == Error)
    {
        *progress = impl->progress;
        hr = impl->hr;
    }
    LeaveCriticalSection( &impl->cs );

    return hr;
}

static HRESULT WINAPI async_impl_Start( IWineAsyncInfoWithProgressImpl *iface )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );

    TRACE( "iface %p.\n", iface );

    /* keep the async alive in the callback */
    IInspectable_AddRef( impl->IInspectable_outer );
    SubmitThreadpoolWork( impl->async_run_work );

    return S_OK;
}

static const struct IWineAsyncInfoWithProgressImplVtbl async_impl_vtbl =
{
    /* IUnknown methods */
    async_impl_QueryInterface,
    async_impl_AddRef,
    async_impl_Release,
    /* IWineAsyncInfoWithProgressImpl */
    async_impl_put_Completed,
    async_impl_get_Completed,
    async_impl_get_Result,
    async_impl_get_Progress,
    async_impl_put_Progress,
    async_impl_Start,
};

static void CALLBACK async_info_with_progress_callback( TP_CALLBACK_INSTANCE *instance, void *iface, TP_WORK *work )
{
    struct async_info_with_progress *impl = impl_from_IWineAsyncInfoWithProgressImpl( iface );
    IInspectable *operation = impl->IInspectable_outer;
    PROPVARIANT result;
    HRESULT hr;

    hr = impl->callback( impl->invoker, impl->param, &result, impl->progress );

    EnterCriticalSection( &impl->cs );
    if (impl->status != Closed) impl->status = FAILED(hr) ? Error : Completed;
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

HRESULT async_info_with_progress_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback,
                                  IInspectable *outer, IWineAsyncInfoWithProgressImpl **out )
{
    struct async_info_with_progress *impl;
    HRESULT hr;

    if (!(impl = calloc( 1, sizeof(struct async_info_with_progress) ))) return E_OUTOFMEMORY;
    impl->IWineAsyncInfoWithProgressImpl_iface.lpVtbl = &async_impl_vtbl;
    impl->IAsyncInfo_iface.lpVtbl = &async_info_vtbl;
    impl->IInspectable_outer = outer;
    impl->ref = 1;

    impl->callback = callback;
    impl->handler = HANDLER_NOT_SET;
    impl->status = Started;

    if (!(impl->async_run_work = CreateThreadpoolWork( async_info_with_progress_callback, &impl->IWineAsyncInfoWithProgressImpl_iface, NULL )))
    {
        hr = HRESULT_FROM_WIN32( GetLastError() );
        free( impl );
        return hr;
    }

    if ((impl->invoker = invoker)) IUnknown_AddRef( impl->invoker );
    //Broken. if ((impl->param = param)) IUnknown_AddRef( impl->param ); 
    impl->param = param;

    InitializeCriticalSectionEx( &impl->cs, 0, RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO );
    impl->cs.DebugInfo->Spare[0] = (DWORD_PTR)( __FILE__ ": async_info_with_progress.cs" );
    
    *out = &impl->IWineAsyncInfoWithProgressImpl_iface;
    return S_OK;
}

struct async_with_progress_inspectable_uint64
{
    IAsyncOperationWithProgress_IInspectable_UINT64 IAsyncOperationWithProgress_IInspectable_UINT64_iface;
    IWineAsyncInfoWithProgressImpl *IWineAsyncInfoWithProgressImpl_inner;
    struct async_operation_iids iids;
    LONG ref;
};

static inline struct async_with_progress_inspectable_uint64 *impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( IAsyncOperationWithProgress_IInspectable_UINT64 *iface )
{
    return CONTAINING_RECORD( iface, struct async_with_progress_inspectable_uint64, IAsyncOperationWithProgress_IInspectable_UINT64_iface );
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_QueryInterface( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, REFIID iid, void **out )
{
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationWithProgress_IInspectable_UINT64 ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperationWithProgress_IInspectable_UINT64_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, impl->iids.operation ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperationWithProgress_IInspectable_UINT64_iface) );
        return S_OK;   
    }

    return IWineAsyncInfoWithProgressImpl_QueryInterface( impl->IWineAsyncInfoWithProgressImpl_inner, iid, out );
}

static ULONG WINAPI async_with_progress_inspectable_uint64_AddRef( IAsyncOperationWithProgress_IInspectable_UINT64 *iface )
{
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI async_with_progress_inspectable_uint64_Release( IAsyncOperationWithProgress_IInspectable_UINT64 *iface )
{
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_GetIids( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_GetRuntimeClassName( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperationWithProgress`1<IInspectable, UINT64>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperationWithProgress`1<IInspectable, UINT64>"),
                                class_name );
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_GetTrustLevel( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_put_Progress( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, IAsyncOperationProgressHandler_IInspectable_UINT64 *progress )
{
    IWineAsyncOperationProgressHandler *handler = (IWineAsyncOperationProgressHandler *)progress;
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, &handler );
    return IWineAsyncInfoWithProgressImpl_put_Progress( impl->IWineAsyncInfoWithProgressImpl_inner, handler );
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_get_Progress( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, IAsyncOperationProgressHandler_IInspectable_UINT64 **progress )
{
    IWineAsyncOperationProgressHandler **handler = (IWineAsyncOperationProgressHandler **)progress;
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    IWineAsyncInfoWithProgressImpl_get_Progress( impl->IWineAsyncInfoWithProgressImpl_inner, handler );
    return S_OK;
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_put_Completed( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, IAsyncOperationWithProgressCompletedHandler_IInspectable_UINT64 *inspectable_handler )
{
    IWineAsyncOperationCompletedHandler *handler = (IWineAsyncOperationCompletedHandler *)inspectable_handler;
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoWithProgressImpl_put_Completed( impl->IWineAsyncInfoWithProgressImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_get_Completed( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, IAsyncOperationWithProgressCompletedHandler_IInspectable_UINT64 **inspectable_handler )
{
    IWineAsyncOperationCompletedHandler **handler = (IWineAsyncOperationCompletedHandler **)inspectable_handler;
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoWithProgressImpl_get_Completed( impl->IWineAsyncInfoWithProgressImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI async_with_progress_inspectable_uint64_GetResults( IAsyncOperationWithProgress_IInspectable_UINT64 *iface, IInspectable **results )
{
    struct async_with_progress_inspectable_uint64 *impl = impl_from_IAsyncOperationWithProgress_IInspectable_UINT64( iface );
    PROPVARIANT result = {.vt = VT_UNKNOWN};
    HRESULT hr;

    TRACE( "iface %p, results %p.\n", iface, results );

    hr = IWineAsyncInfoWithProgressImpl_get_Result( impl->IWineAsyncInfoWithProgressImpl_inner, &result );

    *results = (IInspectable *)result.punkVal;
    PropVariantClear( &result );
    return hr;
}

static const struct IAsyncOperationWithProgress_IInspectable_UINT64Vtbl async_with_progress_inspectable_uint64_vtbl =
{
    /* IUnknown methods */
    async_with_progress_inspectable_uint64_QueryInterface,
    async_with_progress_inspectable_uint64_AddRef,
    async_with_progress_inspectable_uint64_Release,
    /* IInspectable methods */
    async_with_progress_inspectable_uint64_GetIids,
    async_with_progress_inspectable_uint64_GetRuntimeClassName,
    async_with_progress_inspectable_uint64_GetTrustLevel,
    /* IAsyncOperationWithProgress<IInspectable *, UINT64> */    
    async_with_progress_inspectable_uint64_put_Progress,
    async_with_progress_inspectable_uint64_get_Progress,
    async_with_progress_inspectable_uint64_put_Completed,
    async_with_progress_inspectable_uint64_get_Completed,
    async_with_progress_inspectable_uint64_GetResults,
};

HRESULT async_operation_with_progress_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback, const struct async_operation_iids iids,
                                        IAsyncOperationWithProgress_IInspectable_UINT64 **out )
{
    struct async_with_progress_inspectable_uint64 *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperationWithProgress_IInspectable_UINT64_iface.lpVtbl = &async_with_progress_inspectable_uint64_vtbl;
    impl->iids = iids;
    impl->ref = 1;

    if (FAILED(hr = async_info_with_progress_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperationWithProgress_IInspectable_UINT64_iface, &impl->IWineAsyncInfoWithProgressImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoWithProgressImpl_Start( impl->IWineAsyncInfoWithProgressImpl_inner )))
    {
        if (impl->IWineAsyncInfoWithProgressImpl_inner) IWineAsyncInfoWithProgressImpl_Release( impl->IWineAsyncInfoWithProgressImpl_inner );
        free( impl );
        return hr;
    }

    *out = &impl->IAsyncOperationWithProgress_IInspectable_UINT64_iface;
    TRACE( "created IAsyncOperationWithProgress_IInspectable_UINT64 %p\n", *out );
    return S_OK;
}