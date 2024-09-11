/* WinRT IVector<HSTRING> Implementation.
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

#include "HSTRINGVectorView.h"

WINE_DEFAULT_DEBUG_CHANNEL(storage);

struct hstring_iterator *impl_from_IIterator_HSTRING( IIterator_HSTRING *iface )
{
   return CONTAINING_RECORD(iface, struct hstring_iterator, IIterator_HSTRING_iface);
}

static HRESULT WINAPI hstring_iterator_QueryInterface( IIterator_HSTRING *iface, REFIID iid, void **out )
{
    struct hstring_iterator *impl = impl_from_IIterator_HSTRING(iface);

   TRACE("iface %p, iid %s, out %p.\n", iface, debugstr_guid(iid), out);

   if (IsEqualGUID(iid, &IID_IUnknown) ||
       IsEqualGUID(iid, &IID_IInspectable) ||
       IsEqualGUID(iid, &IID_IAgileObject) ||
       IsEqualGUID(iid, &IID_IIterator_HSTRING))
    {
       IInspectable_AddRef((*out = &impl->IIterator_HSTRING_iface));
       return S_OK;
    }

    WARN("%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid(iid));
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_iterator_AddRef( IIterator_HSTRING *iface )
{
    struct hstring_iterator *impl = impl_from_IIterator_HSTRING(iface);
    ULONG ref = InterlockedIncrement(&impl->ref);
    TRACE("iface %p increasing refcount to %lu.\n", iface, ref);
    return ref;
}

static ULONG WINAPI hstring_iterator_Release( IIterator_HSTRING *iface )
{
    struct hstring_iterator *impl = impl_from_IIterator_HSTRING(iface);
    ULONG ref = InterlockedDecrement(&impl->ref);

    TRACE("iface %p decreasing refcount to %lu.\n", iface, ref);

    if (!ref)
    {
        IVectorView_HSTRING_Release(impl->view);
        free(impl);
    }

    return ref;
}

static HRESULT WINAPI hstring_iterator_GetIids( IIterator_HSTRING *iface, ULONG *iid_count, IID **iids )
{
    FIXME("iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids);
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_iterator_GetRuntimeClassName( IIterator_HSTRING *iface, HSTRING *class_name )
{
    FIXME("iface %p, class_name %p stub!\n", iface, class_name);
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_iterator_GetTrustLevel( IIterator_HSTRING *iface, TrustLevel *trust_level )
{
    FIXME("iface %p, trust_level %p stub!\n", iface, trust_level);
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_iterator_get_Current( IIterator_HSTRING *iface, HSTRING *value )
{
    struct hstring_iterator *impl = impl_from_IIterator_HSTRING(iface);
    TRACE("iface %p, value %p.\n", iface, value);
    return IVectorView_HSTRING_GetAt(impl->view, impl->index, value);
}

static HRESULT WINAPI hstring_iterator_get_HasCurrent( IIterator_HSTRING *iface, boolean *value )
{
    struct hstring_iterator *impl = impl_from_IIterator_HSTRING(iface);

    TRACE("iface %p, value %p.\n", iface, value);

    *value = impl->index < impl->size;
    return S_OK;
}

static HRESULT WINAPI hstring_iterator_MoveNext( IIterator_HSTRING *iface, boolean *value )
{
    struct hstring_iterator *impl = impl_from_IIterator_HSTRING(iface);

    TRACE("iface %p, value %p.\n", iface, value);

    if (impl->index < impl->size) impl->index++;
    return IIterator_HSTRING_get_HasCurrent(iface, value);
}

static HRESULT WINAPI hstring_iterator_GetMany( IIterator_HSTRING *iface, UINT32 items_size,
                                                HSTRING *items, UINT *count )
{
    struct hstring_iterator *impl = impl_from_IIterator_HSTRING(iface);
    TRACE("iface %p, items_size %u, items %p, count %p.\n", iface, items_size, items, count);
    return IVectorView_HSTRING_GetMany(impl->view, impl->index, items_size, items, count);
}

static const IIterator_HSTRINGVtbl hstring_iterator_vtbl =
{
    /* IUnknown methods */
    hstring_iterator_QueryInterface,
    hstring_iterator_AddRef,
    hstring_iterator_Release,
    /* IInspectable methods */
    hstring_iterator_GetIids,
    hstring_iterator_GetRuntimeClassName,
    hstring_iterator_GetTrustLevel,
    /* IIterator<HSTRING> methods */
    hstring_iterator_get_Current,
    hstring_iterator_get_HasCurrent,
    hstring_iterator_MoveNext,
    hstring_iterator_GetMany,
};

struct hstring_vector_view *impl_from_IVectorView_HSTRING( IVectorView_HSTRING *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_vector_view, IVectorView_HSTRING_iface );
}

