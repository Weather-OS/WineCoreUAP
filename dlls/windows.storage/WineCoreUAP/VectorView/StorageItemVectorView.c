/* WinRT IVectorView<Windows.Storage.IStorageItem *> Implementation.
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

#include "StorageItemVectorView.h"

#include "../../private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(combase);

struct storage_item_vector_view *impl_from_IVectorView_IStorageItem( IVectorView_IStorageItem *iface )
{
    return CONTAINING_RECORD( iface, struct storage_item_vector_view, IVectorView_IStorageItem_iface );
}

static HRESULT WINAPI storage_item_vector_view_QueryInterface( IVectorView_IStorageItem *iface, REFIID iid, void **out )
{
    struct storage_item_vector_view *impl = impl_from_IVectorView_IStorageItem( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iids.view ))
    {
        IInspectable_AddRef( (*out = &impl->IVectorView_IStorageItem_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI storage_item_vector_view_AddRef( IVectorView_IStorageItem *iface )
{
    struct storage_item_vector_view *impl = impl_from_IVectorView_IStorageItem( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI storage_item_vector_view_Release( IVectorView_IStorageItem *iface )
{
    struct storage_item_vector_view *impl = impl_from_IVectorView_IStorageItem( iface );
    ULONG i, ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        for (i = 0; i < impl->size; ++i) IStorageItem_Release( impl->elements[i] );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI storage_item_vector_view_GetIids( IVectorView_IStorageItem *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_vector_view_GetRuntimeClassName( IVectorView_IStorageItem *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_vector_view_GetTrustLevel( IVectorView_IStorageItem *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI storage_item_vector_view_GetAt( IVectorView_IStorageItem *iface, UINT32 index, IStorageItem **value )
{
    struct storage_item_vector_view *impl = impl_from_IVectorView_IStorageItem( iface );

    TRACE( "iface %p, index %u, value %p.\n", iface, index, value );

    *value = NULL;
    if (index >= impl->size) return E_BOUNDS;

    IStorageItem_AddRef( (*value = impl->elements[index]) );
    return S_OK;
}

static HRESULT WINAPI storage_item_vector_view_get_Size( IVectorView_IStorageItem *iface, UINT32 *value )
{
    struct storage_item_vector_view *impl = impl_from_IVectorView_IStorageItem( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

    *value = impl->size;
    return S_OK;
}

static HRESULT WINAPI storage_item_vector_view_IndexOf( IVectorView_IStorageItem *iface, IStorageItem *element,
                                           UINT32 *index, BOOLEAN *found )
{
    struct storage_item_vector_view *impl = impl_from_IVectorView_IStorageItem( iface );
    ULONG i;

    TRACE( "iface %p, element %p, index %p, found %p.\n", iface, element, index, found );

    for (i = 0; i < impl->size; ++i) if (impl->elements[i] == element) break;
    if ((*found = (i < impl->size))) *index = i;
    else *index = 0;

    return S_OK;
}

static HRESULT WINAPI storage_item_vector_view_GetMany( IVectorView_IStorageItem *iface, UINT32 start_index,
                                           UINT32 items_size, IStorageItem **items, UINT *count )
{
    struct storage_item_vector_view *impl = impl_from_IVectorView_IStorageItem( iface );
    UINT32 i;

    TRACE( "iface %p, start_index %u, items_size %u, items %p, count %p.\n",
           iface, start_index, items_size, items, count );

    if (start_index >= impl->size) return E_BOUNDS;

    for (i = start_index; i < impl->size; ++i)
    {
        if (i - start_index >= items_size) break;
        IStorageItem_AddRef( (items[i - start_index] = impl->elements[i]) );
    }
    *count = i - start_index;

    return S_OK;
}

struct IVectorView_IStorageItemVtbl storage_item_vector_view_vtbl =
{
    storage_item_vector_view_QueryInterface,
    storage_item_vector_view_AddRef,
    storage_item_vector_view_Release,
    /* IInspectable methods */
    storage_item_vector_view_GetIids,
    storage_item_vector_view_GetRuntimeClassName,
    storage_item_vector_view_GetTrustLevel,
    /* IVectorView<IStorageItem*> methods */
    storage_item_vector_view_GetAt,
    storage_item_vector_view_get_Size,
    storage_item_vector_view_IndexOf,
    storage_item_vector_view_GetMany,
};