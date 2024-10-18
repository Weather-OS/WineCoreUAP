/* WinRT Windows.Storage.SystemProperties Implementation
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

#include "SystemPropertiesInternal.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

// System Properties

struct system_properties
{
    IActivationFactory IActivationFactory_iface;
    ISystemProperties ISystemProperties_iface;
    LONG ref;
};

static inline struct system_properties *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct system_properties, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{

    struct system_properties *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_ISystemProperties ))
    {
        *out = &impl->ISystemProperties_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct system_properties *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct system_properties *impl = impl_from_IActivationFactory( iface );
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

DEFINE_IINSPECTABLE( system_properties, ISystemProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_properties_get_Author( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Author";
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Comment( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Comment";
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_ItemNameDisplay( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.ItemNameDisplay";
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Keywords( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Keywords";
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Rating( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Rating";
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Title( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Title";
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Audio( ISystemProperties *iface, ISystemAudioProperties **value )
{
    FIXME( "iface %p, instance %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI system_properties_get_GPS( ISystemProperties *iface, ISystemGPSProperties **value )
{
    FIXME( "iface %p, instance %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI system_properties_get_Media( ISystemProperties *iface, ISystemMediaProperties **value )
{
    FIXME( "iface %p, instance %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI system_properties_get_Music( ISystemProperties *iface, ISystemMusicProperties **value )
{
    FIXME( "iface %p, instance %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI system_properties_get_Photo( ISystemProperties *iface, ISystemPhotoProperties **value )
{
    FIXME( "iface %p, instance %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI system_properties_get_Video( ISystemProperties *iface, ISystemVideoProperties **value )
{
    FIXME( "iface %p, instance %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI system_properties_get_Image( ISystemProperties *iface, ISystemImageProperties **value )
{
    FIXME( "iface %p, instance %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static const struct ISystemPropertiesVtbl system_properties_vtbl =
{
    system_properties_QueryInterface,
    system_properties_AddRef,
    system_properties_Release,
    /* IInspectable methods */
    system_properties_GetIids,
    system_properties_GetRuntimeClassName,
    system_properties_GetTrustLevel,
    /* ISystemProperties methods */
    system_properties_get_Author,
    system_properties_get_Comment,
    system_properties_get_ItemNameDisplay,
    system_properties_get_Keywords,
    system_properties_get_Rating,
    system_properties_get_Title,
    system_properties_get_Audio,
    system_properties_get_GPS,
    system_properties_get_Media,
    system_properties_get_Music,
    system_properties_get_Photo,
    system_properties_get_Video,
    system_properties_get_Image
};

static struct system_properties system_properties =
{
    {&factory_vtbl},
    {&system_properties_vtbl},
    1,
};

IActivationFactory *system_properties_factory = &system_properties.IActivationFactory_iface;
