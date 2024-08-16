/* WinRT IAsyncOperation<Windows.Storage.StorageItem *> Implementation
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

#include "../../private.h"
#include "provider.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(wineasync);

struct storage_item_async
{
    IAsyncOperation_IStorageItem IAsyncOperation_IStorageItem_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    LONG ref;
};

static inline struct storage_item_async *impl_from_IAsyncOperation_IStorageItem( IAsyncOperation_IStorageItem *iface )
{
    return CONTAINING_RECORD( iface, struct storage_item_async, IAsyncOperation_IStorageItem_iface );
}

static HRESULT WINAPI storage_item_async_QueryInterface( IAsyncOperation_IStorageItem *iface, REFIID iid, void **out )
{
    struct storage_item_async *impl = impl_from_IAsyncOperation_IStorageItem( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_IStorageItem ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_IStorageItem_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI storage_item_async_AddRef( IAsyncOperation_IStorageItem *iface )
{
    struct storage_item_async *impl = impl_from_IAsyncOperation_IStorageItem( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_item_async_Release( IAsyncOperation_IStorageItem *iface )
{
    struct storage_item_async *impl = impl_from_IAsyncOperation_IStorageItem( iface );
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

static HRESULT WINAPI storage_item_async_GetIids( IAsyncOperation_IStorageItem *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_async_GetRuntimeClassName( IAsyncOperation_IStorageItem *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageItem>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageItem>"),
                                class_name );
}

static HRESULT WINAPI storage_item_async_GetTrustLevel( IAsyncOperation_IStorageItem *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_async_put_Completed( IAsyncOperation_IStorageItem *iface, IAsyncOperationCompletedHandler_IStorageItem *handler )
{
    struct storage_item_async *impl = impl_from_IAsyncOperation_IStorageItem( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI storage_item_async_get_Completed( IAsyncOperation_IStorageItem *iface, IAsyncOperationCompletedHandler_IStorageItem **handler )
{
    struct storage_item_async *impl = impl_from_IAsyncOperation_IStorageItem( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI storage_item_async_GetResults( IAsyncOperation_IStorageItem *iface, IStorageItem **results )
{
    struct storage_item_async *impl = impl_from_IAsyncOperation_IStorageItem( iface );
    PROPVARIANT result = {.vt = VT_UNKNOWN};
    HRESULT hr;

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    *results = (IStorageItem *)result.ppunkVal;
    PropVariantClear( &result );
    return hr;
}

static const struct IAsyncOperation_IStorageItemVtbl storage_item_async_vtbl =
{
    /* IUnknown methods */
    storage_item_async_QueryInterface,
    storage_item_async_AddRef,
    storage_item_async_Release,
    /* IInspectable methods */
    storage_item_async_GetIids,
    storage_item_async_GetRuntimeClassName,
    storage_item_async_GetTrustLevel,
    /* IAsyncOperation<StorageItem> */
    storage_item_async_put_Completed,
    storage_item_async_get_Completed,
    storage_item_async_GetResults,
};

HRESULT async_operation_storage_item_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IStorageItem **out )
{
    struct storage_item_async *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_IStorageItem_iface.lpVtbl = &storage_item_async_vtbl;
    impl->ref = 1;
    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_IStorageItem_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }
    

    *out = &impl->IAsyncOperation_IStorageItem_iface;
    TRACE( "created IAsyncOperation_IStorageItem %p\n", *out );
    return S_OK;
}
