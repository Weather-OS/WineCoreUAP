/* WinRT Windows.Foundation.IMap<K, V> implementation
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

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iid ))
    {
        IInspectable_AddRef( (*out = &impl->IKeyValuePair_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_key_AddRef( IKeyValuePair_HSTRING_IInspectable *iface )
{
    struct hstring_key *impl = impl_from_IKeyValuePair_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_key_Release( IKeyValuePair_HSTRING_IInspectable *iface )
{
    struct hstring_key *impl = impl_from_IKeyValuePair_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        WindowsDeleteString( impl->Key );
        IInspectable_Release( impl->Value );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI hstring_key_GetIids( IKeyValuePair_HSTRING_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_key_GetRuntimeClassName( IKeyValuePair_HSTRING_IInspectable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_key_GetTrustLevel( IKeyValuePair_HSTRING_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
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

    TRACE( "iface %p, value %p.\n", iface, value );

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
    struct map_iids iids;
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

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

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

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI hstring_map_view_AddRef( IMapView_HSTRING_IInspectable *iface )
{
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI hstring_map_view_Release( IMapView_HSTRING_IInspectable *iface )
{
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );
    ULONG i, ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        for ( i = 0; i < impl->size; ++i ) IKeyValuePair_HSTRING_IInspectable_Release( impl->elements[i] );
        IIterable_IKeyValuePair_HSTRING_IInspectable_Release( &impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface );
        free( impl );
    }

    return ref;
}

static HRESULT WINAPI hstring_map_view_GetIids( IMapView_HSTRING_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_view_GetRuntimeClassName( IMapView_HSTRING_IInspectable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_view_GetTrustLevel( IMapView_HSTRING_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI hstring_map_view_Lookup( IMapView_HSTRING_IInspectable *iface, HSTRING key, IInspectable **value )
{
    HRESULT status;
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

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

    return E_BOUNDS;
}

static HRESULT WINAPI hstring_map_view_get_Size( IMapView_HSTRING_IInspectable *iface, UINT32 *value )
{
    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

    *value = impl->size;
    return S_OK;
}

static HRESULT WINAPI hstring_map_view_HasKey( IMapView_HSTRING_IInspectable *iface, HSTRING key, boolean *found )
{
    HRESULT status;
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    struct hstring_map_view *impl = impl_from_IMapView_HSTRING_IInspectable( iface );

    TRACE( "iface %p, key %s, found %p.\n", iface, debugstr_hstring(key), found );

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

    if ( impl->size <= 1 ) return E_FAIL;

    if (!(firstMap = calloc( 1, sizeof(*firstMap) ))) return E_OUTOFMEMORY;
    if (!(secondMap = calloc( 1, sizeof(*secondMap) ))) return E_OUTOFMEMORY;

    firstMap->IMapView_HSTRING_IInspectable_iface.lpVtbl = impl->IMapView_HSTRING_IInspectable_iface.lpVtbl;
    firstMap->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl;
    firstMap->ref = 1;

    secondMap->IMapView_HSTRING_IInspectable_iface.lpVtbl = impl->IMapView_HSTRING_IInspectable_iface.lpVtbl;
    secondMap->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl;
    secondMap->ref = 1;

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
    if ( !firstMap->elements || !secondMap->elements ) return E_OUTOFMEMORY;

    for ( iter = 0; iter < firstMap->size; iter++ )
        IKeyValuePair_HSTRING_IInspectable_AddRef( firstMap->elements[iter] = impl->elements[iter] );

    for ( ; iter < secondMap->size; iter++ )
        IKeyValuePair_HSTRING_IInspectable_AddRef( secondMap->elements[iter] = impl->elements[iter] );

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
};

struct hstring_map_changed_event_args
{
    IMapChangedEventArgs_HSTRING IMapChangedEventArgs_HSTRING_iface;
    CollectionChange Change;    
    HSTRING Key;
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
    struct map_iids iids;

    IKeyValuePair_HSTRING_IInspectable **elements;
    UINT32 size;
    UINT32 capacity;

    CRITICAL_SECTION cs; // multiple asynchronous tasks can mess up the hstring map if they do write ops

    struct hstring_map_changed_event_handler **mapEventHandlers;
    UINT32 handlerSize;
    UINT32 handlerCapacity;    
    
    LONG ref;
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
        IsEqualGUID( iid, impl->iids.map ))
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
    UINT i;

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref)
    {
        IMap_HSTRING_IInspectable_Clear( iface );
        IIterable_IKeyValuePair_HSTRING_IInspectable_Release( &impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface );
        for ( i = 0; i < impl->handlerSize; i++ ) 
            free( impl->mapEventHandlers[i] );
        free( impl->mapEventHandlers );
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
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );

    TRACE( "iface %p, value %p.\n", iface, value );

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

    return E_BOUNDS;
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
    HSTRING currKeyStr;
    UINT32 i;
    INT32 comparisonResult;

    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );

    TRACE( "iface %p, key %s, found %p.\n", iface, debugstr_hstring(key), found );

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

    TRACE( "iface %p, value %p.\n", iface, value );

    if (!(view = calloc( 1, sizeof(*view) ))) return E_OUTOFMEMORY;

    view->IMapView_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_view_vtbl;
    view->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &iterable_hstring_map_view_vtbl;
    view->iids = impl->iids;
    view->size = impl->size;
    view->ref = 1;

    view->elements = impl->elements;

    *value = &view->IMapView_HSTRING_IInspectable_iface;
    return S_OK;
}

static HRESULT WINAPI hstring_map_Insert( IMap_HSTRING_IInspectable* iface, HSTRING key, IInspectable *value, boolean *replaced )
{    
    BOOLEAN keyExists = FALSE;
    UINT32 eventIterator;
    UINT32 newCapacity;
    UINT32 i;
    INT32 comparisonResult;

    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    struct hstring_key *newKey;
    struct hstring_map_changed_event_args *changedArgs;

    IKeyValuePair_HSTRING_IInspectable **tmp;

    TRACE( "iface %p, key %s, value %p.\n", iface, debugstr_hstring(key), value );

    EnterCriticalSection( &impl->cs );

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
                if ( replaced ) *replaced = TRUE;
                break;
            }
        }
    }
    else if (impl->size == impl->capacity)
    {
        newCapacity = impl->capacity + ( impl->capacity >> 1 );
        if ( newCapacity <= impl->capacity ) newCapacity = impl->capacity + 1;

        tmp = realloc( impl->elements, newCapacity * sizeof(*tmp) );
        if ( !tmp ) return E_OUTOFMEMORY;

        impl->elements = tmp;
        impl->capacity = newCapacity;

        if (!(newKey = calloc( 1, sizeof(*newKey) ))) return E_OUTOFMEMORY;

        newKey->IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &hstring_key_vtbl;
        WindowsDuplicateString( key, &newKey->Key );
        newKey->Value = value;

        impl->elements[impl->size++] = &newKey->IKeyValuePair_HSTRING_IInspectable_iface;
        changedArgs->Change = CollectionChange_ItemInserted;

        if ( replaced ) *replaced = FALSE;
    } else
        return E_BOUNDS;

    LeaveCriticalSection( &impl->cs );

    for ( eventIterator = 0; eventIterator < impl->handlerSize; eventIterator++ )
    {
        if ( impl->mapEventHandlers[eventIterator] )
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

    TRACE( "iface %p, key %s.\n", iface, debugstr_hstring(key) );

    EnterCriticalSection( &impl->cs );

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

    LeaveCriticalSection( &impl->cs );

    if ( keyExists )
        for ( eventIterator = 0; eventIterator < impl->handlerSize; eventIterator++ )
            if ( impl->mapEventHandlers[eventIterator] )
                IMapChangedEventHandler_HSTRING_IInspectable_Invoke( impl->mapEventHandlers[eventIterator]->mapEventHandler, &impl->IObservableMap_HSTRING_IInspectable_iface, &changedArgs->IMapChangedEventArgs_HSTRING_iface );

    return S_OK;
}

static HRESULT WINAPI hstring_map_Clear( IMap_HSTRING_IInspectable* iface )
{
    UINT32 eventIterator;
    UINT32 i;

    struct hstring_map *impl = impl_from_IMap_HSTRING_IInspectable( iface );
    struct hstring_map_changed_event_args *changedArgs;

    TRACE( "iface %p.\n", iface );

    EnterCriticalSection( &impl->cs );

    if (!(changedArgs = calloc( 1, sizeof(*changedArgs) ))) return E_OUTOFMEMORY;

    changedArgs->IMapChangedEventArgs_HSTRING_iface.lpVtbl = &hstring_map_changed_event_args_vtbl;
    changedArgs->Change = CollectionChange_Reset;
    changedArgs->ref = 1;

    for ( i = 0; i < impl->size; i++ )
        IKeyValuePair_HSTRING_IInspectable_Release( impl->elements[i] );
    
    for ( eventIterator = 0; eventIterator < impl->handlerSize; eventIterator++ )
    {
        if ( impl->mapEventHandlers[eventIterator] )
            IMapChangedEventHandler_HSTRING_IInspectable_Invoke( impl->mapEventHandlers[eventIterator]->mapEventHandler, &impl->IObservableMap_HSTRING_IInspectable_iface, &changedArgs->IMapChangedEventArgs_HSTRING_iface );
    }

    LeaveCriticalSection( &impl->cs );
    
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
        IsEqualGUID( iid, impl->iids.observableMap ))
    {
        IInspectable_AddRef( (*out = &impl->IObservableMap_HSTRING_IInspectable_iface) );
        return S_OK;
    }

    if (IsEqualGUID( iid, impl->iids.map ))
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
        IMap_HSTRING_IInspectable_Release( &impl->IMap_HSTRING_IInspectable_iface );
        IObservableMap_HSTRING_IInspectable_Release( &impl->IObservableMap_HSTRING_IInspectable_iface );
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
    UINT32 newCapacity;

    struct hstring_map *impl = impl_from_IObservableMap_HSTRING_IInspectable( iface );
    struct hstring_map_changed_event_handler **tmp;
    struct hstring_map_changed_event_handler *entry;

    TRACE( "iface %p, handler %p.\n", iface, handler );

    EnterCriticalSection( &impl->cs );

    if ( impl->handlerSize == impl->handlerCapacity )
    {
        newCapacity = impl->handlerCapacity
                  ? impl->handlerCapacity + ( impl->handlerCapacity >> 1 )
                  : 1;

        if (!(tmp = realloc( impl->mapEventHandlers, newCapacity * sizeof(*impl->mapEventHandlers) ))) return E_OUTOFMEMORY;
        impl->mapEventHandlers = tmp;
        impl->handlerCapacity = newCapacity;
    }

    if (!(entry = calloc( 1, sizeof(*entry) ))) return E_OUTOFMEMORY;

    entry->token.value = impl->handlerSize; //Quick and dirty way to set a token.
    entry->mapEventHandler = handler;
    registeredToken.value = impl->handlerSize;
    *token = registeredToken;

    impl->mapEventHandlers[entry->token.value] = entry;

    impl->handlerSize++;

    LeaveCriticalSection( &impl->cs );

    return S_OK;
}

static HRESULT WINAPI observable_hstring_map_remove_MapChanged( IObservableMap_HSTRING_IInspectable *iface, EventRegistrationToken token )
{
    size_t moveValue = token.value;
    struct hstring_map *impl = impl_from_IObservableMap_HSTRING_IInspectable( iface );

    TRACE( "iface %p, token %lld.\n", iface, token.value );

    EnterCriticalSection( &impl->cs );

    //Tokens remain catenated 
    IMapChangedEventHandler_HSTRING_IInspectable_Release( impl->mapEventHandlers[moveValue]->mapEventHandler );
    free( impl->mapEventHandlers[moveValue] );

    impl->mapEventHandlers[token.value] = NULL;
    impl->handlerSize--;
    impl->mapEventHandlers[moveValue] = impl->mapEventHandlers[impl->handlerSize];
    impl->mapEventHandlers[impl->handlerSize] = NULL;

    LeaveCriticalSection( &impl->cs );
    
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

struct hstring_map_event_handler
{
    IMapChangedEventHandler_HSTRING_IInspectable IMapChangedEventHandler_HSTRING_IInspectable_iface;
    observable_hstring_map_callback callback;
    IUnknown *data;
    LONG ref;
};

DEFINE_IUNKNOWN( hstring_map_event_handler, IMapChangedEventHandler_HSTRING_IInspectable, struct hstring_map_event_handler, IMapChangedEventHandler_HSTRING_IInspectable_iface )

static HRESULT WINAPI hstring_map_event_handler_Invoke( IMapChangedEventHandler_HSTRING_IInspectable *iface, IObservableMap_HSTRING_IInspectable *sender, IMapChangedEventArgs_HSTRING *args )
{
    struct hstring_map_event_handler *impl = impl_from_IMapChangedEventHandler_HSTRING_IInspectable( iface );
    return impl->callback( sender, impl->data, args );
}

static const struct IMapChangedEventHandler_HSTRING_IInspectableVtbl hstring_map_event_handler_vtbl =
{
    hstring_map_event_handler_QueryInterface,
    hstring_map_event_handler_AddRef,
    hstring_map_event_handler_Release,
    /* IMapChangedEventHandler<K, T> methods */
    hstring_map_event_handler_Invoke
};

