/* WinRT Windows.Storage.Search.QueryOptions Implementation
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

#include "QueryOptionsInternal.h"

_ENABLE_DEBUGGING_

// Query Options

static struct query_options_factory *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct query_options_factory, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct query_options_factory *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IQueryOptionsFactory))
    {
        *out = &impl->IQueryOptionsFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct query_options_factory *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct query_options_factory *impl = impl_from_IActivationFactory( iface );
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

struct query_options_factory *impl_from_IQueryOptionsFactory( IQueryOptionsFactory *iface )
{
    return CONTAINING_RECORD( iface, struct query_options_factory, IQueryOptionsFactory_iface );
}

static HRESULT WINAPI query_options_factory_QueryInterface( IQueryOptionsFactory *iface, REFIID iid, void **out )
{
    struct query_options_factory *impl = impl_from_IQueryOptionsFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IQueryOptionsFactory))
    {
        *out = &impl->IQueryOptionsFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI query_options_factory_AddRef( IQueryOptionsFactory *iface )
{
    struct query_options_factory *impl = impl_from_IQueryOptionsFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI query_options_factory_Release( IQueryOptionsFactory *iface )
{
    struct query_options_factory *impl = impl_from_IQueryOptionsFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI query_options_factory_GetIids( IQueryOptionsFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_options_factory_GetRuntimeClassName( IQueryOptionsFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_options_factory_GetTrustLevel( IQueryOptionsFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_options_factory_CreateCommonFileQuery( IQueryOptionsFactory *iface, CommonFileQuery query, IIterable_HSTRING *fileTypeFilter, IQueryOptions **value )
{
    IIterator_HSTRING *fileTypeFilterIterator = NULL;

    HRESULT hr;
    HSTRING currentTypeFilter;
    BOOLEAN nextKeyIsAvail;

    struct query_options *queryOptions;
    SortEntry sortEntry;

    //non-runtime class require redefinition
    #undef SortEntry
    DEFINE_VECTOR_IIDS( SortEntry )
    #define SortEntry __x_ABI_CWindows_CStorage_CSearch_CSortEntry

    if (!(queryOptions = calloc( 1, sizeof(*queryOptions) ))) return E_OUTOFMEMORY;

    queryOptions->IQueryOptions_iface.lpVtbl = &query_options_vtbl;

    hr = hstring_vector_create( &queryOptions->FileTypeFilters );
    if ( FAILED( hr ) ) return hr;

    hr = vector_create( &SortEntry_iids, (void **)&queryOptions->SortOrders );

    if ( fileTypeFilter )
    {    

        hr = IIterable_HSTRING_First( fileTypeFilter, &fileTypeFilterIterator );
        if ( FAILED( hr ) ) return hr;

        IIterator_HSTRING_get_HasCurrent( fileTypeFilterIterator, &nextKeyIsAvail );

        while ( nextKeyIsAvail )
        {
            IIterator_HSTRING_get_Current( fileTypeFilterIterator, &currentTypeFilter );
            IVector_HSTRING_Append( queryOptions->FileTypeFilters, currentTypeFilter );
            IIterator_HSTRING_MoveNext( fileTypeFilterIterator, &nextKeyIsAvail );
        }
    }
    
    sortEntry.AscendingOrder = TRUE;

    switch ( query )
    {
        case CommonFileQuery_DefaultQuery:
            //Default sorting query is System.Name
            WindowsCreateString( L"System.Name", wcslen( L"System.Name" ), &sortEntry.PropertyName );
            break;

        case CommonFileQuery_OrderByDate:
            WindowsCreateString( L"System.DateModified", wcslen( L"System.DateModified" ), &sortEntry.PropertyName );
            break;
        
        case CommonFileQuery_OrderByMusicProperties:
            return E_NOTIMPL;

        case CommonFileQuery_OrderByName:
            WindowsCreateString( L"System.Name", wcslen( L"System.Name" ), &sortEntry.PropertyName );
            break;
        
        case CommonFileQuery_OrderBySearchRank:
            return E_NOTIMPL;

        case CommonFileQuery_OrderByTitle:
            WindowsCreateString( L"System.Title", wcslen( L"System.Title" ), &sortEntry.PropertyName );
            break;

        default:
            return E_NOTIMPL;
    }

    queryOptions->Depth = FolderDepth_Shallow;
    queryOptions->Indexer = IndexerOption_DoNotUseIndexer;
    queryOptions->DateStack = DateStackOption_None;
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->ApplicationSearchFilter );
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->UserSearchFilter );
    WindowsCreateString( L"en", wcslen( L"en" ), &queryOptions->Language );
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->GroupPropertyName );
    queryOptions->ref = 1;

    *value = &queryOptions->IQueryOptions_iface;
    return S_OK;
}

static HRESULT WINAPI query_options_factory_CreateCommonFolderQuery( IQueryOptionsFactory *iface, CommonFolderQuery query, IQueryOptions **value )
{
    HRESULT hr;

    struct query_options *queryOptions;
    SortEntry sortEntry;

    //non-runtime class require redefinition
    #undef SortEntry
    DEFINE_VECTOR_IIDS( SortEntry )
    #define SortEntry __x_ABI_CWindows_CStorage_CSearch_CSortEntry

    if (!(queryOptions = calloc( 1, sizeof(*queryOptions) ))) return E_OUTOFMEMORY;

    queryOptions->IQueryOptions_iface.lpVtbl = &query_options_vtbl;

    hr = vector_create( &SortEntry_iids, (void **)&queryOptions->SortOrders );
    if ( FAILED( hr ) ) return hr;

    sortEntry.AscendingOrder = TRUE;

    switch ( query )
    {
        case CommonFolderQuery_DefaultQuery:
            //Default sorting query is System.Name
            WindowsCreateString( L"System.Name", wcslen( L"System.Name" ), &sortEntry.PropertyName );
            break;

        default:
            return E_NOTIMPL;
    }

    queryOptions->Depth = FolderDepth_Shallow;
    queryOptions->Indexer = IndexerOption_DoNotUseIndexer;
    queryOptions->DateStack = DateStackOption_None;
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->ApplicationSearchFilter );
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->UserSearchFilter );
    WindowsCreateString( L"en", wcslen( L"en" ), &queryOptions->Language );
    WindowsCreateString( L"", wcslen( L"" ), &queryOptions->GroupPropertyName );
    queryOptions->ref = 1;

    *value = &queryOptions->IQueryOptions_iface;
    return S_OK;
}

const struct IQueryOptionsFactoryVtbl query_options_factory_vtbl =
{
    query_options_factory_QueryInterface,
    query_options_factory_AddRef,
    query_options_factory_Release,
    /* IInspectable methods */
    query_options_factory_GetIids,
    query_options_factory_GetRuntimeClassName,
    query_options_factory_GetTrustLevel,
    /* IQueryOptionsFactory methods */
    query_options_factory_CreateCommonFileQuery,
    query_options_factory_CreateCommonFolderQuery

};

