/* WinRT Windows.Storage.StorageProvider Implementation
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

#include "StorageProviderInternal.h"

_ENABLE_DEBUGGING_

// Storage Provider

static struct storage_provider *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct storage_provider, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct storage_provider *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IStorageProvider ))
    {
        *out = &impl->IStorageProvider_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct storage_provider *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct storage_provider *impl = impl_from_IActivationFactory( iface );
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

DEFINE_IINSPECTABLE( storage_provider, IStorageProvider, struct storage_provider, IStorageProvider_iface )

static HRESULT WINAPI storage_provider_get_Id( IStorageProvider *iface, HSTRING *value )
{
    struct storage_provider *impl = impl_from_IStorageProvider( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;

    WindowsDuplicateString( impl->Id, value );
    return S_OK;
}

static HRESULT WINAPI storage_provider_get_DisplayName( IStorageProvider *iface, HSTRING *value )
{    
    struct storage_provider *impl = impl_from_IStorageProvider( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    //Arguments
    if ( !value ) return E_POINTER;

    WindowsDuplicateString( impl->DisplayName, value );
    return S_OK;
}

const struct IStorageProviderVtbl storage_provider_vtbl =
{
    storage_provider_QueryInterface,
    storage_provider_AddRef,
    storage_provider_Release,
    /* IInspectable methods */
    storage_provider_GetIids,
    storage_provider_GetRuntimeClassName,
    storage_provider_GetTrustLevel,
    /* IStorageProvider methods */    
    storage_provider_get_Id,
    storage_provider_get_DisplayName
};

static struct storage_provider storage_provider =
{    
    {&factory_vtbl},    
    {&storage_provider_vtbl},    
    NULL,
    NULL,     
    1,    
};

IActivationFactory *storage_provider_factory = &storage_provider.IActivationFactory_iface;