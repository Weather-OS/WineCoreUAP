/* WinRT Windows.Storage.FileProperties.MusicProperties Implementation
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

#include "MusicPropertiesInternal.h"

_ENABLE_DEBUGGING_

// Music Properties

static struct music_properties *impl_from_IMusicProperties( IMusicProperties *iface )
{
    return CONTAINING_RECORD( iface, struct music_properties, IMusicProperties_iface );
}

static HRESULT WINAPI music_properties_QueryInterface( IMusicProperties *iface, REFIID iid, void **out )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IMusicProperties ))
    {
        *out = &impl->IMusicProperties_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI music_properties_AddRef( IMusicProperties *iface )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI music_properties_Release( IMusicProperties *iface )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI music_properties_GetIids( IMusicProperties *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI music_properties_GetRuntimeClassName( IMusicProperties *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI music_properties_GetTrustLevel( IMusicProperties *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI music_properties_get_Album( IMusicProperties *iface, HSTRING *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( impl->Album, value );
}

static HRESULT WINAPI music_properties_put_Album( IMusicProperties *iface, HSTRING value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( value, &impl->Album );
}

static HRESULT WINAPI music_properties_get_Artist( IMusicProperties *iface, HSTRING *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( impl->Album, value );
}

static HRESULT WINAPI music_properties_put_Artist( IMusicProperties *iface, HSTRING value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( value, &impl->Album );
}

static HRESULT WINAPI music_properties_get_Genre( IMusicProperties *iface, IVector_HSTRING **value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Genre;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_TrackNumber( IMusicProperties *iface, UINT32 *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->TrackNumber;
    return S_OK;
}

static HRESULT WINAPI music_properties_put_TrackNumber( IMusicProperties *iface, UINT32 value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    impl->TrackNumber = value;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_Title( IMusicProperties *iface, HSTRING *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( impl->Title, value );
}

static HRESULT WINAPI music_properties_put_Title( IMusicProperties *iface, HSTRING value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( value, &impl->Title );
}

static HRESULT WINAPI music_properties_get_Rating( IMusicProperties *iface, UINT32 *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Rating;
    return S_OK;
}

static HRESULT WINAPI music_properties_put_Rating( IMusicProperties *iface, UINT32 value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    impl->Rating = value;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_Duration( IMusicProperties *iface, UINT32 *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Duration;
    return S_OK;
}
 
static HRESULT WINAPI music_properties_get_Bitrate( IMusicProperties *iface, UINT32 *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Bitrate;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_AlbumArtist( IMusicProperties *iface, HSTRING *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( impl->AlbumArtist, value );
}

static HRESULT WINAPI music_properties_put_AlbumArtist( IMusicProperties *iface, HSTRING value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( value, &impl->AlbumArtist );
}

static HRESULT WINAPI music_properties_get_Composers( IMusicProperties *iface, IVector_HSTRING **value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Composers;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_Conductors( IMusicProperties *iface, IVector_HSTRING **value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Conductors;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_Subtitle( IMusicProperties *iface, HSTRING *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( impl->Subtitle, value );
}

static HRESULT WINAPI music_properties_put_Subtitle( IMusicProperties *iface, HSTRING value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( value, &impl->Subtitle );
}

static HRESULT WINAPI music_properties_get_Producers( IMusicProperties *iface, IVector_HSTRING **value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Producers;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_Publisher( IMusicProperties *iface, HSTRING *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( impl->Publisher, value );
}

static HRESULT WINAPI music_properties_put_Publisher( IMusicProperties *iface, HSTRING value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( value, &impl->Publisher );
}

static HRESULT WINAPI music_properties_get_Writers( IMusicProperties *iface, IVector_HSTRING **value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Writers;
    return S_OK;
}

static HRESULT WINAPI music_properties_get_Year( IMusicProperties *iface, UINT32 *value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    *value = impl->Year;
    return S_OK;
}

static HRESULT WINAPI music_properties_put_Year( IMusicProperties *iface, UINT32 value )
{
    struct music_properties *impl = impl_from_IMusicProperties( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    impl->Year = value;
    return S_OK;
}
const struct IMusicPropertiesVtbl music_properties_vtbl =
{
    music_properties_QueryInterface,
    music_properties_AddRef,
    music_properties_Release,
    /* IInspectable methods */
    music_properties_GetIids,
    music_properties_GetRuntimeClassName,
    music_properties_GetTrustLevel,
    /* IMusicProperties methods */
    music_properties_get_Album,
    music_properties_put_Album,
    music_properties_get_Artist,
    music_properties_put_Artist,
    music_properties_get_Genre,
    music_properties_get_TrackNumber,
    music_properties_put_TrackNumber,
    music_properties_get_Title,
    music_properties_put_Title,
    music_properties_get_Rating,
    music_properties_put_Rating,
    music_properties_get_Duration,
    music_properties_get_Bitrate,
    music_properties_get_AlbumArtist,
    music_properties_put_AlbumArtist,
    music_properties_get_Composers,
    music_properties_get_Conductors,
    music_properties_get_Subtitle,
    music_properties_put_Subtitle,
    music_properties_get_Producers,
    music_properties_get_Publisher,
    music_properties_put_Publisher,
    music_properties_get_Writers,
    music_properties_get_Year,
    music_properties_put_Year
};