/* WinRT IAsyncOperationWithProgress<UINT64, UINT64> Implementation
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

struct uint64_async_with_progress
{
    IAsyncOperationWithProgress_UINT64_UINT64 IAsyncOperationWithProgress_UINT64_UINT64_iface;
    IWineAsyncInfoWithProgressImpl *IWineAsyncInfoWithProgressImpl_inner;
    struct async_operation_iids iids;
    LONG ref;
};

static inline struct uint64_async_with_progress *impl_from_IAsyncOperationWithProgress_UINT64_UINT64( IAsyncOperationWithProgress_UINT64_UINT64 *iface )
{
    return CONTAINING_RECORD( iface, struct uint64_async_with_progress, IAsyncOperationWithProgress_UINT64_UINT64_iface );
}

static HRESULT WINAPI uint64_async_with_progress_QueryInterface( IAsyncOperationWithProgress_UINT64_UINT64 *iface, REFIID iid, void **out )
{
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationWithProgress_UINT64_UINT64 ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperationWithProgress_UINT64_UINT64_iface) );
        return S_OK;
    }

    if ( IsEqualGUID( iid, impl->iids.operation ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperationWithProgress_UINT64_UINT64_iface) );
        return S_OK;
    }

    return IWineAsyncInfoWithProgressImpl_QueryInterface( impl->IWineAsyncInfoWithProgressImpl_inner, iid, out );
}

static ULONG WINAPI uint64_async_with_progress_AddRef( IAsyncOperationWithProgress_UINT64_UINT64 *iface )
{
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI uint64_async_with_progress_Release( IAsyncOperationWithProgress_UINT64_UINT64 *iface )
{
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI uint64_async_with_progress_GetIids( IAsyncOperationWithProgress_UINT64_UINT64 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI uint64_async_with_progress_GetRuntimeClassName( IAsyncOperationWithProgress_UINT64_UINT64 *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperationWithProgress`1<UINT64, UINT64>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperationWithProgress`1<UINT64, UINT64>"),
                                class_name );
}

static HRESULT WINAPI uint64_async_with_progress_GetTrustLevel( IAsyncOperationWithProgress_UINT64_UINT64 *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI uint64_async_with_progress_put_Completed( IAsyncOperationWithProgress_UINT64_UINT64 *iface, IAsyncOperationWithProgressCompletedHandler_UINT64_UINT64 *handler )
{
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoWithProgressImpl_put_Completed( impl->IWineAsyncInfoWithProgressImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI uint64_async_with_progress_get_Completed( IAsyncOperationWithProgress_UINT64_UINT64 *iface, IAsyncOperationWithProgressCompletedHandler_UINT64_UINT64 **handler )
{
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoWithProgressImpl_get_Completed( impl->IWineAsyncInfoWithProgressImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI uint64_async_with_progress_put_Progress( IAsyncOperationWithProgress_UINT64_UINT64 *iface, IAsyncOperationProgressHandler_UINT64_UINT64 *progress )
{
    IWineAsyncOperationProgressHandler *handler = (IWineAsyncOperationProgressHandler *)progress;
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, &handler );
    return IWineAsyncInfoWithProgressImpl_put_Progress( impl->IWineAsyncInfoWithProgressImpl_inner, handler );
}

static HRESULT WINAPI uint64_async_with_progress_get_Progress( IAsyncOperationWithProgress_UINT64_UINT64 *iface, IAsyncOperationProgressHandler_UINT64_UINT64 **progress )
{
    IWineAsyncOperationProgressHandler **handler = (IWineAsyncOperationProgressHandler **)progress;
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    IWineAsyncInfoWithProgressImpl_get_Progress( impl->IWineAsyncInfoWithProgressImpl_inner, handler );
    return S_OK;
}

static HRESULT WINAPI uint64_async_with_progress_GetResults( IAsyncOperationWithProgress_UINT64_UINT64 *iface, UINT64 *results )
{
    struct uint64_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT64_UINT64( iface );
    PROPVARIANT result = {.vt = VT_UI4};
    HRESULT hr;

    hr = IWineAsyncInfoWithProgressImpl_get_Result( impl->IWineAsyncInfoWithProgressImpl_inner, &result );
    
    //UINT64 Referenced from ulVal
    *results = (UINT64)result.ulVal;
    PropVariantClear( &result );

    return hr;
}

static const struct IAsyncOperationWithProgress_UINT64_UINT64Vtbl uint64_async_with_progress_vtbl =
{
    /* IUnknown methods */
    uint64_async_with_progress_QueryInterface,
    uint64_async_with_progress_AddRef,
    uint64_async_with_progress_Release,
    /* IInspectable methods */
    uint64_async_with_progress_GetIids,
    uint64_async_with_progress_GetRuntimeClassName,
    uint64_async_with_progress_GetTrustLevel,
    /* IAsyncOperation<StorageItem> */
    uint64_async_with_progress_put_Progress,
    uint64_async_with_progress_get_Progress,
    uint64_async_with_progress_put_Completed,
    uint64_async_with_progress_get_Completed,
    uint64_async_with_progress_GetResults,
};

HRESULT async_operation_with_progress_uint64_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperationWithProgress_UINT64_UINT64 **out )
{
    struct uint64_async_with_progress *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperationWithProgress_UINT64_UINT64_iface.lpVtbl = &uint64_async_with_progress_vtbl;
    impl->iids = iids;
    impl->ref = 1;
    if (FAILED(hr = async_info_with_progress_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperationWithProgress_UINT64_UINT64_iface, &impl->IWineAsyncInfoWithProgressImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoWithProgressImpl_Start( impl->IWineAsyncInfoWithProgressImpl_inner )))
    {
        if (impl->IWineAsyncInfoWithProgressImpl_inner) IWineAsyncInfoWithProgressImpl_Release( impl->IWineAsyncInfoWithProgressImpl_inner );
        free( impl );
        return hr;
    }
    

    *out = &impl->IAsyncOperationWithProgress_UINT64_UINT64_iface;
    TRACE( "created IAsyncOperationWithProgress_UINT64_UINT64 %p\n", *out );
    return S_OK;
}
