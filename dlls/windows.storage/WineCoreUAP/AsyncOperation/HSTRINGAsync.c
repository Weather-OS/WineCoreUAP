/* WinRT IAsyncOperation<HSTRING> Implementation
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

struct hstring_async
{
    IAsyncOperation_HSTRING IAsyncOperation_HSTRING_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    LONG ref;
};

static inline struct hstring_async *impl_from_IAsyncOperation_HSTRING( IAsyncOperation_HSTRING *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_async, IAsyncOperation_HSTRING_iface );
}

static HRESULT WINAPI hstring_async_QueryInterface( IAsyncOperation_HSTRING *iface, REFIID iid, void **out )
{
    struct hstring_async *impl = impl_from_IAsyncOperation_HSTRING( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_HSTRING ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_HSTRING_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI hstring_async_AddRef( IAsyncOperation_HSTRING *iface )
{
    struct hstring_async *impl = impl_from_IAsyncOperation_HSTRING( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_async_Release( IAsyncOperation_HSTRING *iface )
{
    struct hstring_async *impl = impl_from_IAsyncOperation_HSTRING( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI hstring_async_GetIids( IAsyncOperation_HSTRING *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_async_GetRuntimeClassName( IAsyncOperation_HSTRING *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageItem>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageItem>"),
                                class_name );
}

static HRESULT WINAPI hstring_async_GetTrustLevel( IAsyncOperation_HSTRING *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_async_put_Completed( IAsyncOperation_HSTRING *iface, IAsyncOperationCompletedHandler_HSTRING *handler )
{
    struct hstring_async *impl = impl_from_IAsyncOperation_HSTRING( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI hstring_async_get_Completed( IAsyncOperation_HSTRING *iface, IAsyncOperationCompletedHandler_HSTRING **handler )
{
    struct hstring_async *impl = impl_from_IAsyncOperation_HSTRING( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI hstring_async_GetResults( IAsyncOperation_HSTRING *iface, HSTRING *results )
{
    struct hstring_async *impl = impl_from_IAsyncOperation_HSTRING( iface );
    PROPVARIANT result = {.vt = VT_LPWSTR};
    HRESULT hr;

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    //HSTRING Referenced from LPWSTR
    WindowsCreateString( result.pwszVal, wcslen( result.pwszVal ), results );
    PropVariantClear( &result );

    return hr;
}

static const struct IAsyncOperation_HSTRINGVtbl hstring_async_vtbl =
{
    /* IUnknown methods */
    hstring_async_QueryInterface,
    hstring_async_AddRef,
    hstring_async_Release,
    /* IInspectable methods */
    hstring_async_GetIids,
    hstring_async_GetRuntimeClassName,
    hstring_async_GetTrustLevel,
    /* IAsyncOperation<StorageItem> */
    hstring_async_put_Completed,
    hstring_async_get_Completed,
    hstring_async_GetResults,
};

HRESULT async_operation_hstring_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_HSTRING **out )
{
    struct hstring_async *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_HSTRING_iface.lpVtbl = &hstring_async_vtbl;
    impl->ref = 1;
    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_HSTRING_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }
    

    *out = &impl->IAsyncOperation_HSTRING_iface;
    TRACE( "created IAsyncOperation_HSTRING %p\n", *out );
    return S_OK;
}
