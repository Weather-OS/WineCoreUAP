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

#include "HSTRINGVector.h"
#include "../VectorView/HSTRINGVectorView.h"

WINE_DEFAULT_DEBUG_CHANNEL(combase);

struct hstring_vector *impl_from_IVector_HSTRING( IVector_HSTRING *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_vector, IVector_HSTRING_iface );
}

static HRESULT WINAPI hstring_vector_QueryInterface( IVector_HSTRING *iface, REFIID iid, void **out )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iids.view ))
    {
        IInspectable_AddRef( (*out = &impl->IVector_HSTRING_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_vector_AddRef( IVector_HSTRING *iface )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_vector_Release( IVector_HSTRING *iface )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI hstring_vector_GetIids( IVector_HSTRING *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_vector_GetRuntimeClassName( IVector_HSTRING *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_vector_GetTrustLevel( IVector_HSTRING *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_vector_GetAt( IVector_HSTRING *iface, UINT32 index, HSTRING *value )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );

    TRACE( "iface %p, index %u, value %p.\n", iface, index, value );

    *value = NULL;
    if (index >= impl->size) return E_BOUNDS;

    WindowsDuplicateString( impl->elements[index], value );

    return S_OK;
}

static HRESULT WINAPI hstring_vector_get_Size( IVector_HSTRING *iface, UINT32 *value )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

    *value = impl->size;
    return S_OK;
}

static HRESULT WINAPI hstring_vector_GetView( IVector_HSTRING *iface, IVectorView_HSTRING **value )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    struct hstring_vector_view *view;

    if (!(view = calloc( 1, sizeof(*view) ))) return E_OUTOFMEMORY;

    view->IVectorView_HSTRING_iface.lpVtbl = &hstring_vector_view_vtbl;
    view->IIterable_HSTRING_iface.lpVtbl = &hstring_iterable_vtbl;
    view->elements = impl->elements;
    view->iids = impl->iids;
    view->size = impl->size;
    view->ref = 1;

    *value = &view->IVectorView_HSTRING_iface;

    TRACE( "iface %p, value %p\n", iface, value );
    
    return S_OK;
}

static HRESULT WINAPI hstring_vector_IndexOf( IVector_HSTRING *iface, HSTRING element,
                                           UINT32 *index, BOOLEAN *found )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    ULONG i;

    TRACE( "iface %p, element %p, index %p, found %p.\n", iface, element, index, found );

    for (i = 0; i < impl->size; ++i) if (impl->elements[i] == element) break;
    if ((*found = (i < impl->size))) *index = i;
    else *index = 0;

    return S_OK;
}

static HRESULT WINAPI hstring_vector_SetAt( IVector_HSTRING *iface, UINT32 index, HSTRING value )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );

    TRACE( "iface %p, index %u, value %p.\n", iface, index, value );

    if (index >= impl->size) return E_BOUNDS;

    WindowsDuplicateString( value, &impl->elements[index] );

    return S_OK;
}

static HRESULT WINAPI hstring_vector_InsertAt( IVector_HSTRING *iface, UINT32 index, HSTRING value )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    ULONG i;

    TRACE( "iface %p, index %u, value %p.\n", iface, index, value );
    
    if (index > impl->size) return E_BOUNDS;

    for ( i = index; i < impl->size; i++ )
    {
        WindowsDuplicateString( impl->elements[i], &impl->elements[i + 1] );
    }

    impl->size++;

    WindowsDuplicateString( value, &impl->elements[index] );

    return S_OK;
}

static HRESULT WINAPI hstring_vector_RemoveAt( IVector_HSTRING *iface, UINT32 index )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    ULONG i;

    TRACE( "iface %p, index %u.\n", iface, index );
    
    if (index >= impl->size) return E_BOUNDS;

    WindowsDeleteString( impl->elements[index] );

    for ( i = index; i < impl->size - 1; i++ )
    {
        WindowsDuplicateString( impl->elements[i + 1], &impl->elements[i] );
    }

    impl->size--;

    return S_OK;
}

static HRESULT WINAPI hstring_vector_Append( IVector_HSTRING *iface, HSTRING value )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

    WindowsDuplicateString( value, &impl->elements[impl->size] );

    impl->size++;

    return S_OK;
}

static HRESULT WINAPI hstring_vector_RemoveAtEnd( IVector_HSTRING *iface )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );

    TRACE( "iface %p.\n", iface );
    
    WindowsDeleteString( impl->elements[impl->size - 1] );

    impl->size--;

    return S_OK;
}

static HRESULT WINAPI hstring_vector_Clear( IVector_HSTRING *iface )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    ULONG i;

    TRACE( "iface %p.\n", iface );

    for (i = 0; i < impl->size; i++)
    {
        WindowsDeleteString( impl->elements[i] );
    }

    impl->size = 0;

    return S_OK;
}

static HRESULT WINAPI hstring_vector_GetMany( IVector_HSTRING *iface, UINT32 start_index,
                                           UINT32 items_size, HSTRING *items, UINT *count )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
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

static HRESULT WINAPI hstring_vector_ReplaceAll( IVector_HSTRING *iface, UINT32 count, HSTRING *items )
{
    struct hstring_vector *impl = impl_from_IVector_HSTRING( iface );
    ULONG i;

    TRACE( "iface %p, count %u, items %p.\n", iface, count, items );

    for (i = 0; i < impl->size; i++)
    {
        WindowsDeleteString( impl->elements[i] );
    }

    for (i = 0; i < count; i++)
    {
        WindowsDuplicateString( items[i], &impl->elements[i] );
    }

    impl->size = count;

    return S_OK;
}

struct IVector_HSTRINGVtbl hstring_vector_vtbl =
{
    hstring_vector_QueryInterface,
    hstring_vector_AddRef,
    hstring_vector_Release,
    /* IInspectable methods */
    hstring_vector_GetIids,
    hstring_vector_GetRuntimeClassName,
    hstring_vector_GetTrustLevel,
    /* IVector<HSTRING> methods */
    hstring_vector_GetAt,
    hstring_vector_get_Size,
    hstring_vector_GetView,
    hstring_vector_IndexOf,
    hstring_vector_SetAt,
    hstring_vector_InsertAt,
    hstring_vector_RemoveAt,
    hstring_vector_Append,
    hstring_vector_RemoveAtEnd,
    hstring_vector_Clear,
    hstring_vector_GetMany,
    hstring_vector_ReplaceAll,
};