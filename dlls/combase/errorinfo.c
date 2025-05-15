/*
 * ErrorInfo API
 *
 * Copyright 2000 Patrik Stridvall, Juergen Schmied
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

#define COBJMACROS
#define WINOLEAUTAPI

#include "oleauto.h"
#include "restrictederrorinfo.h"

#include "combase_private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ole);

struct error_info
{
    IErrorInfo IErrorInfo_iface;
    ICreateErrorInfo ICreateErrorInfo_iface;
    ISupportErrorInfo ISupportErrorInfo_iface;    
    IRestrictedErrorInfo IRestrictedErrorInfo_iface;
    LONG refcount;

    GUID guid;
    // IErrorInfo Section
    WCHAR *source;
    WCHAR *description;
    WCHAR *help_file;
    // IRestrictedErrorInfo Section
    HRESULT hresult_error;
    BSTR restricted_description;
    BSTR capability_sid;
    BSTR reference;
    DWORD help_context;
};

static struct error_info *impl_from_IErrorInfo(IErrorInfo *iface)
{
    return CONTAINING_RECORD(iface, struct error_info, IErrorInfo_iface);
}

static struct error_info *impl_from_ICreateErrorInfo(ICreateErrorInfo *iface)
{
    return CONTAINING_RECORD(iface, struct error_info, ICreateErrorInfo_iface);
}

static struct error_info *impl_from_ISupportErrorInfo(ISupportErrorInfo *iface)
{
    return CONTAINING_RECORD(iface, struct error_info, ISupportErrorInfo_iface);
}

static struct error_info *impl_from_IRestrictedErrorInfo_iface(IRestrictedErrorInfo *iface)
{
    return CONTAINING_RECORD(iface, struct error_info, IRestrictedErrorInfo_iface); 
}

static HRESULT WINAPI errorinfo_QueryInterface(IErrorInfo *iface, REFIID riid, void **obj)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);

    TRACE("%p, %s, %p.\n", iface, debugstr_guid(riid), obj);

    *obj = NULL;

    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IErrorInfo))
    {
        *obj = &error_info->IErrorInfo_iface;
    }
    else if (IsEqualIID(riid, &IID_ICreateErrorInfo))
    {
        *obj = &error_info->ICreateErrorInfo_iface;
    }
    else if (IsEqualIID(riid, &IID_ISupportErrorInfo))
    {
        *obj = &error_info->ISupportErrorInfo_iface;
    }
    else if (IsEqualIID(riid, &IID_IRestrictedErrorInfo))
    {
        *obj = &error_info->IRestrictedErrorInfo_iface;
    }

    if (*obj)
    {
        IUnknown_AddRef((IUnknown *)*obj);
        return S_OK;
    }

    WARN("Unsupported interface %s.\n", debugstr_guid(riid));
    return E_NOINTERFACE;
}

static ULONG WINAPI errorinfo_AddRef(IErrorInfo *iface)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);
    ULONG refcount = InterlockedIncrement(&error_info->refcount);

    TRACE("%p, refcount %lu.\n", iface, refcount);

    return refcount;
}

static ULONG WINAPI errorinfo_Release(IErrorInfo *iface)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);
    ULONG refcount = InterlockedDecrement(&error_info->refcount);

    TRACE("%p, refcount %lu.\n", iface, refcount);

    if (!refcount)
    {
        free(error_info->source);
        free(error_info->description);
        free(error_info->help_file);
        free(error_info);
    }

    return refcount;
}

static HRESULT WINAPI errorinfo_GetGUID(IErrorInfo *iface, GUID *guid)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);

    TRACE("%p, %p.\n", iface, guid);

    if (!guid) return E_INVALIDARG;
    *guid = error_info->guid;
    return S_OK;
}

static HRESULT WINAPI errorinfo_GetSource(IErrorInfo* iface, BSTR *source)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);

    TRACE("%p, %p.\n", iface, source);

    if (!source)
        return E_INVALIDARG;
    *source = SysAllocString(error_info->source);
    return S_OK;
}

static HRESULT WINAPI errorinfo_GetDescription(IErrorInfo *iface, BSTR *description)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);

    TRACE("%p, %p.\n", iface, description);

    if (!description)
        return E_INVALIDARG;
    *description = SysAllocString(error_info->description);
    return S_OK;
}

static HRESULT WINAPI errorinfo_GetHelpFile(IErrorInfo *iface, BSTR *helpfile)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);

    TRACE("%p, %p.\n", iface, helpfile);

    if (!helpfile)
        return E_INVALIDARG;
    *helpfile = SysAllocString(error_info->help_file);
    return S_OK;
}

static HRESULT WINAPI errorinfo_GetHelpContext(IErrorInfo *iface, DWORD *help_context)
{
    struct error_info *error_info = impl_from_IErrorInfo(iface);

    TRACE("%p, %p.\n", iface, help_context);

    if (!help_context)
        return E_INVALIDARG;
    *help_context = error_info->help_context;

    return S_OK;
}

static const IErrorInfoVtbl errorinfo_vtbl =
{
    errorinfo_QueryInterface,
    errorinfo_AddRef,
    errorinfo_Release,
    errorinfo_GetGUID,
    errorinfo_GetSource,
    errorinfo_GetDescription,
    errorinfo_GetHelpFile,
    errorinfo_GetHelpContext
};

static HRESULT WINAPI create_errorinfo_QueryInterface(ICreateErrorInfo *iface, REFIID riid, void **obj)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);
    return IErrorInfo_QueryInterface(&error_info->IErrorInfo_iface, riid, obj);
}

static ULONG WINAPI create_errorinfo_AddRef(ICreateErrorInfo *iface)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);
    return IErrorInfo_AddRef(&error_info->IErrorInfo_iface);
}

static ULONG WINAPI create_errorinfo_Release(ICreateErrorInfo *iface)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);
    return IErrorInfo_Release(&error_info->IErrorInfo_iface);
}

static HRESULT WINAPI create_errorinfo_SetGUID(ICreateErrorInfo *iface, REFGUID guid)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);

    TRACE("%p, %s.\n", iface, debugstr_guid(guid));

    error_info->guid = *guid;

    return S_OK;
}

static HRESULT WINAPI create_errorinfo_SetSource(ICreateErrorInfo *iface, LPOLESTR source)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);

    TRACE("%p, %s.\n", iface, debugstr_w(source));

    free(error_info->source);
    error_info->source = wcsdup(source);

    return S_OK;
}

static HRESULT WINAPI create_errorinfo_SetDescription(ICreateErrorInfo *iface, LPOLESTR description)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);

    TRACE("%p, %s.\n", iface, debugstr_w(description));

    free(error_info->description);
    error_info->description = wcsdup(description);

    return S_OK;
}

static HRESULT WINAPI create_errorinfo_SetHelpFile(ICreateErrorInfo *iface, LPOLESTR helpfile)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);

    TRACE("%p, %s.\n", iface, debugstr_w(helpfile));

    free(error_info->help_file);
    error_info->help_file = wcsdup(helpfile);

    return S_OK;
}

static HRESULT WINAPI create_errorinfo_SetHelpContext(ICreateErrorInfo *iface, DWORD help_context)
{
    struct error_info *error_info = impl_from_ICreateErrorInfo(iface);

    TRACE("%p, %#lx.\n", iface, help_context);

    error_info->help_context = help_context;

    return S_OK;
}

static const ICreateErrorInfoVtbl create_errorinfo_vtbl =
{
    create_errorinfo_QueryInterface,
    create_errorinfo_AddRef,
    create_errorinfo_Release,
    create_errorinfo_SetGUID,
    create_errorinfo_SetSource,
    create_errorinfo_SetDescription,
    create_errorinfo_SetHelpFile,
    create_errorinfo_SetHelpContext
};

static HRESULT WINAPI support_errorinfo_QueryInterface(ISupportErrorInfo *iface, REFIID riid, void **obj)
{
    struct error_info *error_info = impl_from_ISupportErrorInfo(iface);
    return IErrorInfo_QueryInterface(&error_info->IErrorInfo_iface, riid, obj);
}

static ULONG WINAPI support_errorinfo_AddRef(ISupportErrorInfo *iface)
{
    struct error_info *error_info = impl_from_ISupportErrorInfo(iface);
    return IErrorInfo_AddRef(&error_info->IErrorInfo_iface);
}

static ULONG WINAPI support_errorinfo_Release(ISupportErrorInfo *iface)
{
    struct error_info *error_info = impl_from_ISupportErrorInfo(iface);
    return IErrorInfo_Release(&error_info->IErrorInfo_iface);
}

static HRESULT WINAPI support_errorinfo_InterfaceSupportsErrorInfo(ISupportErrorInfo *iface, REFIID riid)
{
    struct error_info *error_info = impl_from_ISupportErrorInfo(iface);

    TRACE("%p, %s.\n", iface, debugstr_guid(riid));

    return IsEqualIID(riid, &error_info->guid) ? S_OK : S_FALSE;
}

static const ISupportErrorInfoVtbl support_errorinfo_vtbl =
{
    support_errorinfo_QueryInterface,
    support_errorinfo_AddRef,
    support_errorinfo_Release,
    support_errorinfo_InterfaceSupportsErrorInfo
};

static HRESULT WINAPI restricted_errorinfo_QueryInterface(IRestrictedErrorInfo *iface, REFIID riid, void **obj)
{
    struct error_info *error_info = impl_from_IRestrictedErrorInfo_iface(iface);
    return IErrorInfo_QueryInterface(&error_info->IErrorInfo_iface, riid, obj);
}

static ULONG WINAPI restricted_errorinfo_AddRef(IRestrictedErrorInfo *iface)
{
    struct error_info *error_info = impl_from_IRestrictedErrorInfo_iface(iface);
    return IErrorInfo_AddRef(&error_info->IErrorInfo_iface);
}

static ULONG WINAPI restricted_errorinfo_Release(IRestrictedErrorInfo *iface)
{
    struct error_info *error_info = impl_from_IRestrictedErrorInfo_iface(iface);
    return IErrorInfo_Release(&error_info->IErrorInfo_iface);
}

static HRESULT WINAPI restricted_errorinfo_GetErrorDetails(IRestrictedErrorInfo *iface, BSTR *description, HRESULT *error, BSTR *restrictedDescription, BSTR *capabilitySid)
{
    struct error_info *error_info = impl_from_IRestrictedErrorInfo_iface(iface);
    if( description ) *description = SysAllocString(error_info->description);
    if( restrictedDescription ) *restrictedDescription = SysAllocString(error_info->restricted_description);
    if( capabilitySid ) *capabilitySid = SysAllocString(error_info->capability_sid);
    if( error ) *error = error_info->hresult_error;
    return S_OK;
}

static HRESULT WINAPI restricted_errorinfo_GetReference(IRestrictedErrorInfo *iface, BSTR *reference)
{
    struct error_info *error_info = impl_from_IRestrictedErrorInfo_iface(iface);
    *reference = SysAllocString(error_info->reference);
    return S_OK;
}

static const IRestrictedErrorInfoVtbl restricted_errorinfo_vtbl =
{
    restricted_errorinfo_QueryInterface,
    restricted_errorinfo_AddRef,
    restricted_errorinfo_Release,
    restricted_errorinfo_GetErrorDetails,
    restricted_errorinfo_GetReference
};

/***********************************************************************
 *                CreateErrorInfo (combase.@)
 */
