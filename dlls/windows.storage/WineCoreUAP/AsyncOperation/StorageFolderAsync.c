/* WinRT IAsyncOperation<Windows.Storage.StorageFolder *> Implementation
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

struct storage_folder_async
{
    IAsyncOperation_StorageFolder IAsyncOperation_StorageFolder_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    LONG ref;
};

static inline struct storage_folder_async *impl_from_IAsyncOperation_StorageFolder( IAsyncOperation_StorageFolder *iface )
{
    return CONTAINING_RECORD( iface, struct storage_folder_async, IAsyncOperation_StorageFolder_iface );
}

static HRESULT WINAPI storage_folder_async_QueryInterface( IAsyncOperation_StorageFolder *iface, REFIID iid, void **out )
{
    struct storage_folder_async *impl = impl_from_IAsyncOperation_StorageFolder( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperation_StorageFolder ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperation_StorageFolder_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( impl->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI storage_folder_async_AddRef( IAsyncOperation_StorageFolder *iface )
{
    struct storage_folder_async *impl = impl_from_IAsyncOperation_StorageFolder( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_folder_async_Release( IAsyncOperation_StorageFolder *iface )
{
    struct storage_folder_async *impl = impl_from_IAsyncOperation_StorageFolder( iface );
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

static HRESULT WINAPI storage_folder_async_GetIids( IAsyncOperation_StorageFolder *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_async_GetRuntimeClassName( IAsyncOperation_StorageFolder *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageFolder>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperation`1<Windows.Storage.StorageFolder>"),
                                class_name );
}

static HRESULT WINAPI storage_folder_async_GetTrustLevel( IAsyncOperation_StorageFolder *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_folder_async_put_Completed( IAsyncOperation_StorageFolder *iface, IAsyncOperationCompletedHandler_StorageFolder *handler )
{
    struct storage_folder_async *impl = impl_from_IAsyncOperation_StorageFolder( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI storage_folder_async_get_Completed( IAsyncOperation_StorageFolder *iface, IAsyncOperationCompletedHandler_StorageFolder **handler )
{
    struct storage_folder_async *impl = impl_from_IAsyncOperation_StorageFolder( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI storage_folder_async_GetResults( IAsyncOperation_StorageFolder *iface, IStorageFolder **results )
{
    struct storage_folder_async *impl = impl_from_IAsyncOperation_StorageFolder( iface );
    PROPVARIANT result = {.vt = VT_UNKNOWN};
    HRESULT hr;

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    *results = (IStorageFolder *)result.ppunkVal;
    PropVariantClear( &result );
    return hr;
}

static const struct IAsyncOperation_StorageFolderVtbl storage_folder_async_vtbl =
{
    /* IUnknown methods */
    storage_folder_async_QueryInterface,
    storage_folder_async_AddRef,
    storage_folder_async_Release,
    /* IInspectable methods */
    storage_folder_async_GetIids,
    storage_folder_async_GetRuntimeClassName,
    storage_folder_async_GetTrustLevel,
    /* IAsyncOperation<StorageFolder> */
    storage_folder_async_put_Completed,
    storage_folder_async_get_Completed,
    storage_folder_async_GetResults,
};

HRESULT async_operation_storage_folder_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_StorageFolder **out )
{
    struct storage_folder_async *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperation_StorageFolder_iface.lpVtbl = &storage_folder_async_vtbl;
    impl->ref = 1;
    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperation_StorageFolder_iface, &impl->IWineAsyncInfoImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( impl->IWineAsyncInfoImpl_inner )))
    {
        if (impl->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( impl->IWineAsyncInfoImpl_inner );
        free( impl );
        return hr;
    }
    

    *out = &impl->IAsyncOperation_StorageFolder_iface;
    TRACE( "created IAsyncOperation_StorageFolder %p\n", *out );
    return S_OK;
}
