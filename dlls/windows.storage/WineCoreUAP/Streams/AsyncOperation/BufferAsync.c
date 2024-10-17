/* WinRT IAsyncOperation<Windows.Storage.Streams.IBuffer *> Implementation
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

#include "../../../private.h"
#include "provider.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(wineasync);

struct buffer_async
{
    IAsyncOperation_IBuffer IAsyncOperation_IBuffer_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    LONG ref;
};

static inline struct buffer_async *impl_from_IAsyncOperation_IBuffer( IAsyncOperation_IBuffer *iface )
{
    return CONTAINING_RECORD( iface, struct buffer_async, IAsyncOperation_IBuffer_iface );
}

static HRESULT WINAPI buffer_async_QueryInterface( IAsyncOperation_IBuffer *iface, REFIID iid, void **out )
{
    struct buffer_async *impl = impl_from_IAsyncOperation_IBuffer( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_IBuffer ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_IBuffer_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI buffer_async_AddRef( IAsyncOperation_IBuffer *iface )
{
    struct buffer_async *impl = impl_from_IAsyncOperation_IBuffer( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI buffer_async_Release( IAsyncOperation_IBuffer *iface )
{
    struct buffer_async *impl = impl_from_IAsyncOperation_IBuffer( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
    {
        /* guard against re-entry if inner releases an outer iface */
        InterlockedIncrement( &impl->ref );
        IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI buffer_async_GetIids( IAsyncOperation_IBuffer *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_async_GetRuntimeClassName( IAsyncOperation_IBuffer *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.Streams.Buffer>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.Streams.Buffer>"),
                                class_name );
}

static HRESULT WINAPI buffer_async_GetTrustLevel( IAsyncOperation_IBuffer *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_async_put_Completed( IAsyncOperation_IBuffer *iface, IAsyncOperationCompletedHandler_IBuffer *handler )
{
    struct buffer_async *impl = impl_from_IAsyncOperation_IBuffer( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI buffer_async_get_Completed( IAsyncOperation_IBuffer *iface, IAsyncOperationCompletedHandler_IBuffer **handler )
{
    struct buffer_async *impl = impl_from_IAsyncOperation_IBuffer( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI buffer_async_GetResults( IAsyncOperation_IBuffer *iface, IBuffer **results )
{
    struct buffer_async *impl = impl_from_IAsyncOperation_IBuffer( iface );
    PROPVARIANT result = {.vt = VT_UNKNOWN};
    HRESULT hr;

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    *results = (IBuffer *)result.ppunkVal;
    PropVariantClear( &result );
    return hr;
}

static const struct IAsyncOperation_IBufferVtbl buffer_async_vtbl =
{
    /* IUnknown methods */
    buffer_async_QueryInterface,
    buffer_async_AddRef,
    buffer_async_Release,
    /* IInspectable methods */
    buffer_async_GetIids,
    buffer_async_GetRuntimeClassName,
    buffer_async_GetTrustLevel,
    /* IAsyncOperation<Buffer> */
    buffer_async_put_Completed,
    buffer_async_get_Completed,
    buffer_async_GetResults,
};

HRESULT async_operation_buffer_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IBuffer **out )
{
    struct buffer_async *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_IBuffer_iface.lpVtbl = &buffer_async_vtbl;
    impl->ref = 1;
    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_IBuffer_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }
    

    *out = &impl->IAsyncOperation_IBuffer_iface;
    TRACE( "created IAsyncOperation_IBuffer %p\n", *out );
    return S_OK;
}
