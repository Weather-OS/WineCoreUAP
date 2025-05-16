/* WinRT Windows.Storage.ApplicationDataContainer Implementation
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

#include "ApplicationDataContainerInternal.h"

_ENABLE_DEBUGGING_

struct application_data_container *impl_from_IApplicationDataContainer( IApplicationDataContainer *iface )
{
    return CONTAINING_RECORD( iface, struct application_data_container, IApplicationDataContainer_iface );
}

static HRESULT WINAPI application_data_container_QueryInterface( IApplicationDataContainer *iface, REFIID iid, void **out )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IApplicationDataContainer ))
    {
        *out = &impl->IApplicationDataContainer_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI application_data_container_AddRef( IApplicationDataContainer *iface )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI application_data_container_Release( IApplicationDataContainer *iface )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI application_data_container_GetIids( IApplicationDataContainer *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_container_GetRuntimeClassName( IApplicationDataContainer *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI application_data_container_GetTrustLevel( IApplicationDataContainer *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::ApplicationDataContainer
*/

static HRESULT WINAPI application_data_container_get_Name( IApplicationDataContainer *iface, HSTRING *value )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Name;
    return S_OK;
}

static HRESULT WINAPI application_data_container_get_Locality( IApplicationDataContainer *iface, ApplicationDataLocality *value )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Locality;
    return S_OK;
}

static HRESULT WINAPI application_data_container_get_Values( IApplicationDataContainer *iface, IPropertySet **value )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Values;
    return S_OK;
}

static HRESULT WINAPI application_data_container_get_Containers( IApplicationDataContainer *iface, IMapView_HSTRING_ApplicationDataContainer **value )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    return IMap_HSTRING_ApplicationDataContainer_GetView( impl->Containers, value );
}

static HRESULT WINAPI application_data_container_CreateContainer( IApplicationDataContainer *iface, HSTRING name, ApplicationDataCreateDisposition disposition, IApplicationDataContainer **container )
{
    TRACE( "iface %p, name %s\n", iface, debugstr_hstring(name) );
    return application_data_container_CreateAndTrackContainer( iface, name, disposition, container );
}

static HRESULT WINAPI application_data_container_DeleteContainer( IApplicationDataContainer *iface, HSTRING name )
{
    struct application_data_container *impl = impl_from_IApplicationDataContainer( iface );
    TRACE( "iface %p, name %s\n", iface, debugstr_hstring(name) );
    return IMap_HSTRING_ApplicationDataContainer_Remove( impl->Containers, name );
}

const struct IApplicationDataContainerVtbl application_data_container_vtbl =
{
    application_data_container_QueryInterface,
    application_data_container_AddRef,
    application_data_container_Release,
    /* IInspectable methods */
    application_data_container_GetIids,
    application_data_container_GetRuntimeClassName,
    application_data_container_GetTrustLevel,
    /* IApplicationDataContainer methods */
    application_data_container_get_Name,
    application_data_container_get_Locality,
    application_data_container_get_Values,
    application_data_container_get_Containers,
    application_data_container_CreateContainer,
    application_data_container_DeleteContainer
};
