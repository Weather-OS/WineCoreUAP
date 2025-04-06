/* WinRT Windows.Storage.FileProperties.BasicProperties Implementation
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

#include "BasicPropertiesInternal.h"

_ENABLE_DEBUGGING_

// Basic Properties

static struct basic_properties *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct basic_properties, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct basic_properties *impl = impl_from_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IBasicProperties ))
    {
        *out = &impl->IBasicProperties_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct basic_properties *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct basic_properties *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    /* IInspectable methods */
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    /* IActivationFactory methods */
    factory_ActivateInstance,
};

DEFINE_IINSPECTABLE( basic_properties, IBasicProperties, struct basic_properties, IActivationFactory_iface )

static HRESULT WINAPI basic_properties_get_Size( IBasicProperties *iface, UINT64 *value )
{
    struct basic_properties *impl = impl_from_IBasicProperties( iface );
    TRACE( "iface %p value %p.\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->size;
    return S_OK;
}

static HRESULT WINAPI basic_properties_get_DateModified( IBasicProperties *iface, DateTime *value )
{
    struct basic_properties *impl = impl_from_IBasicProperties( iface );
    TRACE( "iface %p value %p.\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->DateModified;
    return S_OK;
}

static HRESULT WINAPI basic_properties_get_TimeDate( IBasicProperties *iface, DateTime *value )
{
    struct basic_properties *impl = impl_from_IBasicProperties( iface );
    TRACE( "iface %p value %p.\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->ItemDate;
    return S_OK;
}

const struct IBasicPropertiesVtbl basic_properties_vtbl =
{
    basic_properties_QueryInterface,
    basic_properties_AddRef,
    basic_properties_Release,
    /* IInspectable methods */
    basic_properties_GetIids,
    basic_properties_GetRuntimeClassName,
    basic_properties_GetTrustLevel,
    /* IBasicProperties methods */
    basic_properties_get_Size,
    basic_properties_get_DateModified,
    basic_properties_get_TimeDate
};

static struct basic_properties basic_properties =
{
    {&factory_vtbl},
    {&basic_properties_vtbl},
    {0},
    {0},
    0,
    {&storage_item_extra_properties_vtbl},
    NULL,
    NULL,
    1,
    1,
};

IActivationFactory *basic_properties_factory = &basic_properties.IActivationFactory_iface;