HRESULT WINAPI CreateErrorInfo(ICreateErrorInfo **ret)
{
    struct error_info *error_info;

    TRACE("%p.\n", ret);

    if (!ret) return E_INVALIDARG;

    if (!(error_info = malloc(sizeof(*error_info))))
        return E_OUTOFMEMORY;

    error_info->IErrorInfo_iface.lpVtbl = &errorinfo_vtbl;
    error_info->ICreateErrorInfo_iface.lpVtbl = &create_errorinfo_vtbl;
    error_info->ISupportErrorInfo_iface.lpVtbl = &support_errorinfo_vtbl;
    error_info->IRestrictedErrorInfo_iface.lpVtbl = &restricted_errorinfo_vtbl;
    error_info->refcount = 1;
    error_info->source = NULL;
    error_info->description = NULL;
    error_info->help_file = NULL;
    error_info->hresult_error = S_OK;
    error_info->restricted_description = NULL;
    error_info->capability_sid = NULL;
    error_info->reference = NULL;
    error_info->help_context = 0;

    *ret = &error_info->ICreateErrorInfo_iface;

    return S_OK;
}

/***********************************************************************
 *                GetErrorInfo    (combase.@)
 */
HRESULT WINAPI GetErrorInfo(ULONG reserved, IErrorInfo **error_info)
{
    struct tlsdata *tlsdata;
    HRESULT hr;

    TRACE("%lu, %p\n", reserved, error_info);

    if (reserved || !error_info)
        return E_INVALIDARG;

    if (FAILED(hr = com_get_tlsdata(&tlsdata)))
        return hr;

    if (!tlsdata->errorinfo)
    {
        *error_info = NULL;
        return S_FALSE;
    }

    *error_info = tlsdata->errorinfo;
    tlsdata->errorinfo = NULL;

    return S_OK;
}

