/* WinRT Windows.Storage.UserDataPaths Implementation
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

#include "UserInternalPaths.h"

_ENABLE_DEBUGGING_

// User Data Paths

static inline struct user_data_paths_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct user_data_paths_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{

    struct user_data_paths_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IUserDataPathsStatics ))
    {
        *out = &impl->IUserDataPathsStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct user_data_paths_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct user_data_paths_statics *impl = impl_from_IActivationFactory( iface );
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

struct user_data_paths
{
    IUserDataPaths IUserDataPaths_iface;
    LONG ref;
};

DEFINE_IINSPECTABLE( user_data_paths, IUserDataPaths, struct user_data_paths, IUserDataPaths_iface )

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::UserDataPaths
*/

static HRESULT WINAPI user_data_paths_get_CameraRoll( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "cameraroll", value);
}

static HRESULT WINAPI user_data_paths_get_Cookies( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "cookies", value);
}

static HRESULT WINAPI user_data_paths_get_Desktop( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "desktop", value);
}

static HRESULT WINAPI user_data_paths_get_Documents( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "documents", value);
}

static HRESULT WINAPI user_data_paths_get_Downloads( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "downloads", value);
}

static HRESULT WINAPI user_data_paths_get_Favorites( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "favorites", value);
}

static HRESULT WINAPI user_data_paths_get_History( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "history", value);
}

static HRESULT WINAPI user_data_paths_get_InternetCache( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "internetcache", value);
}

static HRESULT WINAPI user_data_paths_get_LocalAppData( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "localappdata", value);
}

static HRESULT WINAPI user_data_paths_get_LocalAppDataLow( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "localappdatalow", value);
}

static HRESULT WINAPI user_data_paths_get_Music( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "music", value);
}

static HRESULT WINAPI user_data_paths_get_Pictures( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "pictures", value);
}

static HRESULT WINAPI user_data_paths_get_Profile( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "profile", value);
}

static HRESULT WINAPI user_data_paths_get_Recent( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "recent", value);
}

static HRESULT WINAPI user_data_paths_get_RoamingAppData( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "roamingappdata", value);
}

static HRESULT WINAPI user_data_paths_get_SavedPictures( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "savedpictures", value);
}

static HRESULT WINAPI user_data_paths_get_Screenshots( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "screenshots", value);
}

static HRESULT WINAPI user_data_paths_get_Templates( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "templates", value);
}

static HRESULT WINAPI user_data_paths_get_Videos( IUserDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return user_data_paths_GetKnownFolder(iface, "videos", value);
}

static const struct IUserDataPathsVtbl user_data_paths_vtbl =
{
    user_data_paths_QueryInterface,
    user_data_paths_AddRef,
    user_data_paths_Release,
    /* IInspectable methods */
    user_data_paths_GetIids,
    user_data_paths_GetRuntimeClassName,
    user_data_paths_GetTrustLevel,
    /* IUserDataPaths methods */
    user_data_paths_get_CameraRoll,
    user_data_paths_get_Cookies,
    user_data_paths_get_Desktop,
    user_data_paths_get_Documents,
    user_data_paths_get_Downloads,
    user_data_paths_get_Favorites,
    user_data_paths_get_History,
    user_data_paths_get_InternetCache,
    user_data_paths_get_LocalAppData,
    user_data_paths_get_LocalAppDataLow,
    user_data_paths_get_Music,
    user_data_paths_get_Pictures,
    user_data_paths_get_Profile,
    user_data_paths_get_Recent,
    user_data_paths_get_RoamingAppData,
    user_data_paths_get_SavedPictures,
    user_data_paths_get_Screenshots,
    user_data_paths_get_Templates,
    user_data_paths_get_Videos
};

DEFINE_IINSPECTABLE( user_data_paths_statics, IUserDataPathsStatics, struct user_data_paths_statics, IActivationFactory_iface )

static HRESULT WINAPI user_data_paths_statics_GetForUser( IUserDataPathsStatics *iface, IUser *user, IUserDataPaths **result )
{
    struct user_data_paths *impl;

    TRACE( "iface %p, value %p\n", iface, result );

    // Arguments
    if (!result) return E_POINTER;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IUserDataPaths_iface.lpVtbl = &user_data_paths_vtbl;
    impl->ref = 1;

    *result = &impl->IUserDataPaths_iface;
    TRACE( "created IUserDataPaths %p.\n", *result );
    return S_OK;
}

static HRESULT WINAPI user_data_paths_statics_GetDefault( IUserDataPathsStatics *iface, IUserDataPaths **result )
{
    struct user_data_paths *impl;

    TRACE( "iface %p, value %p\n", iface, result );

    // Arguments
    if ( !result ) return E_POINTER;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->IUserDataPaths_iface.lpVtbl = &user_data_paths_vtbl;
    impl->ref = 1;

    *result = &impl->IUserDataPaths_iface;
    TRACE( "created IUserDataPaths %p.\n", *result );
    return S_OK;
}


static const struct IUserDataPathsStaticsVtbl user_data_paths_statics_vtbl =
{
    user_data_paths_statics_QueryInterface,
    user_data_paths_statics_AddRef,
    user_data_paths_statics_Release,
    /* IInspectable methods */
    user_data_paths_statics_GetIids,
    user_data_paths_statics_GetRuntimeClassName,
    user_data_paths_statics_GetTrustLevel,
    /* IUserDataPathsStatics methods */
    user_data_paths_statics_GetForUser,
    user_data_paths_statics_GetDefault
};

static struct user_data_paths_statics user_data_paths_statics =
{
    {&factory_vtbl},
    {&user_data_paths_statics_vtbl},
    1,
};

IActivationFactory *user_data_paths_factory = &user_data_paths_statics.IActivationFactory_iface;