static HRESULT WINAPI hstring_vector_view_QueryInterface( IVectorView_HSTRING *iface, REFIID iid, void **out )
{
    struct hstring_vector_view *impl = impl_from_IVectorView_HSTRING( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iids.view ))
    {
        IInspectable_AddRef( (*out = &impl->IVectorView_HSTRING_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_vector_view_AddRef( IVectorView_HSTRING *iface )
{
    struct hstring_vector_view *impl = impl_from_IVectorView_HSTRING( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_vector_view_Release( IVectorView_HSTRING *iface )
{
    struct hstring_vector_view *impl = impl_from_IVectorView_HSTRING( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI hstring_vector_view_GetIids( IVectorView_HSTRING *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_vector_view_GetRuntimeClassName( IVectorView_HSTRING *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_vector_view_GetTrustLevel( IVectorView_HSTRING *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_vector_view_GetAt( IVectorView_HSTRING *iface, UINT32 index, HSTRING *value )
{
    struct hstring_vector_view *impl = impl_from_IVectorView_HSTRING( iface );

    TRACE( "iface %p, index %u, value %p.\n", iface, index, value );

    *value = NULL;
    if (index >= impl->size) return E_BOUNDS;

    WindowsDuplicateString( impl->elements[index], value );

    return S_OK;
}

static HRESULT WINAPI hstring_vector_view_get_Size( IVectorView_HSTRING *iface, UINT32 *value )
{
    struct hstring_vector_view *impl = impl_from_IVectorView_HSTRING( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

    *value = impl->size;
    return S_OK;
}

static HRESULT WINAPI hstring_vector_view_IndexOf( IVectorView_HSTRING *iface, HSTRING element,
                                           UINT32 *index, BOOLEAN *found )
{
    struct hstring_vector_view *impl = impl_from_IVectorView_HSTRING( iface );
    ULONG i;

    TRACE( "iface %p, element %p, index %p, found %p.\n", iface, element, index, found );

    for (i = 0; i < impl->size; ++i) if (impl->elements[i] == element) break;
    if ((*found = (i < impl->size))) *index = i;
    else *index = 0;

    return S_OK;
}

static HRESULT WINAPI hstring_vector_view_GetMany( IVectorView_HSTRING *iface, UINT32 start_index,
                                           UINT32 items_size, HSTRING *items, UINT *count )
{
    struct hstring_vector_view *impl = impl_from_IVectorView_HSTRING( iface );
    UINT32 i;

    TRACE( "iface %p, start_index %u, items_size %u, items %p, count %p.\n",
           iface, start_index, items_size, items, count );

    if (start_index >= impl->size) return E_BOUNDS;

    for (i = start_index; i < impl->size; ++i)
    {
        if (i - start_index >= items_size) break;
        items[i - start_index] = impl->elements[i];
    }
    *count = i - start_index;

    return S_OK;
}

struct IVectorView_HSTRINGVtbl hstring_vector_view_vtbl =
{
    hstring_vector_view_QueryInterface,
    hstring_vector_view_AddRef,
    hstring_vector_view_Release,
    /* IInspectable methods */
    hstring_vector_view_GetIids,
    hstring_vector_view_GetRuntimeClassName,
    hstring_vector_view_GetTrustLevel,
    /* IVectorView<HSTRING> methods */
    hstring_vector_view_GetAt,
    hstring_vector_view_get_Size,
    hstring_vector_view_IndexOf,
    hstring_vector_view_GetMany,
};

DEFINE_IINSPECTABLE( hstring_iterable, IIterable_HSTRING, struct hstring_vector_view, IVectorView_HSTRING_iface )

static HRESULT WINAPI hstring_iterable_First( IIterable_HSTRING *iface, IIterator_HSTRING **value )
{
    struct hstring_vector_view *impl = impl_from_IIterable_HSTRING(iface);
    struct hstring_iterator *iter;

    TRACE("iface %p, value %p.\n", iface, value);

    if (!(iter = calloc(1, sizeof(*iter)))) return E_OUTOFMEMORY;
    iter->IIterator_HSTRING_iface.lpVtbl = &hstring_iterator_vtbl;
    iter->ref = 1;

    IVectorView_HSTRING_AddRef((iter->view = &impl->IVectorView_HSTRING_iface));
    iter->size = impl->size;

    *value = &iter->IIterator_HSTRING_iface;
    return S_OK;
}

struct IIterable_HSTRINGVtbl hstring_iterable_vtbl =
{
    hstring_iterable_QueryInterface,
    hstring_iterable_AddRef,
    hstring_iterable_Release,
    /* IInspectable methods */
    hstring_iterable_GetIids,
    hstring_iterable_GetRuntimeClassName,
    hstring_iterable_GetTrustLevel,
    /* IIterable<HSTRING> methods */
    hstring_iterable_First
};