/* WinRT Windows.Storage.FileProperties.IStorageItemExtraProperties Implementation
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

#include "StorageItemExtraPropertiesInternal.h"

_ENABLE_DEBUGGING_

// Storage Item Extra Properties

struct storage_item_extra_properties *impl_from_IStorageItemExtraProperties( IStorageItemExtraProperties *iface )
{
    return CONTAINING_RECORD( iface, struct storage_item_extra_properties, IStorageItemExtraProperties_iface );
}

static HRESULT WINAPI storage_item_extra_properties_QueryInterface( IStorageItemExtraProperties *iface, REFIID iid, void **out )
{
    struct storage_item_extra_properties *impl = impl_from_IStorageItemExtraProperties( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IStorageItemExtraProperties ))
    {
        *out = &impl->IStorageItemExtraProperties_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_item_extra_properties_AddRef( IStorageItemExtraProperties *iface )
{
    struct storage_item_extra_properties *impl = impl_from_IStorageItemExtraProperties( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_item_extra_properties_Release( IStorageItemExtraProperties *iface )
{
    struct storage_item_extra_properties *impl = impl_from_IStorageItemExtraProperties( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI storage_item_extra_properties_GetIids( IStorageItemExtraProperties *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_extra_properties_GetRuntimeClassName( IStorageItemExtraProperties *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_extra_properties_GetTrustLevel( IStorageItemExtraProperties *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_extra_properties_RetrievePropertiesAsync( IStorageItemExtraProperties *iface, IIterable_HSTRING *propertiesToRetrieve, IAsyncOperation_IMap_HSTRING_IInspectable **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IMap_HSTRING_IInspectable };

    TRACE( "iface %p, propertiesToRetrieve %p, operation %p\n", iface, propertiesToRetrieve, operation );

    // Arguments 
    if ( !propertiesToRetrieve ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;
    
    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)propertiesToRetrieve, storage_item_extra_properties_FetchPropertiesAsync, iids, (IAsyncOperation_IInspectable **)operation );
    return hr;
}

static HRESULT WINAPI storage_item_extra_properties_SavePropertiesAsync( IStorageItemExtraProperties *iface, IIterable_IKeyValuePair_HSTRING_IInspectable *propertiesToSave, IAsyncAction **operation )
{
    HRESULT hr;

    TRACE( "iface %p, propertiesToSave %p, operation %p\n", iface, propertiesToSave, operation );

    // Arguments
    if ( !propertiesToSave ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)propertiesToSave, storage_item_extra_properties_SubmitPropertiesAsync, operation );
    return hr;
}

static HRESULT WINAPI storage_item_extra_properties_SavePropertiesAsyncOverloadDefault( IStorageItemExtraProperties *iface, IAsyncAction **operation )
{
    HRESULT hr;

    TRACE( "iface %p, operation %p\n", iface, operation );

    // Arguments
    if ( !operation ) return E_POINTER;

    hr = async_action_create( (IUnknown *)iface, NULL, storage_item_extra_properties_SubmitPropertiesAsync, operation );
    return hr;
}

const struct IStorageItemExtraPropertiesVtbl storage_item_extra_properties_vtbl =
{
    storage_item_extra_properties_QueryInterface,
    storage_item_extra_properties_AddRef,
    storage_item_extra_properties_Release,
    /* IInspectable methods */
    storage_item_extra_properties_GetIids,
    storage_item_extra_properties_GetRuntimeClassName,
    storage_item_extra_properties_GetTrustLevel,
    /* IStorageItemExtraProperties methods */
    storage_item_extra_properties_RetrievePropertiesAsync,
    storage_item_extra_properties_SavePropertiesAsync,
    storage_item_extra_properties_SavePropertiesAsyncOverloadDefault
};