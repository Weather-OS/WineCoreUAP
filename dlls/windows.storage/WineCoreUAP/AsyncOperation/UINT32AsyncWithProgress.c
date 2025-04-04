/* WinRT IAsyncOperationWithProgress<UINT32, UINT32> Implementation
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

struct uint32_async_with_progress
{
    IAsyncOperationWithProgress_UINT32_UINT32 IAsyncOperationWithProgress_UINT32_UINT32_iface;
    IWineAsyncInfoWithProgressImpl *IWineAsyncInfoWithProgressImpl_inner;
    struct async_operation_iids iids;
    LONG ref;
};

static inline struct uint32_async_with_progress *impl_from_IAsyncOperationWithProgress_UINT32_UINT32( IAsyncOperationWithProgress_UINT32_UINT32 *iface )
{
    return CONTAINING_RECORD( iface, struct uint32_async_with_progress, IAsyncOperationWithProgress_UINT32_UINT32_iface );
}

static HRESULT WINAPI uint32_async_with_progress_QueryInterface( IAsyncOperationWithProgress_UINT32_UINT32 *iface, REFIID iid, void **out )
{
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncOperationWithProgress_UINT32_UINT32 ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperationWithProgress_UINT32_UINT32_iface) );
        return S_OK;
    }

    if ( IsEqualGUID( iid, impl->iids.operation ))
    {
        IInspectable_AddRef( (*out = &impl->IAsyncOperationWithProgress_UINT32_UINT32_iface) );
        return S_OK;
    }

    return IWineAsyncInfoWithProgressImpl_QueryInterface( impl->IWineAsyncInfoWithProgressImpl_inner, iid, out );
}

static ULONG WINAPI uint32_async_with_progress_AddRef( IAsyncOperationWithProgress_UINT32_UINT32 *iface )
{
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI uint32_async_with_progress_Release( IAsyncOperationWithProgress_UINT32_UINT32 *iface )
{
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p, ref %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI uint32_async_with_progress_GetIids( IAsyncOperationWithProgress_UINT32_UINT32 *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI uint32_async_with_progress_GetRuntimeClassName( IAsyncOperationWithProgress_UINT32_UINT32 *iface, HSTRING *class_name )
{
    return WindowsCreateString( L"Windows.Foundation.IAsyncOperationWithProgress`1<UINT32, UINT32>",
                                ARRAY_SIZE(L"Windows.Foundation.IAsyncOperationWithProgress`1<UINT32, UINT32>"),
                                class_name );
}

static HRESULT WINAPI uint32_async_with_progress_GetTrustLevel( IAsyncOperationWithProgress_UINT32_UINT32 *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI uint32_async_with_progress_put_Completed( IAsyncOperationWithProgress_UINT32_UINT32 *iface, IAsyncOperationWithProgressCompletedHandler_UINT32_UINT32 *handler )
{
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoWithProgressImpl_put_Completed( impl->IWineAsyncInfoWithProgressImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI uint32_async_with_progress_get_Completed( IAsyncOperationWithProgress_UINT32_UINT32 *iface, IAsyncOperationWithProgressCompletedHandler_UINT32_UINT32 **handler )
{
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoWithProgressImpl_get_Completed( impl->IWineAsyncInfoWithProgressImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI uint32_async_with_progress_put_Progress( IAsyncOperationWithProgress_UINT32_UINT32 *iface, IAsyncOperationProgressHandler_UINT32_UINT32 *progress )
{
    IWineAsyncOperationProgressHandler *handler = (IWineAsyncOperationProgressHandler *)progress;
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );
    TRACE( "iface %p, handler %p.\n", iface, &handler );
    return IWineAsyncInfoWithProgressImpl_put_Progress( impl->IWineAsyncInfoWithProgressImpl_inner, handler );
}

static HRESULT WINAPI uint32_async_with_progress_get_Progress( IAsyncOperationWithProgress_UINT32_UINT32 *iface, IAsyncOperationProgressHandler_UINT32_UINT32 **progress )
{
    IWineAsyncOperationProgressHandler **handler = (IWineAsyncOperationProgressHandler **)progress;
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    IWineAsyncInfoWithProgressImpl_get_Progress( impl->IWineAsyncInfoWithProgressImpl_inner, handler );
    return S_OK;
}

static HRESULT WINAPI uint32_async_with_progress_GetResults( IAsyncOperationWithProgress_UINT32_UINT32 *iface, UINT32 *results )
{
    struct uint32_async_with_progress *impl = impl_from_IAsyncOperationWithProgress_UINT32_UINT32( iface );
    PROPVARIANT result = {.vt = VT_UI4};
    HRESULT hr;

    hr = IWineAsyncInfoWithProgressImpl_get_Result( impl->IWineAsyncInfoWithProgressImpl_inner, &result );
    
    //UINT32 Referenced from ulVal
    *results = (UINT32)result.ulVal;
    PropVariantClear( &result );

    return hr;
}

static const struct IAsyncOperationWithProgress_UINT32_UINT32Vtbl uint32_async_with_progress_vtbl =
{
    /* IUnknown methods */
    uint32_async_with_progress_QueryInterface,
    uint32_async_with_progress_AddRef,
    uint32_async_with_progress_Release,
    /* IInspectable methods */
    uint32_async_with_progress_GetIids,
    uint32_async_with_progress_GetRuntimeClassName,
    uint32_async_with_progress_GetTrustLevel,
    /* IAsyncOperation<StorageItem> */
    uint32_async_with_progress_put_Progress,
    uint32_async_with_progress_get_Progress,
    uint32_async_with_progress_put_Completed,
    uint32_async_with_progress_get_Completed,
    uint32_async_with_progress_GetResults,
};

HRESULT async_operation_with_progress_uint32_create( IUnknown *invoker, IUnknown *param, async_operation_with_progress_callback callback, const struct async_operation_iids iids,
                                              IAsyncOperationWithProgress_UINT32_UINT32 **out )
{
    struct uint32_async_with_progress *impl;
    HRESULT hr;

    *out = NULL;
    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IAsyncOperationWithProgress_UINT32_UINT32_iface.lpVtbl = &uint32_async_with_progress_vtbl;
    impl->iids = iids;
    impl->ref = 1;

    if (FAILED(hr = async_info_with_progress_create( invoker, param, callback, (IInspectable *)&impl->IAsyncOperationWithProgress_UINT32_UINT32_iface, &impl->IWineAsyncInfoWithProgressImpl_inner )) ||
        FAILED(hr = IWineAsyncInfoWithProgressImpl_Start( impl->IWineAsyncInfoWithProgressImpl_inner )))
    {
        if (impl->IWineAsyncInfoWithProgressImpl_inner) IWineAsyncInfoWithProgressImpl_Release( impl->IWineAsyncInfoWithProgressImpl_inner );
        free( impl );
        return hr;
    }

    *out = &impl->IAsyncOperationWithProgress_UINT32_UINT32_iface;
    TRACE( "created IAsyncOperationWithProgress_UINT32_UINT32 %p\n", *out );
    return S_OK;
}
