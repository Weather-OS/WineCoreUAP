/* WinRT IAsyncOperation<Windows.Storage.FileProperties.BasicProperties *> Implementation
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

#include "../../../private.h"
#include "provider.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(wineasync);

struct basic_properties_async
{
    IAsyncOperation_BasicProperties IAsyncOperation_BasicProperties_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    LONG ref;
};

static inline struct basic_properties_async *impl_from_IAsyncOperation_BasicProperties( IAsyncOperation_BasicProperties *iface )
{
    return CONTAINING_RECORD( iface, struct basic_properties_async, IAsyncOperation_BasicProperties_iface );
}

static HRESULT WINAPI basic_properties_async_QueryInterface( IAsyncOperation_BasicProperties *iface, REFIID iid, void **out )
{
    struct basic_properties_async *impl = impl_from_IAsyncOperation_BasicProperties( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_BasicProperties ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_BasicProperties_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI basic_properties_async_AddRef( IAsyncOperation_BasicProperties *iface )
{
    struct basic_properties_async *impl = impl_from_IAsyncOperation_BasicProperties( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI basic_properties_async_Release( IAsyncOperation_BasicProperties *iface )
{
    struct basic_properties_async *impl = impl_from_IAsyncOperation_BasicProperties( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI basic_properties_async_GetIids( IAsyncOperation_BasicProperties *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI basic_properties_async_GetRuntimeClassName( IAsyncOperation_BasicProperties *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.BasicProperties>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.BasicProperties>"),
                                class_name );
}

static HRESULT WINAPI basic_properties_async_GetTrustLevel( IAsyncOperation_BasicProperties *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI basic_properties_async_put_Completed( IAsyncOperation_BasicProperties *iface, IAsyncOperationCompletedHandler_BasicProperties *handler )
{
    struct basic_properties_async *impl = impl_from_IAsyncOperation_BasicProperties( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI basic_properties_async_get_Completed( IAsyncOperation_BasicProperties *iface, IAsyncOperationCompletedHandler_BasicProperties **handler )
{
    struct basic_properties_async *impl = impl_from_IAsyncOperation_BasicProperties( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI basic_properties_async_GetResults( IAsyncOperation_BasicProperties *iface, IBasicProperties **results )
{
    struct basic_properties_async *impl = impl_from_IAsyncOperation_BasicProperties( iface );
    PROPVARIANT result = {.vt = VT_UNKNOWN};
    HRESULT hr;

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    *results = (IBasicProperties *)result.ppunkVal;
    PropVariantClear( &result );
    return hr;
}

static const struct IAsyncOperation_BasicPropertiesVtbl basic_properties_async_vtbl =
{
    /* IUnknown methods */
    basic_properties_async_QueryInterface,
    basic_properties_async_AddRef,
    basic_properties_async_Release,
    /* IInspectable methods */
    basic_properties_async_GetIids,
    basic_properties_async_GetRuntimeClassName,
    basic_properties_async_GetTrustLevel,
    /* IAsyncOperation<BasicProperties> */
    basic_properties_async_put_Completed,
    basic_properties_async_get_Completed,
    basic_properties_async_GetResults,
};

HRESULT async_operation_basic_properties_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_BasicProperties **out )
{
    struct basic_properties_async *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_BasicProperties_iface.lpVtbl = &basic_properties_async_vtbl;
    impl->ref = 1;
    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_BasicProperties_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }
    

    *out = &impl->IAsyncOperation_BasicProperties_iface;
    TRACE( "created IAsyncOperation_BasicProperties %p\n", *out );
    return S_OK;
}
