/* WinRT Windows.Storage.SystemDataPaths Implementation
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

#include "SystemInternalPaths.h"

_ENABLE_DEBUGGING_

// System Data Paths

static inline struct system_data_paths_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct system_data_paths_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{

    struct system_data_paths_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_ISystemDataPathsStatics ))
    {
        *out = &impl->ISystemDataPathsStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct system_data_paths_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct system_data_paths_statics *impl = impl_from_IActivationFactory( iface );
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

struct system_data_paths
{
    ISystemDataPaths ISystemDataPaths_iface;
    LONG ref;
};

DEFINE_IINSPECTABLE( system_data_paths, ISystemDataPaths, struct system_data_paths, ISystemDataPaths_iface )

/**
 * COM Oriented, WinRT Implementation: winrt::Windows::Storage::SystemDataPaths
*/

static HRESULT WINAPI system_data_paths_get_Fonts( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "fonts", value);
}

static HRESULT WINAPI system_data_paths_get_ProgramData( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "programdata", value);
}

static HRESULT WINAPI system_data_paths_get_Public( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "public", value);
}

static HRESULT WINAPI system_data_paths_get_PublicDesktop( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "publicdesktop", value);
}

static HRESULT WINAPI system_data_paths_get_PublicDocuments( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "publicdocuments", value);
}

static HRESULT WINAPI system_data_paths_get_PublicDownloads( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "publicdownloads", value);
}

static HRESULT WINAPI system_data_paths_get_PublicMusic( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "publicmusic", value);
}

static HRESULT WINAPI system_data_paths_get_PublicPictures( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "publicpictures", value);
}

static HRESULT WINAPI system_data_paths_get_PublicVideos( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "publicvideos", value);
}

static HRESULT WINAPI system_data_paths_get_System( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "system", value);
}

static HRESULT WINAPI system_data_paths_get_SystemHost( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "systemhost", value);
}

static HRESULT WINAPI system_data_paths_get_SystemX86( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "systemx86", value);
}

static HRESULT WINAPI system_data_paths_get_SystemX64( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "systemx64", value);
}

static HRESULT WINAPI system_data_paths_get_SystemARM( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "systemarm", value);
}

static HRESULT WINAPI system_data_paths_get_UserProfiles( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "userprofiles", value);
}

static HRESULT WINAPI system_data_paths_get_Windows( ISystemDataPaths *iface, HSTRING *value )
{
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return system_data_paths_GetKnownFolder(iface, "windows", value);
}

static const struct ISystemDataPathsVtbl system_data_paths_vtbl =
{
    system_data_paths_QueryInterface,
    system_data_paths_AddRef,
    system_data_paths_Release,
    /* IInspectable methods */
    system_data_paths_GetIids,
    system_data_paths_GetRuntimeClassName,
    system_data_paths_GetTrustLevel,
    /* ISystemDataPaths methods */
    system_data_paths_get_Fonts,
    system_data_paths_get_ProgramData,
    system_data_paths_get_Public,
    system_data_paths_get_PublicDesktop,
    system_data_paths_get_PublicDocuments,
    system_data_paths_get_PublicDownloads,
    system_data_paths_get_PublicMusic,
    system_data_paths_get_PublicPictures,
    system_data_paths_get_PublicVideos,
    system_data_paths_get_System,
    system_data_paths_get_SystemHost,
    system_data_paths_get_SystemX86,
    system_data_paths_get_SystemX64,
    system_data_paths_get_SystemARM,
    system_data_paths_get_UserProfiles,
    system_data_paths_get_Windows,
};

DEFINE_IINSPECTABLE( system_data_paths_statics, ISystemDataPathsStatics, struct system_data_paths_statics, IActivationFactory_iface )

static HRESULT WINAPI system_data_paths_statics_GetDefault( ISystemDataPathsStatics *iface, ISystemDataPaths **result )
{
    struct system_data_paths *impl;

    TRACE( "iface %p, value %p\n", iface, result );

    // Arguments
    if ( !result ) return E_POINTER;

    if (!(impl = calloc( 1, sizeof(*impl) ))) return E_OUTOFMEMORY;

    impl->ISystemDataPaths_iface.lpVtbl = &system_data_paths_vtbl;
    impl->ref = 1;

    *result = &impl->ISystemDataPaths_iface;
    TRACE( "created ISystemDataPaths %p.\n", *result );
    return S_OK;
}


static const struct ISystemDataPathsStaticsVtbl system_data_paths_statics_vtbl =
{
    system_data_paths_statics_QueryInterface,
    system_data_paths_statics_AddRef,
    system_data_paths_statics_Release,
    /* IInspectable methods */
    system_data_paths_statics_GetIids,
    system_data_paths_statics_GetRuntimeClassName,
    system_data_paths_statics_GetTrustLevel,
    /* ISystemDataPathsStatics methods */
    system_data_paths_statics_GetDefault
};

static struct system_data_paths_statics system_data_paths_statics =
{
    {&factory_vtbl},
    {&system_data_paths_statics_vtbl},
    1,
};

IActivationFactory *system_data_paths_factory = &system_data_paths_statics.IActivationFactory_iface;