DEFINE_IINSPECTABLE( query_options, IQueryOptions, struct query_options, IQueryOptions_iface )

static HRESULT WINAPI query_options_get_FileTypeFilter( IQueryOptions *iface, IVector_HSTRING **value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->FileTypeFilters;
    return S_OK;
}

static HRESULT WINAPI query_options_get_FolderDepth( IQueryOptions *iface, FolderDepth *value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->Depth;
    return S_OK;
}

static HRESULT WINAPI query_options_put_FolderDepth( IQueryOptions *iface, FolderDepth value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %d\n", iface, value );
    impl->Depth = value;
    return S_OK;
}

static HRESULT WINAPI query_options_get_ApplicationSearchFilter( IQueryOptions *iface, HSTRING *value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    WindowsDuplicateString( impl->ApplicationSearchFilter, value );
    return S_OK;
}

static HRESULT WINAPI query_options_put_ApplicationSearchFilter( IQueryOptions *iface, HSTRING value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    WindowsDuplicateString( value, &impl->ApplicationSearchFilter );
    return S_OK;
}

static HRESULT WINAPI query_options_get_UserSearchFilter( IQueryOptions *iface, HSTRING *value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    WindowsDuplicateString( impl->UserSearchFilter, value );
    return S_OK;
}

static HRESULT WINAPI query_options_put_UserSearchFilter( IQueryOptions *iface, HSTRING value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    WindowsDuplicateString( value, &impl->UserSearchFilter );
    return S_OK;
}