/***********************************************************************
 *               SetErrorInfo    (combase.@)
 */
HRESULT WINAPI SetErrorInfo(ULONG reserved, IErrorInfo *error_info)
{
    struct tlsdata *tlsdata;
    HRESULT hr;

    TRACE("%lu, %p\n", reserved, error_info);

    if (reserved)
        return E_INVALIDARG;

    if (FAILED(hr = com_get_tlsdata(&tlsdata)))
        return hr;

    if (tlsdata->errorinfo)
        IErrorInfo_Release(tlsdata->errorinfo);

    tlsdata->errorinfo = error_info;
    if (error_info)
        IErrorInfo_AddRef(error_info);

    return S_OK;
}


/***********************************************************************
 *       GetRestrictedErrorInfo    (combase.@)
 */
HRESULT WINAPI GetRestrictedErrorInfo(IRestrictedErrorInfo **info)
{
    struct tlsdata *tlsdata;
    HRESULT hr;

    TRACE("%p\n", info);

    if (!info)
        return E_INVALIDARG;

    if (FAILED(hr = com_get_tlsdata(&tlsdata)))
        return hr;

    if (!tlsdata->errorinfo)
    {
        *info = NULL;
        return S_FALSE;
    }

    hr = IErrorInfo_QueryInterface( tlsdata->errorinfo, &IID_IRestrictedErrorInfo, (void **)info );
    if (FAILED(hr))
        return hr;
    tlsdata->errorinfo = NULL;

    return S_OK;
}

