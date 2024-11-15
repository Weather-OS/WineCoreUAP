/* WinRT Windows.Gaming.Input implementation
 *
 * Copyright 2021 Rémi Bernon for CodeWeavers
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

#include "../../private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(combase);

struct hstring_key
{
    IKeyValuePair_HSTRING_IInspectable IKeyValuePair_HSTRING_IInspectable_iface;
    HSTRING Key;
    IInspectable *Value;
    const GUID *iid;

    LONG ref;
};


static inline struct hstring_key *impl_from_IKeyValuePair_HSTRING_IInspectable( IKeyValuePair_HSTRING_IInspectable *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_key, IKeyValuePair_HSTRING_IInspectable_iface );
}

static HRESULT WINAPI hstring_key_QueryInterface( IKeyValuePair_HSTRING_IInspectable *iface, REFIID iid, void **out )
{
    struct hstring_key *impl = impl_from_IKeyValuePair_HSTRING_IInspectable( iface );

    TRACE( "iface %p, iid %s, out %p.n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iid ))
    {
        IInspectable_AddRef( (*out = &impl->IKeyValuePair_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_key_AddRef( IKeyValuePair_HSTRING_IInspectable *iface )
{
    struct hstring_key *impl = impl_from_IKeyValuePair_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_key_Release( IKeyValuePair_HSTRING_IInspectable *iface )
{
    struct hstring_key *impl = impl_from_IKeyValuePair_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI hstring_key_GetIids( IKeyValuePair_HSTRING_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_key_GetRuntimeClassName( IKeyValuePair_HSTRING_IInspectable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_key_GetTrustLevel( IKeyValuePair_HSTRING_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_key_get_Key( IKeyValuePair_HSTRING_IInspectable *iface, HSTRING *value )
{
    struct hstring_key *impl = impl_from_IKeyValuePair_HSTRING_IInspectable( iface );
    return WindowsDuplicateString( impl->Key, value );
}

static HRESULT WINAPI hstring_key_get_Value( IKeyValuePair_HSTRING_IInspectable *iface, IInspectable **value )
{
    struct hstring_key *impl = impl_from_IKeyValuePair_HSTRING_IInspectable( iface );

    TRACE( "iface %p, value %p.n", iface, value );

    *value = impl->Value;
    return S_OK;
}

static const IKeyValuePair_HSTRING_IInspectableVtbl hstring_key_vtbl =
{
    hstring_key_QueryInterface,
    hstring_key_AddRef,
    hstring_key_Release,
    /* IInspectable methods */
    hstring_key_GetIids,
    hstring_key_GetRuntimeClassName,
    hstring_key_GetTrustLevel,
    /* IKeyValuePair<K, T> methods */
    hstring_key_get_Key,
    hstring_key_get_Value
};

struct hstring_map_view
{
    IMapView_HSTRING_IInspectable IMapView_HSTRING_IInspectable_iface;
    IIterable_IKeyValuePair_HSTRING_IInspectable IIterable_IKeyValuePair_HSTRING_IInspectable_iface;
    struct vector_iids iids;
    LONG ref;

    UINT32 size;
    IKeyValuePair_HSTRING_IInspectable **elements;
};

static inline struct hstring_map_view *impl_from_IMapView_HSTRING_IInspectable( IMapView_HSTRING_IInspectable *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_map_view, IMapView_HSTRING_IInspectable_iface );
}