static HRESULT WINAPI query_options_get_Language( IQueryOptions *iface, HSTRING *value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    WindowsDuplicateString( impl->Language, value );
    return S_OK;
}

static HRESULT WINAPI query_options_put_Language( IQueryOptions *iface, HSTRING value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    WindowsDuplicateString( value, &impl->Language );
    return S_OK;
}

static HRESULT WINAPI query_options_get_IndexerOption( IQueryOptions *iface, IndexerOption *value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->Indexer;
    return S_OK;
}

static HRESULT WINAPI query_options_put_IndexerOption( IQueryOptions *iface, IndexerOption value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %d\n", iface, value );
    impl->Indexer = value;
    return S_OK;
}

static HRESULT WINAPI query_options_get_SortOrder( IQueryOptions *iface, IVector_SortEntry **value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->SortOrders;
    return S_OK;
}

static HRESULT WINAPI query_options_get_GroupPropertyName( IQueryOptions *iface, HSTRING *value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    WindowsDuplicateString( impl->GroupPropertyName, value );
    return S_OK;
}

static HRESULT WINAPI query_options_get_DateStackOption( IQueryOptions *iface, DateStackOption *value )
{
    struct query_options *impl = impl_from_IQueryOptions( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = impl->DateStack;
    return S_OK;
}

static HRESULT WINAPI query_options_SaveToString( IQueryOptions *iface, HSTRING *value )
{
    FIXME( "Saving is not supported. iface %p stub!\n", iface );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_options_LoadFromString( IQueryOptions *iface, HSTRING value )
{
    FIXME( "Loading is not supported. iface %p, value %p stub!\n", iface, &value );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_options_SetThumbnailPrefetch( IQueryOptions *iface, ThumbnailMode mode, UINT32 requestedSize, ThumbnailOptions options )
{
    //Unix does not support item thumbnails.
    FIXME( "iface %p, mode %d, requestedSize %u, options %d stub!\n", iface, mode, requestedSize, options );
    return E_NOTIMPL;
}

static HRESULT WINAPI query_options_SetPropertyPrefetch( IQueryOptions *iface, PropertyPrefetchOptions options, IIterable_HSTRING *propertiesToRetrieve )
{
    FIXME( "iface %p, options %d, propertiesToRetrieve %p stub!\n", iface, options, propertiesToRetrieve );
    return E_NOTIMPL;
}

const struct IQueryOptionsVtbl query_options_vtbl =
{
    query_options_QueryInterface,
    query_options_AddRef,
    query_options_Release,
    /* IInspectable methods */
    query_options_GetIids,
    query_options_GetRuntimeClassName,
    query_options_GetTrustLevel,
    /* IQueryOptions methods */
    query_options_get_FileTypeFilter,
    query_options_get_FolderDepth,
    query_options_put_FolderDepth,
    query_options_get_ApplicationSearchFilter,
    query_options_put_ApplicationSearchFilter,
    query_options_get_UserSearchFilter,
    query_options_put_UserSearchFilter,
    query_options_get_Language,
    query_options_put_Language,
    query_options_get_IndexerOption,
    query_options_put_IndexerOption,
    query_options_get_SortOrder,
    query_options_get_GroupPropertyName,
    query_options_get_DateStackOption,
    query_options_SaveToString,
    query_options_LoadFromString,
    query_options_SetThumbnailPrefetch,
    query_options_SetPropertyPrefetch
};

static struct query_options_factory query_options_factory =
{    
    {&factory_vtbl},    
    {&query_options_factory_vtbl},
    1,    
};

IActivationFactory *query_options_activatable_factory = &query_options_factory.IActivationFactory_iface;