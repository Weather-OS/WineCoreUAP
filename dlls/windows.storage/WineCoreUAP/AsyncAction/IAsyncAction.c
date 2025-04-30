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
#include "wine/debug.h"

#include "provider.h"

WINE_DEFAULT_DEBUG_CHANNEL(wineasync);

struct async_action
{
    IAsyncAction IAsyncAction_iface;
    IWineAsyncInfoImpl *IWineAsyncInfoImpl_inner;
    LONG refcount;
};

static inline struct async_action *impl_from_IAsyncAction(IAsyncAction *iface)
{
    return CONTAINING_RECORD(iface, struct async_action, IAsyncAction_iface);
}

static HRESULT WINAPI async_action_QueryInterface(IAsyncAction *iface, REFIID iid, void **out)
{
    struct async_action *action = impl_from_IAsyncAction(iface);

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncAction ))
    {
        IInspectable_AddRef( (*out = &action->IAsyncAction_iface) );
        return S_OK;
    }

    return IWineAsyncInfoImpl_QueryInterface( action->IWineAsyncInfoImpl_inner, iid, out );
}

static ULONG WINAPI async_action_AddRef(IAsyncAction *iface)
{
    struct async_action *action = impl_from_IAsyncAction(iface);
    ULONG refcount = InterlockedIncrement(&action->refcount);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static ULONG WINAPI async_action_Release(IAsyncAction *iface)
{
    struct async_action *action = impl_from_IAsyncAction(iface);
    ULONG refcount = InterlockedDecrement(&action->refcount);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    if (!refcount)
        free( action );

    return refcount;
}

static HRESULT WINAPI async_action_GetIids(
        IAsyncAction *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);

    return E_NOTIMPL;
}

static HRESULT WINAPI async_action_GetRuntimeClassName(
        IAsyncAction *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);

    return E_NOTIMPL;
}

static HRESULT WINAPI async_action_GetTrustLevel(
        IAsyncAction *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);

    return E_NOTIMPL;
}

static HRESULT WINAPI async_action_put_Completed(IAsyncAction *iface, IAsyncActionCompletedHandler *handler)
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_put_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler *)handler );
}

static HRESULT WINAPI async_action_get_Completed(IAsyncAction *iface, IAsyncActionCompletedHandler **handler)
{
    struct async_action *impl = impl_from_IAsyncAction( iface );
    TRACE( "iface %p, handler %p.\n", iface, handler );
    return IWineAsyncInfoImpl_get_Completed( impl->IWineAsyncInfoImpl_inner, (IWineAsyncOperationCompletedHandler **)handler );
}

static HRESULT WINAPI async_action_GetResults(IAsyncAction *iface)
{
    //IAsyncAction does not return anything upon completion
    struct async_action *impl = impl_from_IAsyncAction( iface );
    PROPVARIANT result = {.vt = VT_NULL};
    HRESULT hr;

    hr = IWineAsyncInfoImpl_get_Result( impl->IWineAsyncInfoImpl_inner, &result );

    return hr;
}

static const IAsyncActionVtbl async_action_vtbl =
{
    async_action_QueryInterface,
    async_action_AddRef,
    async_action_Release,
    async_action_GetIids,
    async_action_GetRuntimeClassName,
    async_action_GetTrustLevel,
    async_action_put_Completed,
    async_action_get_Completed,
    async_action_GetResults,
};

HRESULT async_action_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, IAsyncAction **ret)
{
    HRESULT hr;
    struct async_action *object;

    *ret = NULL;

    if (!(object = calloc(1, sizeof(*object))))
        return E_OUTOFMEMORY;

    object->IAsyncAction_iface.lpVtbl = &async_action_vtbl;
    object->refcount = 1;

    if (FAILED(hr = async_info_create( invoker, param, callback, (IInspectable *)&object->IAsyncAction_iface, &object->IWineAsyncInfoImpl_inner)) ||
        FAILED(hr = IWineAsyncInfoImpl_Start( object->IWineAsyncInfoImpl_inner)))
    {
        if (object->IWineAsyncInfoImpl_inner) IWineAsyncInfoImpl_Release( object->IWineAsyncInfoImpl_inner );
        free( object );
        return hr;
    }

    *ret = &object->IAsyncAction_iface;
    TRACE( "created IAsyncAction %p\n", *ret );
    return S_OK;
}