/***********************************************************************
 *       SetRestrictedErrorInfo    (combase.@)
 */
HRESULT WINAPI SetRestrictedErrorInfo(IRestrictedErrorInfo *restricted_error_info)
{
    struct error_info *error_info;

    HRESULT hr;
    IErrorInfo *errorInfo = NULL;
    ICreateErrorInfo *createErrorInfo = NULL;

    TRACE("%p\n", restricted_error_info);

    if (!restricted_error_info) 
        return E_INVALIDARG;

    CreateErrorInfo(&createErrorInfo);
    error_info = impl_from_ICreateErrorInfo(createErrorInfo);

    hr = IRestrictedErrorInfo_GetErrorDetails(restricted_error_info, &error_info->description, 
                                              &error_info->hresult_error, &error_info->restricted_description,
                                              &error_info->capability_sid);
    if (FAILED(hr)) 
        return hr;
    hr = IRestrictedErrorInfo_GetReference(restricted_error_info, &error_info->reference);
    if (FAILED(hr)) 
        return hr;

    FIXME("application reported an error! error description was %s, error hresult was 0x%#lx\n", debugstr_w(error_info->description), error_info->hresult_error);

    ICreateErrorInfo_QueryInterface( createErrorInfo, &IID_IErrorInfo, (void **)&errorInfo );

    return SetErrorInfo( 0, errorInfo );;
}