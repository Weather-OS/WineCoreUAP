/* WinRT Windows.Storage.SetVersionRequest Implementation
 *
 * Written by Weather
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

#include "SetVersionInternal.h"

_ENABLE_DEBUGGING_

struct set_version *impl_from_ISetVersionRequest( ISetVersionRequest *iface )
{
    return CONTAINING_RECORD( iface, struct set_version, ISetVersionRequest_iface );
}

static HRESULT WINAPI set_version_QueryInterface( ISetVersionRequest *iface, REFIID iid, void **out )
{
    struct set_version *impl = impl_from_ISetVersionRequest( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ISetVersionRequest ))
    {
        *out = &impl->ISetVersionRequest_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI set_version_AddRef( ISetVersionRequest *iface )
{
    struct set_version *impl = impl_from_ISetVersionRequest( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI set_version_Release( ISetVersionRequest *iface )
{
    struct set_version *impl = impl_from_ISetVersionRequest( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI set_version_GetIids( ISetVersionRequest *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI set_version_GetRuntimeClassName( ISetVersionRequest *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI set_version_GetTrustLevel( ISetVersionRequest *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::SetVersionRequest
*/

static HRESULT WINAPI set_version_get_CurrentVersion( ISetVersionRequest *iface, UINT32 *value )
{
    struct set_version *impl = impl_from_ISetVersionRequest( iface );
    *value = impl->CurrentVersion;
    return S_OK;
}

static HRESULT WINAPI set_version_get_DesiredVersion( ISetVersionRequest *iface, UINT32 *value )
{
    struct set_version *impl = impl_from_ISetVersionRequest( iface );
    *value = impl->DesiredVersion;
    return S_OK;
}

static HRESULT WINAPI set_version_GetDeferral( ISetVersionRequest *iface, ISetVersionDeferral **value )
{
    struct set_version *impl = impl_from_ISetVersionRequest( iface );
    *value = &impl->ISetVersionDeferral_iface;
    return S_OK;
}

const struct ISetVersionRequestVtbl set_version_vtbl =
{
    set_version_QueryInterface,
    set_version_AddRef,
    set_version_Release,
    /* IInspectable methods */
    set_version_GetIids,
    set_version_GetRuntimeClassName,
    set_version_GetTrustLevel,
    /* ISetVersionRequest methods */
    set_version_get_CurrentVersion,
    set_version_get_DesiredVersion,
    set_version_GetDeferral
};


static inline struct set_version *impl_from_ISetVersionDeferral( ISetVersionDeferral *iface )
{
    return CONTAINING_RECORD( iface, struct set_version, ISetVersionDeferral_iface );
}

static HRESULT WINAPI set_version_deferral_QueryInterface( ISetVersionDeferral *iface, REFIID iid, void **out )
{
    struct set_version *impl = impl_from_ISetVersionDeferral( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_ISetVersionDeferral ))
    {
        *out = &impl->ISetVersionDeferral_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI set_version_deferral_AddRef( ISetVersionDeferral *iface )
{
    struct set_version *impl = impl_from_ISetVersionDeferral( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI set_version_deferral_Release( ISetVersionDeferral *iface )
{
    struct set_version *impl = impl_from_ISetVersionDeferral( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI set_version_deferral_GetIids( ISetVersionDeferral *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI set_version_deferral_GetRuntimeClassName( ISetVersionDeferral *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI set_version_deferral_GetTrustLevel( ISetVersionDeferral *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI set_version_deferral_Completed( ISetVersionDeferral *iface )
{
    struct set_version *impl = impl_from_ISetVersionDeferral( iface );
    free(impl);
    return S_OK;
}

const struct ISetVersionDeferralVtbl set_version_deferral_vtbl =
{
    set_version_deferral_QueryInterface,
    set_version_deferral_AddRef,
    set_version_deferral_Release,
    /* IInspectable methods */
    set_version_deferral_GetIids,
    set_version_deferral_GetRuntimeClassName,
    set_version_deferral_GetTrustLevel,
    /* ISetVersionDeferral methods */
    set_version_deferral_Completed
};
