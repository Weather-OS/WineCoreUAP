/* WinRT IAsyncOperationWithProgress<UINT32> Implementation
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

#include "../../private.h"
#include "provider.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(wineasync);

struct uint32_async
{
    IAsyncOperation_UINT32 IAsyncOperation_UINT32_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    struct async_operation_iids iids;
    LONG ref;
};

static inline struct uint32_async *impl_from_IAsyncOperation_UINT32( IAsyncOperation_UINT32 *iface )
{
    return CONTAINING_RECORD( iface, struct uint32_async, IAsyncOperation_UINT32_iface );
}

static HRESULT WINAPI uint32_async_QueryInterface( IAsyncOperation_UINT32 *iface, REFIID iid, void **out )
{
    struct uint32_async *impl = impl_from_IAsyncOperation_UINT32( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_UINT32 ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_UINT32_iface) );
        return S_OK;
    }

    if ( IsEqualGUID( iid, impl->iids.operation ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_UINT32_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI uint32_async_AddRef( IAsyncOperation_UINT32 *iface )
{
    struct uint32_async *impl = impl_from_IAsyncOperation_UINT32( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI uint32_async_Release( IAsyncOperation_UINT32 *iface )
{
    struct uint32_async *impl = impl_from_IAsyncOperation_UINT32( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI uint32_async_GetIids( IAsyncOperation_UINT32 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI uint32_async_GetRuntimeClassName( IAsyncOperation_UINT32 *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageItem>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageItem>"),
                                class_name );
}

static HRESULT WINAPI uint32_async_GetTrustLevel( IAsyncOperation_UINT32 *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI uint32_async_put_Completed( IAsyncOperation_UINT32 *iface, IAsyncOperationCompletedHandler_UINT32 *handler )
{
    struct uint32_async *impl = impl_from_IAsyncOperation_UINT32( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI uint32_async_get_Completed( IAsyncOperation_UINT32 *iface, IAsyncOperationCompletedHandler_UINT32 **handler )
{
    struct uint32_async *impl = impl_from_IAsyncOperation_UINT32( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI uint32_async_GetResults( IAsyncOperation_UINT32 *iface, UINT32 *results )
{
    struct uint32_async *impl = impl_from_IAsyncOperation_UINT32( iface );
    PROPVARIANT result = {.vt = VT_UI4};
    HRESULT hr;

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );
    
    //UINT32 Referenced from ulVal
    *results = (UINT32)result.ulVal;
    PropVariantClear( &result );

    return hr;
}

static const struct IAsyncOperation_UINT32Vtbl uint32_async_vtbl =
{
    /* IUnknown methods */
    uint32_async_QueryInterface,
    uint32_async_AddRef,
    uint32_async_Release,
    /* IInspectable methods */
    uint32_async_GetIids,
    uint32_async_GetRuntimeClassName,
    uint32_async_GetTrustLevel,
    /* IAsyncOperation<StorageItem> */
    uint32_async_put_Completed,
    uint32_async_get_Completed,
    uint32_async_GetResults,
};

HRESULT async_operation_uint32_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperation_UINT32 **out )
{
    struct uint32_async *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_UINT32_iface.lpVtbl = &uint32_async_vtbl;
    impl->iids = iids;
    impl->ref = 1;
    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_UINT32_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }
    

    *out = &impl->IAsyncOperation_UINT32_iface;
    TRACE( "created IAsyncOperation_UINT32 %p\n", *out );
    return S_OK;
}
