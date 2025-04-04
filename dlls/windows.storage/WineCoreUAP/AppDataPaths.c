/* WinRT Windows.Storage.AppDataPaths  Implementation
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

#include "AppInternalPaths.h"

_ENABLE_DEBUGGING_

// App Data Paths

struct app_data_paths_statics
{
    IActivationFactory IActivationFactory_iface;
    IAppDataPathsStatics IAppDataPathsStatics_iface;
    LONG ref;
};

static inline struct app_data_paths_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct app_data_paths_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct app_data_paths_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IAppDataPathsStatics ))
    {
        *out = &impl->IAppDataPathsStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct app_data_paths_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct app_data_paths_statics *impl = impl_from_IActivationFactory( iface );
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

struct app_data_paths
{
    IAppDataPaths IAppDataPaths_iface;
    LONG ref;
};

static inline struct app_data_paths *impl_from_IAppDataPaths( IAppDataPaths *iface )
{
    return CONTAINING_RECORD( iface, struct app_data_paths, IAppDataPaths_iface );
}

static HRESULT WINAPI app_data_paths_QueryInterface( IAppDataPaths *iface, REFIID iid, void **out )
{
    struct app_data_paths *impl = impl_from_IAppDataPaths( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAppDataPaths ))
    {
        *out = &impl->IAppDataPaths_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI app_data_paths_AddRef( IAppDataPaths *iface )
{
    struct app_data_paths *impl = impl_from_IAppDataPaths( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI app_data_paths_Release( IAppDataPaths *iface )
{
    struct app_data_paths *impl = impl_from_IAppDataPaths( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI app_data_paths_GetIids( IAppDataPaths *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI app_data_paths_GetRuntimeClassName( IAppDataPaths *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI app_data_paths_GetTrustLevel( IAppDataPaths *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::AppDataPaths
 * 
 * ISSUES: This assumes AppxManifest.xml is present.
*/

static HRESULT WINAPI app_data_paths_get_Cookies( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "cookies", value);
}

static HRESULT WINAPI app_data_paths_get_Desktop( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "desktop", value);
}

static HRESULT WINAPI app_data_paths_get_Documents( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "documents", value);
}

static HRESULT WINAPI app_data_paths_get_Favorites( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "favorites", value);
}

static HRESULT WINAPI app_data_paths_get_History( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "history", value);
}

static HRESULT WINAPI app_data_paths_get_InternetCache( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "internet_cache", value);
}

static HRESULT WINAPI app_data_paths_get_LocalAppData( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "localappdata", value);
}

static HRESULT WINAPI app_data_paths_get_ProgramData( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "programdata", value);
}

static HRESULT WINAPI app_data_paths_get_RoamingAppData( IAppDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    // Arguments
    if ( !value ) return E_POINTER;
    return app_data_paths_GetKnownFolder(iface, "roamingappdata", value);
}

static const struct IAppDataPathsVtbl app_data_paths_vtbl =
{
    app_data_paths_QueryInterface,
    app_data_paths_AddRef,
    app_data_paths_Release,
    /* IInspectable methods */
    app_data_paths_GetIids,
    app_data_paths_GetRuntimeClassName,
    app_data_paths_GetTrustLevel,
    /* IAppDataPaths methods */
    app_data_paths_get_Cookies,
    app_data_paths_get_Desktop,
    app_data_paths_get_Documents,
    app_data_paths_get_Favorites,
    app_data_paths_get_History,
    app_data_paths_get_InternetCache,
    app_data_paths_get_LocalAppData,
    app_data_paths_get_ProgramData,
    app_data_paths_get_RoamingAppData
};

DEFINE_IINSPECTABLE( app_data_paths_statics, IAppDataPathsStatics, struct app_data_paths_statics, IActivationFactory_iface )

static HRESULT WINAPI app_data_paths_statics_GetForUser( IAppDataPathsStatics *iface, IUser *user, IAppDataPaths **result )
{
    //User is not used
    struct app_data_paths *impl;

    TRACE( "iface %p, value %p\n", iface, result );

    // Arguments
    if ( !result ) return E_POINTER;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IAppDataPaths_iface.lpVtbl = &app_data_paths_vtbl;
    impl->ref = 1;

    *result = &impl->IAppDataPaths_iface;
    TRACE( "created IAppDataPaths %p.\n", *result );
    return S_OK;
}

static HRESULT WINAPI app_data_paths_statics_GetDefault( IAppDataPathsStatics *iface, IAppDataPaths **result )
{
    struct app_data_paths *impl;

    TRACE( "iface %p, value %p\n", iface, result );

    // Arguments
    if ( !result ) return E_POINTER;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IAppDataPaths_iface.lpVtbl = &app_data_paths_vtbl;
    impl->ref = 1;

    *result = &impl->IAppDataPaths_iface;
    TRACE( "created IAppDataPaths %p.\n", *result );
    return S_OK;
}


static const struct IAppDataPathsStaticsVtbl app_data_paths_statics_vtbl =
{
    app_data_paths_statics_QueryInterface,
    app_data_paths_statics_AddRef,
    app_data_paths_statics_Release,
    /* IInspectable methods */
    app_data_paths_statics_GetIids,
    app_data_paths_statics_GetRuntimeClassName,
    app_data_paths_statics_GetTrustLevel,
    /* IAppDataPathsStatics methods */
    app_data_paths_statics_GetForUser,
    app_data_paths_statics_GetDefault
};

static struct app_data_paths_statics app_data_paths_statics =
{
    {&factory_vtbl},
    {&app_data_paths_statics_vtbl},
    1,
};

IActivationFactory *app_data_paths_factory = &app_data_paths_statics.IActivationFactory_iface;
