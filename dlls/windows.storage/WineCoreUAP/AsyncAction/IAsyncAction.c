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

WINE_DEFAULT_DEBUG_CHANNEL(wineasync);

struct async_action
{
    IAsyncAction IAsyncAction_iface;
    IAsyncInfo IAsyncInfo_iface;
    LONG refcount;
};

static inline struct async_action *impl_from_IAsyncAction(IAsyncAction *iface)
{
    return CONTAINING_RECORD(iface, struct async_action, IAsyncAction_iface);
}

static inline struct async_action *impl_from_IAsyncInfo(IAsyncInfo *iface)
{
    return CONTAINING_RECORD(iface, struct async_action, IAsyncInfo_iface);
}

static HRESULT STDMETHODCALLTYPE async_action_QueryInterface(IAsyncAction *iface, REFIID iid, void **out)
{
    struct async_action *action = impl_from_IAsyncAction(iface);

    TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

    if (IsEqualIID(iid, &IID_IAsyncAction)
            || IsEqualIID(iid, &IID_IInspectable)
            || IsEqualIID(iid, &IID_IUnknown))
    {
        *out = iface;
    }
    else if (IsEqualIID(iid, &IID_IAsyncInfo))
    {
        *out = &action->IAsyncInfo_iface;
    }
    else
    {
        *out = NULL;
        WARN("Unsupported interface %s.\n", debugstr_guid(iid));
    }

    IUnknown_AddRef((IUnknown *)*out);
    return S_OK;
}

static ULONG STDMETHODCALLTYPE async_action_AddRef(IAsyncAction *iface)
{
    struct async_action *action = impl_from_IAsyncAction(iface);
    ULONG refcount = InterlockedIncrement(&action->refcount);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static ULONG STDMETHODCALLTYPE async_action_Release(IAsyncAction *iface)
{
    struct async_action *action = impl_from_IAsyncAction(iface);
    ULONG refcount = InterlockedDecrement(&action->refcount);

    TRACE("iface %p, refcount %lu.\n", iface, refcount);

    if (!refcount)
        free(action);

    return refcount;
}

static HRESULT STDMETHODCALLTYPE async_action_GetIids(
        IAsyncAction *iface, ULONG *iid_count, IID **iids)
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_action_GetRuntimeClassName(
        IAsyncAction *iface, HSTRING *class_name)
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_action_GetTrustLevel(
        IAsyncAction *iface, TrustLevel *trust_level)
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_action_put_Completed(IAsyncAction *iface, IAsyncActionCompletedHandler *handler)
{
    FIXME("iface %p, handler %p stub!\n", iface, handler);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_action_get_Completed(IAsyncAction *iface, IAsyncActionCompletedHandler **handler)
{
    FIXME("iface %p, handler %p stub!\n", iface, handler);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_action_GetResults(IAsyncAction *iface)
{
    FIXME("iface %p stub!\n", iface);

    return E_NOTIMPL;
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

static HRESULT STDMETHODCALLTYPE async_info_QueryInterface(IAsyncInfo *iface, REFIID iid, void **out)
{
    struct async_action *action = impl_from_IAsyncInfo(iface);
    return IAsyncAction_QueryInterface(&action->IAsyncAction_iface, iid, out);
}

static ULONG STDMETHODCALLTYPE async_info_AddRef(IAsyncInfo *iface)
{
    struct async_action *action = impl_from_IAsyncInfo(iface);
    return IAsyncAction_AddRef(&action->IAsyncAction_iface);
}

static ULONG STDMETHODCALLTYPE async_info_Release(IAsyncInfo *iface)
{
    struct async_action *action = impl_from_IAsyncInfo(iface);
    return IAsyncAction_Release(&action->IAsyncAction_iface);
}

static HRESULT STDMETHODCALLTYPE async_info_GetIids(IAsyncInfo *iface, ULONG *iid_count, IID **iids)
{
    struct async_action *action = impl_from_IAsyncInfo(iface);
    return IAsyncAction_GetIids(&action->IAsyncAction_iface, iid_count, iids);
}

static HRESULT STDMETHODCALLTYPE async_info_GetRuntimeClassName(IAsyncInfo *iface, HSTRING *class_name)
{
    struct async_action *action = impl_from_IAsyncInfo(iface);
    return IAsyncAction_GetRuntimeClassName(&action->IAsyncAction_iface, class_name);
}

static HRESULT STDMETHODCALLTYPE async_info_GetTrustLevel(IAsyncInfo *iface, TrustLevel *trust_level)
{
    struct async_action *action = impl_from_IAsyncInfo(iface);
    return IAsyncAction_GetTrustLevel(&action->IAsyncAction_iface, trust_level);
}

static HRESULT STDMETHODCALLTYPE async_info_get_Id(IAsyncInfo *iface, UINT32 *id)
{
    FIXME("iface %p, id %p stub!\n", iface, id);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_info_get_Status(IAsyncInfo *iface, AsyncStatus *status)
{
    FIXME("iface %p, status %p stub!\n", iface, status);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_info_get_ErrorCode(IAsyncInfo *iface, HRESULT *error_code)
{
    FIXME("iface %p, error_code %p stub!\n", iface, error_code);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_info_Cancel(IAsyncInfo *iface)
{
    FIXME("iface %p stub!\n", iface);

    return E_NOTIMPL;
}

static HRESULT STDMETHODCALLTYPE async_info_Close(IAsyncInfo *iface)
{
    FIXME("iface %p stub!\n", iface);

    return E_NOTIMPL;
}

static const IAsyncInfoVtbl async_info_vtbl =
{
    async_info_QueryInterface,
    async_info_AddRef,
    async_info_Release,
    async_info_GetIids,
    async_info_GetRuntimeClassName,
    async_info_GetTrustLevel,
    async_info_get_Id,
    async_info_get_Status,
    async_info_get_ErrorCode,
    async_info_Cancel,
    async_info_Close,
};

HRESULT async_action_create(IAsyncAction **ret)
{
    struct async_action *object;

    *ret = NULL;

    if (!(object = calloc(1, sizeof(*object))))
        return E_OUTOFMEMORY;

    object->IAsyncAction_iface.lpVtbl = &async_action_vtbl;
    object->IAsyncInfo_iface.lpVtbl = &async_info_vtbl;
    object->refcount = 1;

    *ret = &object->IAsyncAction_iface;

    return S_OK;
}