HRESULT hstring_map_create( const struct map_iids *iids, void **out )
{
    struct hstring_map *impl;

    TRACE( "iid %s, out %p.\n", debugstr_guid( iids->map ), out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IMap_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_vtbl;
    impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &iterable_hstring_map_vtbl;
    impl->iids = *iids;
    impl->ref = 1;

    InitializeCriticalSectionEx( &impl->cs, 0, RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO );
    impl->cs.DebugInfo->Spare[0] = (DWORD_PTR)( __FILE__ ": wine_map.cs" );

    *out = &impl->IMap_HSTRING_IInspectable_iface;
    TRACE( "created %p\n", *out );
    return S_OK;
}

HRESULT observable_hstring_map_create( const struct map_iids *iids, void **out )
{
    struct hstring_map *impl;

    TRACE( "iid %s, out %p.\n", debugstr_guid( iids->map ), out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IMap_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_vtbl;
    impl->IIterable_IKeyValuePair_HSTRING_IInspectable_iface.lpVtbl = &iterable_hstring_map_vtbl;
    impl->IObservableMap_HSTRING_IInspectable_iface.lpVtbl = &observable_hstring_map_vtbl;
    impl->iids = *iids;
    impl->ref = 1;

    InitializeCriticalSectionEx( &impl->cs, 0, RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO );
    impl->cs.DebugInfo->Spare[0] = (DWORD_PTR)( __FILE__ ": wine_map.cs" );

    *out = &impl->IObservableMap_HSTRING_IInspectable_iface;
    TRACE( "created %p\n", *out );
    return S_OK;
}

HRESULT hstring_map_event_handler_create( observable_hstring_map_callback callback, OPTIONAL IUnknown *data, IMapChangedEventHandler_HSTRING_IInspectable **out )
{
    struct hstring_map_event_handler *impl;

    TRACE( "out %p.\n", out );

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;
    impl->IMapChangedEventHandler_HSTRING_IInspectable_iface.lpVtbl = &hstring_map_event_handler_vtbl;
    impl->callback = callback;
    impl->data = data;
    impl->ref = 1;

    *out = &impl->IMapChangedEventHandler_HSTRING_IInspectable_iface;
    TRACE( "created %p\n", *out );
    return S_OK;
}