static HRESULT WINAPI hstring_map_view_QueryInterface( IMapView_HSTRING_IInspectable *iface, REFIID iid, void **out )
{
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );

    TRACE( "iface %p, iid %s, out %p.n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iids.view ))
    {
        IInspectable_AddRef( (*out = &impl->IMapView_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, impl->iids.iterable ))
    {
        IInspectable_AddRef( (*out = &impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_map_view_AddRef( IMapView_HSTRING_IInspectable *iface )
{
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_map_view_Release( IMapView_HSTRING_IInspectable *iface )
{
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );
    ULONG i, ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.n", iface, ref );

    if (!ref)
    {
        for (i = 0; i < impl->size; ++i) IKeyValuePair_HSTRING_IInspectable_Release( impl->elements[i] );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI hstring_map_view_GetIids( IMapView_HSTRING_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_view_GetRuntimeClassName( IMapView_HSTRING_IInspectable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_view_GetTrustLevel( IMapView_HSTRING_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_view_Lookup( IMapView_HSTRING_IInspectable *iface, HSTRING key, IInspectable **value )
{
    HRESULT status;
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    TRACE( "iface %p, value %p.n", iface, value );

    for ( i = 0; i < impl->size; i++ )
    {
        status = IKeyValuePair_HSTRING_IInspectable_get_Key( impl->elements[i], &currKeyStr );
        if ( SUCCEEDED( status ) )
        {
            WindowsCompareStringOrdinal( key, currKeyStr, &comparisonResult );
            if ( !comparisonResult )
                return IKeyValuePair_HSTRING_IInspectable_get_Value( impl->elements[i], value );
        }
    }

    return S_OK;
}

static HRESULT WINAPI hstring_map_view_get_Size( IMapView_HSTRING_IInspectable *iface, UINT32 *value )
{
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );

    TRACE( "iface %p, value %p.n", iface, value );

    *value = impl->size;
    return S_OK;
}

static HRESULT WINAPI hstring_map_view_HasKey( IMapView_HSTRING_IInspectable *iface, HSTRING key, boolean *found )
{
    HRESULT status;
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    *found = FALSE;

    for ( i = 0; i < impl->size; i++ )
    {
        status = IKeyValuePair_HSTRING_IInspectable_get_Key( impl->elements[i], &currKeyStr );
        if ( SUCCEEDED( status ) )
        {
            WindowsCompareStringOrdinal( key, currKeyStr, &comparisonResult );
            if ( !comparisonResult )
            {
                *found = TRUE;
                break;
            }
        }
    }

    return S_OK;
}

static HRESULT WINAPI hstring_map_view_Split( IMapView_HSTRING_IInspectable *iface, IMapView_HSTRING_IInspectable **first, IMapView_HSTRING_IInspectable **second )
{
    UINT32 iter;
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );

    struct hstring_map_view *firstMap;
    struct hstring_map_view *secondMap;

    if (!(firstMap = calloc( 1, sizeof(*firstMap) ))) return E_OUTOFMEMORY;
    if (!(secondMap = calloc( 1, sizeof(*secondMap) ))) return E_OUTOFMEMORY;

    firstMap->IMapView_HSTRING_IInspectable_iface.lpVtbl = impl->IMapView_HSTRING_IInspectable_iface.lpVtbl;
    firstMap->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl;
    firstMap->ref = 1;

    secondMap->IMapView_HSTRING_IInspectable_iface.lpVtbl = impl->IMapView_HSTRING_IInspectable_iface.lpVtbl;
    secondMap->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl;
    secondMap->ref = 1;

    if ( impl->size <= 1 )
        return E_FAIL;

    if ( impl->size % 2 == 0 )
    {
        firstMap->size = secondMap->size = impl->size / 2;
    } else
    {
        firstMap->size = (impl->size + 1) / 2;
        secondMap->size = firstMap->size - 1;
    }

    firstMap->elements = realloc( firstMap->elements, firstMap->size * sizeof( IKeyValuePair_HSTRING_IInspectable* ) );
    secondMap->elements = realloc( secondMap->elements, secondMap->size * sizeof( IKeyValuePair_HSTRING_IInspectable* ) );

    for ( iter = 0; iter < firstMap->size; iter++ )
    {
        firstMap->elements[iter] = impl->elements[iter];
    }

    for ( ; iter < secondMap->size; iter++ )
    {
        secondMap->elements[iter] = impl->elements[iter];
    }

    *first = &firstMap->IMapView_HSTRING_IInspectable_iface;
    *second = &secondMap->IMapView_HSTRING_IInspectable_iface;

    return S_OK;
}

static const IMapView_HSTRING_IInspectableVtbl hstring_map_view_vtbl =
{
    hstring_map_view_QueryInterface,
    hstring_map_view_AddRef,
    hstring_map_view_Release,
    /* IInspectable methods */
    hstring_map_view_GetIids,
    hstring_map_view_GetRuntimeClassName,
    hstring_map_view_GetTrustLevel,
    /* IMapView<K, T> methods */
    hstring_map_view_Lookup,
    hstring_map_view_get_Size,
    hstring_map_view_HasKey,
    hstring_map_view_Split,
};

DEFINE_IINSPECTABLE_( iterable_hstring_map_view, IIterable_IKeyValuePair_HSTRING_IInspectable, struct hstring_map_view, hstring_map_view_impl_from_IIterable_IKeyValuePair_HSTRING_IInspectable,
                      IIterable_IKeyValuePair_HSTRING_IInspectable_iface, &impl->IMapView_HSTRING_IInspectable_iface )

static HRESULT WINAPI iterable_hstring_map_view_First( IIterable_IKeyValuePair_HSTRING_IInspectable *iface, IIterator_IKeyValuePair_HSTRING_IInspectable **value )
{
    IVector_IInspectable *vector;
    struct hstring_map_view *impl = hstring_map_view_impl_from_IIterable_IKeyValuePair_HSTRING_IInspectable( iface );
    DEFINE_VECTOR_IIDS( IInspectable );
    
    TRACE( "iface %p, value %p.\n", iface, value );

    IInspectable_iids.iterable = &IID_IIterable_IKeyValuePair_HSTRING_IInspectable;
    IInspectable_iids.iterator = &IID_IIterator_IKeyValuePair_HSTRING_IInspectable;

    vector_create( &IInspectable_iids, (void **)&vector );

    IVector_IInspectable_ReplaceAll( vector, impl->size, (IInspectable **)impl->elements );
    return IVector_IInspectable_QueryInterface( vector, &IID_IVector_IInspectable, (void **)value );
}

static const IIterable_IKeyValuePair_HSTRING_IInspectableVtbl iterable_hstring_map_view_vtbl =
{
    iterable_hstring_map_view_QueryInterface,
    iterable_hstring_map_view_AddRef,
    iterable_hstring_map_view_Release,
    /* IInspectable methods */
    iterable_hstring_map_view_GetIids,
    iterable_hstring_map_view_GetRuntimeClassName,
    iterable_hstring_map_view_GetTrustLevel,
    /* IIterable<IKeyValuePair<K, T>> methods */
    iterable_hstring_map_view_First,
};

struct hstring_map_changed_event_handler
{
    IMapChangedEventHandler_HSTRING_IInspectable *mapEventHandler;
    EventRegistrationToken token;
    BOOL isStillAvailable;
};

struct hstring_map_changed_event_args
{
    IMapChangedEventArgs_HSTRING IMapChangedEventArgs_HSTRING_iface;
    HSTRING Key;
    CollectionChange Change;
    LONG ref;
};

DEFINE_IINSPECTABLE( hstring_map_changed_event_args, IMapChangedEventArgs_HSTRING, struct hstring_map_changed_event_args, IMapChangedEventArgs_HSTRING_iface )

static HRESULT WINAPI hstring_map_changed_event_args_get_CollectionChange( IMapChangedEventArgs_HSTRING *iface, CollectionChange *change )
{
    struct hstring_map_changed_event_args *impl = impl_from_IMapChangedEventArgs_HSTRING( iface );
    *change = impl->Change;
    return S_OK;
}

static HRESULT WINAPI hstring_map_changed_event_args_get_Key( IMapChangedEventArgs_HSTRING *iface, HSTRING *key )
{
    struct hstring_map_changed_event_args *impl = impl_from_IMapChangedEventArgs_HSTRING( iface );
    WindowsDuplicateString( impl->Key, key );
    return S_OK;
}

static const struct IMapChangedEventArgs_HSTRINGVtbl hstring_map_changed_event_args_vtbl =
{
    hstring_map_changed_event_args_QueryInterface,
    hstring_map_changed_event_args_AddRef,
    hstring_map_changed_event_args_Release,
    /* IInspectable methods */
    hstring_map_changed_event_args_GetIids,
    hstring_map_changed_event_args_GetRuntimeClassName,
    hstring_map_changed_event_args_GetTrustLevel,
    /* IMapChangedEventArgs<K> methods */
    hstring_map_changed_event_args_get_CollectionChange,
    hstring_map_changed_event_args_get_Key
};

struct hstring_map
{
    IMap_HSTRING_IInspectable IMap_HSTRING_IInspectable_iface;
    IIterable_IKeyValuePair_HSTRING_IInspectable IIterable_IKeyValuePair_HSTRING_IInspectable_iface;
    IObservableMap_HSTRING_IInspectable IObservableMap_HSTRING_IInspectable_iface;
    struct vector_iids iids;
    LONG ref;

    UINT32 size;
    UINT32 capacity;
    IKeyValuePair_HSTRING_IInspectable **elements;

    struct hstring_map_changed_event_handler **mapEventHandlers;
    UINT32 handlerSize;
    UINT32 handlerCapacity;
};

static inline struct hstring_map *impl_from_IMap_HSTRING_IInspectable( IMap_HSTRING_IInspectable *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_map, IMap_HSTRING_IInspectable_iface );
}

static HRESULT WINAPI hstring_map_QueryInterface( IMap_HSTRING_IInspectable *iface, REFIID iid, void **out )
{
    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iids.vector ))
    {
        IInspectable_AddRef( (*out = &impl->IMap_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, impl->iids.iterable ))
    {
        IInspectable_AddRef( (*out = &impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_map_AddRef( IMap_HSTRING_IInspectable *iface )
{
    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_map_Release( IMap_HSTRING_IInspectable *iface )
{
    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        IMap_HSTRING_IInspectable_Clear( iface );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI hstring_map_GetIids( IMap_HSTRING_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_GetRuntimeClassName( IMap_HSTRING_IInspectable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_GetTrustLevel( IMap_HSTRING_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_Lookup( IMap_HSTRING_IInspectable *iface, HSTRING key, IInspectable **value )
{
    HRESULT status;
    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    TRACE( "iface %p, value %p.n", iface, value );

    for ( i = 0; i < impl->size; i++ )
    {
        status = IKeyValuePair_HSTRING_IInspectable_get_Key( impl->elements[i], &currKeyStr );
        if ( SUCCEEDED( status ) )
        {
            WindowsCompareStringOrdinal( key, currKeyStr, &comparisonResult );
            if ( !comparisonResult )
                return IKeyValuePair_HSTRING_IInspectable_get_Value( impl->elements[i], value );
        }
    }

    return S_OK;
}

static HRESULT WINAPI hstring_map_get_Size( IMap_HSTRING_IInspectable *iface, UINT32 *value )
{
    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->size;
    return S_OK;
}

static HRESULT WINAPI hstring_map_HasKey( IMap_HSTRING_IInspectable *iface, HSTRING key, boolean *found )
{
    HRESULT status;
    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    *found = FALSE;

    for ( i = 0; i < impl->size; i++ )
    {
        status = IKeyValuePair_HSTRING_IInspectable_get_Key( impl->elements[i], &currKeyStr );
        if ( SUCCEEDED( status ) )
        {
            WindowsCompareStringOrdinal( key, currKeyStr, &comparisonResult );
            if ( !comparisonResult )
            {
                *found = TRUE;
                break;
            }
        }
    }

    return S_OK;
}

static HRESULT WINAPI hstring_map_GetView( IMap_HSTRING_IInspectable *iface, IMapView_HSTRING_IInspectable **value )
{
    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    struct hstring_map_view *view;
    ULONG i;

    TRACE( "iface %p, value %p.\n", iface, value );

    if (!(view = calloc( 1, sizeof(*view) ))) return E_OUTOFMEMORY;
    view->IMapView_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_view_vtbl;
    view->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &iterable_hstring_map_view_vtbl;
    view->iids = impl->iids;
    view->ref = 1;

    for (i = 0; i < impl->size; ++i) IKeyValuePair_HSTRING_IInspectable_AddRef( (view->elements[view->size++] = impl->elements[i]) );

    *value = &view->IMapView_HSTRING_IInspectable_iface;
    return S_OK;
}

static HRESULT WINAPI hstring_map_Insert( IMap_HSTRING_IInspectable* iface, HSTRING key, IInspectable *value, boolean *replaced )
{
    UINT32 eventIterator;
    boolean keyExists = FALSE;
    UINT32 i;
    INT32 comparisonResult;

    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    struct hstring_key *newKey;
    struct hstring_map_changed_event_args *changedArgs;

    IKeyValuePair_HSTRING_IInspectable **tmp = impl->elements;

    TRACE( "iface %p, value %p.\n", iface, value );

    if (!(changedArgs = calloc( 1, sizeof(*changedArgs) ))) return E_OUTOFMEMORY;

    changedArgs->IMapChangedEventArgs_HSTRING_iface.lpVtbl = &hstring_map_changed_event_args_vtbl;
    WindowsDuplicateString( key, &changedArgs->Key );
    changedArgs->ref = 1;

    IMap_HSTRING_IInspectable_HasKey( iface, key, &keyExists );
    if ( keyExists )
    {
        for ( i = 0; i < impl->size; i++ )
        {
            newKey = impl_from_IKeyValuePair_HSTRING_IInspectable( impl->elements[i] );
            WindowsCompareStringOrdinal( newKey->Key, key, &comparisonResult );
            if ( !comparisonResult )
            {
                newKey->Value = value;
                changedArgs->Change = CollectionChange_ItemChanged;
                *replaced = TRUE;
                break;
            }
        }
    }
    else if (impl->size == impl->capacity)
    {
        impl->capacity = max( 32, impl->capacity * 3 / 2 );

        if (!(impl->elements = realloc( impl->elements, impl->capacity * sizeof(*impl->elements) )))
        {
            impl->elements = tmp;
            return E_OUTOFMEMORY;
        }

        if (!(newKey = calloc( 1, sizeof(*newKey) ))) return E_OUTOFMEMORY;

        newKey->IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &hstring_key_vtbl;
        WindowsDuplicateString( key, &newKey->Key );
        newKey->Value = value;

        changedArgs->Change = CollectionChange_ItemInserted;

        *replaced = FALSE;
    } else
    {
        return E_BOUNDS;
    }

    for ( eventIterator = 0; eventIterator < impl->handlerSize; eventIterator++ )
    {
        if ( impl->mapEventHandlers[eventIterator]->isStillAvailable )
            IMapChangedEventHandler_HSTRING_IInspectable_Invoke( impl->mapEventHandlers[eventIterator]->mapEventHandler, &impl->IObservableMap_HSTRING_IInspectable_iface, &changedArgs->IMapChangedEventArgs_HSTRING_iface );
    }

    return S_OK;
}

static HRESULT WINAPI hstring_map_Remove( IMap_HSTRING_IInspectable* iface, HSTRING key )
{
    UINT32 eventIterator;
    UINT32 i;
    INT32 comparisonResult;
    boolean keyExists = FALSE;

    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    struct hstring_key *existingKey;
    struct hstring_map_changed_event_args *changedArgs;

    if (!(changedArgs = calloc( 1, sizeof(*changedArgs) ))) return E_OUTOFMEMORY;

    changedArgs->IMapChangedEventArgs_HSTRING_iface.lpVtbl = &hstring_map_changed_event_args_vtbl;
    WindowsDuplicateString( key, &changedArgs->Key );
    changedArgs->ref = 1;

    IMap_HSTRING_IInspectable_HasKey( iface, key, &keyExists );
    if ( keyExists )
    {
        for ( i = 0; i < impl->size; i++ )
        {
            existingKey = impl_from_IKeyValuePair_HSTRING_IInspectable( impl->elements[i] );
            WindowsCompareStringOrdinal( existingKey->Key, key, &comparisonResult );
            if ( !comparisonResult )
            {
                changedArgs->Change = CollectionChange_ItemRemoved;
                IKeyValuePair_HSTRING_IInspectable_Release( impl->elements[i] );
                break;
            }
        }
    }

    if ( keyExists )
        for ( eventIterator = 0; eventIterator < impl->handlerSize; eventIterator++ )
        {
            if ( impl->mapEventHandlers[eventIterator]->isStillAvailable )
                IMapChangedEventHandler_HSTRING_IInspectable_Invoke( impl->mapEventHandlers[eventIterator]->mapEventHandler, &impl->IObservableMap_HSTRING_IInspectable_iface, &changedArgs->IMapChangedEventArgs_HSTRING_iface );
        }

    return S_OK;
}

static HRESULT WINAPI hstring_map_Clear( IMap_HSTRING_IInspectable* iface )
{
    UINT32 eventIterator;
    UINT32 i;

    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    struct hstring_map_changed_event_args *changedArgs;

    if (!(changedArgs = calloc( 1, sizeof(*changedArgs) ))) return E_OUTOFMEMORY;

    changedArgs->IMapChangedEventArgs_HSTRING_iface.lpVtbl = &hstring_map_changed_event_args_vtbl;
    changedArgs->Change = CollectionChange_Reset;
    changedArgs->ref = 1;

    for ( i = 0; i < impl->size; i++ )
        IKeyValuePair_HSTRING_IInspectable_Release( impl->elements[i] );
    
    for ( eventIterator = 0; eventIterator < impl->handlerSize; eventIterator++ )
    {
        if ( impl->mapEventHandlers[eventIterator]->isStillAvailable )
            IMapChangedEventHandler_HSTRING_IInspectable_Invoke( impl->mapEventHandlers[eventIterator]->mapEventHandler, &impl->IObservableMap_HSTRING_IInspectable_iface, &changedArgs->IMapChangedEventArgs_HSTRING_iface );
    }
    
    return S_OK;
}

static const struct IMap_HSTRING_IInspectableVtbl hstring_map_vtbl =
{
    hstring_map_QueryInterface,
    hstring_map_AddRef,
    hstring_map_Release,
    /* IInspectable methods */
    hstring_map_GetIids,
    hstring_map_GetRuntimeClassName,
    hstring_map_GetTrustLevel,
    /* IMap<HSTRING, IInspectable*> methods */
    hstring_map_Lookup,
    hstring_map_get_Size,
    hstring_map_HasKey,
    hstring_map_GetView,
    hstring_map_Insert,
    hstring_map_Remove,
    hstring_map_Clear
};

DEFINE_IINSPECTABLE( iterable_hstring_map, IIterable_IKeyValuePair_HSTRING_IInspectable, struct hstring_map, IIterable_IKeyValuePair_HSTRING_IInspectable_iface )

static HRESULT WINAPI iterable_hstring_map_First( IIterable_IKeyValuePair_HSTRING_IInspectable *iface, IIterator_IKeyValuePair_HSTRING_IInspectable **value )
{
    IVector_IInspectable *vector;
    struct hstring_map *impl = impl_from_IIterable_IKeyValuePair_HSTRING_IInspectable( iface );
    DEFINE_VECTOR_IIDS( IInspectable );
    
    TRACE( "iface %p, value %p.\n", iface, value );

    IInspectable_iids.iterable = &IID_IIterable_IKeyValuePair_HSTRING_IInspectable;
    IInspectable_iids.iterator = &IID_IIterator_IKeyValuePair_HSTRING_IInspectable;

    vector_create( &IInspectable_iids, (void **)&vector );

    IVector_IInspectable_ReplaceAll( vector, impl->size, (IInspectable **)impl->elements );
    return IVector_IInspectable_QueryInterface( vector, &IID_IVector_IInspectable, (void **)value );
}

static const struct IIterable_IKeyValuePair_HSTRING_IInspectableVtbl iterable_hstring_map_vtbl =
{
    iterable_hstring_map_QueryInterface,
    iterable_hstring_map_AddRef,
    iterable_hstring_map_Release,
    /* IInspectable methods */
    iterable_hstring_map_GetIids,
    iterable_hstring_map_GetRuntimeClassName,
    iterable_hstring_map_GetTrustLevel,
    /* IIterable<IKeyValuePair<K, T>> methods */
    iterable_hstring_map_First,
};

static inline struct hstring_map *impl_from_IObservableMap_HSTRING_IInspectable( IObservableMap_HSTRING_IInspectable *iface )
{
    return CONTAINING_RECORD( iface, struct hstring_map, IObservableMap_HSTRING_IInspectable_iface );
}

static HRESULT WINAPI observable_hstring_map_QueryInterface( IObservableMap_HSTRING_IInspectable *iface, REFIID iid, void **out )
{
    struct hstring_map *impl = impl_from_IObservableMap_HSTRING_IInspectable( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iids.observableVector ))
    {
        IInspectable_AddRef( (*out = &impl->IObservableMap_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, impl->iids.vector ))
    {
        IInspectable_AddRef( (*out = &impl->IMap_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, impl->iids.iterable ))
    {
        IInspectable_AddRef( (*out = &impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI observable_hstring_map_AddRef( IObservableMap_HSTRING_IInspectable *iface )
{
    struct hstring_map *impl = impl_from_IObservableMap_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI observable_hstring_map_Release( IObservableMap_HSTRING_IInspectable *iface )
{
    struct hstring_map *impl = impl_from_IObservableMap_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        IMap_HSTRING_IInspectable_Clear( &impl->IMap_HSTRING_IInspectable_iface );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI observable_hstring_map_GetIids( IObservableMap_HSTRING_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI observable_hstring_map_GetRuntimeClassName( IObservableMap_HSTRING_IInspectable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI observable_hstring_map_GetTrustLevel( IObservableMap_HSTRING_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI observable_hstring_map_add_MapChanged( IObservableMap_HSTRING_IInspectable *iface, IMapChangedEventHandler_HSTRING_IInspectable *handler, EventRegistrationToken *token )
{
    EventRegistrationToken registeredToken;
    struct hstring_map *impl = impl_from_IObservableMap_HSTRING_IInspectable( iface );
    struct hstring_map_changed_event_handler **tmp = impl->mapEventHandlers;

    if (impl->handlerSize == impl->handlerCapacity)
    {
        impl->handlerCapacity = max( 32, impl->handlerCapacity * 3 / 2 );
        if (!(impl->mapEventHandlers = realloc( impl->mapEventHandlers, impl->handlerCapacity * sizeof(*impl->mapEventHandlers) )))
        {
            impl->mapEventHandlers = tmp;
            return E_OUTOFMEMORY;
        }
    }

    registeredToken.value = impl->handlerSize;

    impl->mapEventHandlers[impl->handlerSize]->token = registeredToken;
    impl->mapEventHandlers[impl->handlerSize]->mapEventHandler = handler;
    impl->mapEventHandlers[impl->handlerSize]->isStillAvailable = TRUE;

    *token = registeredToken;

    impl->handlerSize++;

    return S_OK;
}

static HRESULT WINAPI observable_hstring_map_remove_MapChanged( IObservableMap_HSTRING_IInspectable *iface, EventRegistrationToken token )
{
    struct hstring_map *impl = impl_from_IObservableMap_HSTRING_IInspectable( iface );

    //Tokens remain catenated 
    IMapChangedEventHandler_HSTRING_IInspectable_Release( impl->mapEventHandlers[token.value]->mapEventHandler );
    impl->mapEventHandlers[token.value]->isStillAvailable = FALSE;
    
    return S_OK;
}

static const struct IObservableMap_HSTRING_IInspectableVtbl observable_hstring_map_vtbl =
{
    observable_hstring_map_QueryInterface,
    observable_hstring_map_AddRef,
    observable_hstring_map_Release,
    /* IInspectable methods */
    observable_hstring_map_GetIids,
    observable_hstring_map_GetRuntimeClassName,
    observable_hstring_map_GetTrustLevel,
    /* IObservableMap<K, T> methods */
    observable_hstring_map_add_MapChanged,
    observable_hstring_map_remove_MapChanged
};

//All of this fits together to form IPropertySet and IValueSet
struct property_set
{
    IPropertySet IPropertySet_iface;
    struct vector_iids iids;
    //Inheritence tree
        IObservableMap_HSTRING_IInspectable *map;
    LONG ref;
};

static inline struct property_set *impl_from_IPropertySet( IPropertySet *iface )
{
    return CONTAINING_RECORD( iface, struct property_set, IPropertySet_iface );
}

static HRESULT WINAPI property_set_QueryInterface( IPropertySet *iface, REFIID iid, void **out )
{
    struct property_set *impl = impl_from_IPropertySet( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IPropertySet ))
    {
        IInspectable_AddRef( (*out = &impl->IPropertySet_iface) );
        return S_OK;
    }

    return IObservableMap_HSTRING_IInspectable_QueryInterface( impl->map, iid, out );
}

static ULONG WINAPI property_set_AddRef( IPropertySet *iface )
{
    struct property_set *impl = impl_from_IPropertySet( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI property_set_Release( IPropertySet *iface )
{
    struct property_set *impl = impl_from_IPropertySet( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
        free( impl );

    return ref;
}

static HRESULT WINAPI property_set_GetIids( IPropertySet *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI property_set_GetRuntimeClassName( IPropertySet *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI property_set_GetTrustLevel( IPropertySet *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static const struct IPropertySetVtbl property_set_vtbl =
{
    property_set_QueryInterface,
    property_set_AddRef,
    property_set_Release,
    /* IInspectable methods */
    property_set_GetIids,
    property_set_GetRuntimeClassName,
    property_set_GetTrustLevel
};

struct hstring_map_event_handler
{
    IMapChangedEventHandler_HSTRING_IInspectable IMapChangedEventHandler_HSTRING_IInspectable_iface;
    observable_hstring_map_callback callback;
    LONG ref;
};

DEFINE_IUNKNOWN( hstring_map_event_handler, IMapChangedEventHandler_HSTRING_IInspectable, struct hstring_map_event_handler, IMapChangedEventHandler_HSTRING_IInspectable_iface )

static HRESULT WINAPI hstring_map_event_handler_Invoke( IMapChangedEventHandler_HSTRING_IInspectable *iface, IObservableMap_HSTRING_IInspectable *sender, IMapChangedEventArgs_HSTRING *args )
{
    struct hstring_map_event_handler *impl = impl_from_IMapChangedEventHandler_HSTRING_IInspectable( iface );
    return impl->callback( sender, args );
}

static const struct IMapChangedEventHandler_HSTRING_IInspectableVtbl hstring_map_event_handler_vtbl =
{
    hstring_map_event_handler_QueryInterface,
    hstring_map_event_handler_AddRef,
    hstring_map_event_handler_Release,
    /* IMapChangedEventHandler<K, T> methods */
    hstring_map_event_handler_Invoke
};

HRESULT hstring_map_create( const struct vector_iids *iids, void **out )
{
    struct hstring_map *impl;

    TRACE( "iid %s, out %p.\n", debugstr_guid( iids->vector ), out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IMap_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_vtbl;
    impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &iterable_hstring_map_vtbl;
    impl->iids = *iids;
    impl->ref = 1;

    *out = &impl->IMap_HSTRING_IInspectable_iface;
    TRACE( "created %p\n", *out );
    return S_OK;
}

HRESULT observable_hstring_map_create( const struct vector_iids *iids, void **out )
{
    struct hstring_map *impl;

    TRACE( "iid %s, out %p.\n", debugstr_guid( iids->vector ), out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IMap_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_vtbl;
    impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &iterable_hstring_map_vtbl;
    impl->IObservableMap_HSTRING_IInspectable_iface.lpVtbl = &observable_hstring_map_vtbl;
    impl->iids = *iids;
    impl->ref = 1;

    *out = &impl->IObservableMap_HSTRING_IInspectable_iface;
    TRACE( "created %p\n", *out );
    return S_OK;
}

HRESULT hstring_map_event_handler_create( observable_hstring_map_callback callback, IMapChangedEventHandler_HSTRING_IInspectable **out )
{
    struct hstring_map_event_handler *impl;

    TRACE( "out %p.\n", out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IMapChangedEventHandler_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_event_handler_vtbl;
    impl->callback = callback;
    impl->ref = 1;

    *out = &impl->IMapChangedEventHandler_HSTRING_IInspectable_iface;
    TRACE( "created %p\n", *out );
    return S_OK;
}

HRESULT property_set_create( const struct vector_iids *iids, IPropertySet **out )
{
    struct property_set *impl;

    TRACE( "iid %s, out %p.\n", debugstr_guid( iids->vector ), out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    
    impl->IPropertySet_iface.lpVtbl = &property_set_vtbl;
    impl->iids = *iids;
    impl->ref = 1;

    return observable_hstring_map_create( iids, (void **)&impl->map );
}