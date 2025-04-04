/* WinRT Windows.Storage.SystemProperties Implementation
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

#include "SystemPropertiesInternal.h"

_ENABLE_DEBUGGING_

// System Properties

static inline struct system_properties *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct system_properties, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{

    struct system_properties *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_ISystemProperties ))
    {
        *out = &impl->ISystemProperties_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct system_properties *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct system_properties *impl = impl_from_IActivationFactory( iface );
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

DEFINE_IINSPECTABLE( system_audio_properties, ISystemAudioProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_audio_properties_get_EncodingBitrate( ISystemAudioProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Audio.EncodingBitrate";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static const struct ISystemAudioPropertiesVtbl system_audio_properties_vtbl =
{
    system_audio_properties_QueryInterface,
    system_audio_properties_AddRef,
    system_audio_properties_Release,
    /* IInspectable methods */
    system_audio_properties_GetIids,
    system_audio_properties_GetRuntimeClassName,
    system_audio_properties_GetTrustLevel,
    /* ISystemAudioProperties methods */
    system_audio_properties_get_EncodingBitrate
};

DEFINE_IINSPECTABLE( system_gps_properties, ISystemGPSProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_gps_properties_get_LatitudeDecimal( ISystemGPSProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.GPS.LatitudeDecimal";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_gps_properties_get_LongitudeDecimal( ISystemGPSProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.GPS.LongitudeDecimal";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static const struct ISystemGPSPropertiesVtbl system_gps_properties_vtbl =
{
    system_gps_properties_QueryInterface,
    system_gps_properties_AddRef,
    system_gps_properties_Release,
    /* IInspectable methods */
    system_gps_properties_GetIids,
    system_gps_properties_GetRuntimeClassName,
    system_gps_properties_GetTrustLevel,
    /* ISystemGPSProperties methods */
    system_gps_properties_get_LatitudeDecimal,
    system_gps_properties_get_LongitudeDecimal
};

DEFINE_IINSPECTABLE( system_media_properties, ISystemMediaProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_media_properties_get_Duration( ISystemMediaProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Media.Duration";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_media_properties_get_Producer( ISystemMediaProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Media.Producer";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_media_properties_get_Publisher( ISystemMediaProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Media.Publisher";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_media_properties_get_SubTitle( ISystemMediaProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Media.SubTitle";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_media_properties_get_Writer( ISystemMediaProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Media.Writer";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_media_properties_get_Year( ISystemMediaProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Media.Year";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static const struct ISystemMediaPropertiesVtbl system_media_properties_vtbl =
{
    system_media_properties_QueryInterface,
    system_media_properties_AddRef,
    system_media_properties_Release,
    /* IInspectable methods */
    system_media_properties_GetIids,
    system_media_properties_GetRuntimeClassName,
    system_media_properties_GetTrustLevel,
    /* ISystemMediaProperties methods */
    system_media_properties_get_Duration,
    system_media_properties_get_Producer,
    system_media_properties_get_Publisher,
    system_media_properties_get_SubTitle,
    system_media_properties_get_Writer,
    system_media_properties_get_Year
};

DEFINE_IINSPECTABLE( system_music_properties, ISystemMusicProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_music_properties_get_AlbumArtist( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.AlbumArtist";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_music_properties_get_AlbumTitle( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.AlbumTitle";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_music_properties_get_Artist( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.Artist";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_music_properties_get_Composer( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.Composer";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_music_properties_get_Conductor( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.Conductor";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_music_properties_get_DisplayArtist( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.DisplayArtist";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_music_properties_get_Genre( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.Genre";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_music_properties_get_TrackNumber( ISystemMusicProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Music.TrackNumber";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static const struct ISystemMusicPropertiesVtbl system_music_properties_vtbl =
{
    system_music_properties_QueryInterface,
    system_music_properties_AddRef,
    system_music_properties_Release,
    /* IInspectable methods */
    system_music_properties_GetIids,
    system_music_properties_GetRuntimeClassName,
    system_music_properties_GetTrustLevel,
    /* ISystemMusicProperties methods */
    system_music_properties_get_AlbumArtist,
    system_music_properties_get_AlbumTitle,
    system_music_properties_get_Artist,
    system_music_properties_get_Composer,
    system_music_properties_get_Conductor,
    system_music_properties_get_DisplayArtist,
    system_music_properties_get_Genre,
    system_music_properties_get_TrackNumber
};

DEFINE_IINSPECTABLE( system_photo_properties, ISystemPhotoProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_photo_properties_get_CameraManufacturer( ISystemPhotoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Photo.CameraManufacturer";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_photo_properties_get_CameraModel( ISystemPhotoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Photo.CameraModel";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_photo_properties_get_DateTaken( ISystemPhotoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Photo.DateTaken";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_photo_properties_get_Orientation( ISystemPhotoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Photo.Orientation";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_photo_properties_get_PeopleNames( ISystemPhotoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Photo.PeopleNames";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static const struct ISystemPhotoPropertiesVtbl system_photo_properties_vtbl =
{
    system_photo_properties_QueryInterface,
    system_photo_properties_AddRef,
    system_photo_properties_Release,
    /* IInspectable methods */
    system_photo_properties_GetIids,
    system_photo_properties_GetRuntimeClassName,
    system_photo_properties_GetTrustLevel,
    /* ISystemPhotoProperties methods */
    system_photo_properties_get_CameraManufacturer,
    system_photo_properties_get_CameraModel,
    system_photo_properties_get_DateTaken,
    system_photo_properties_get_Orientation,
    system_photo_properties_get_PeopleNames
};

DEFINE_IINSPECTABLE( system_video_properties, ISystemVideoProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_video_properties_get_Director( ISystemVideoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Video.Director";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_video_properties_get_FrameHeight( ISystemVideoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Video.FrameHeight";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_video_properties_get_FrameWidth( ISystemVideoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Video.FrameWidth";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_video_properties_get_Orientation( ISystemVideoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Video.Orientation";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_video_properties_get_TotalBitrate( ISystemVideoProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Video.TotalBitrate";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static const struct ISystemVideoPropertiesVtbl system_video_properties_vtbl =
{
    system_video_properties_QueryInterface,
    system_video_properties_AddRef,
    system_video_properties_Release,
    /* IInspectable methods */
    system_video_properties_GetIids,
    system_video_properties_GetRuntimeClassName,
    system_video_properties_GetTrustLevel,
    /* ISystemVideoProperties methods */
    system_video_properties_get_Director,
    system_video_properties_get_FrameHeight,
    system_video_properties_get_FrameWidth,
    system_video_properties_get_Orientation,
    system_video_properties_get_TotalBitrate
};

DEFINE_IINSPECTABLE( system_image_properties, ISystemImageProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_image_properties_get_HorizontalSize( ISystemImageProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Image.HorizontalSize";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_image_properties_get_VerticalSize( ISystemImageProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Image.VerticalSize";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static const struct ISystemImagePropertiesVtbl system_image_properties_vtbl =
{
    system_image_properties_QueryInterface,
    system_image_properties_AddRef,
    system_image_properties_Release,
    /* IInspectable methods */
    system_image_properties_GetIids,
    system_image_properties_GetRuntimeClassName,
    system_image_properties_GetTrustLevel,
    /* ISystemImageProperties methods */
    system_image_properties_get_HorizontalSize,
    system_image_properties_get_VerticalSize
};

DEFINE_IINSPECTABLE( system_properties, ISystemProperties, struct system_properties, IActivationFactory_iface )

static HRESULT WINAPI system_properties_get_Author( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Author";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Comment( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Comment";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_ItemNameDisplay( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.ItemNameDisplay";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Keywords( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Keywords";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Rating( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Rating";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Title( ISystemProperties *iface, HSTRING *value )
{
    LPCWSTR propertyStr = L"System.Title";
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    return WindowsCreateString( propertyStr, wcslen( propertyStr ), value );
}

static HRESULT WINAPI system_properties_get_Audio( ISystemProperties *iface, ISystemAudioProperties **value )
{
    struct system_properties *impl = impl_from_ISystemProperties( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = &impl->ISystemAudioProperties_iface;
    return S_OK;
}

static HRESULT WINAPI system_properties_get_GPS( ISystemProperties *iface, ISystemGPSProperties **value )
{
    struct system_properties *impl = impl_from_ISystemProperties( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = &impl->ISystemGPSProperties_iface;
    return S_OK;
}

static HRESULT WINAPI system_properties_get_Media( ISystemProperties *iface, ISystemMediaProperties **value )
{
    struct system_properties *impl = impl_from_ISystemProperties( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = &impl->ISystemMediaProperties_iface;
    return S_OK;
}

static HRESULT WINAPI system_properties_get_Music( ISystemProperties *iface, ISystemMusicProperties **value )
{
    struct system_properties *impl = impl_from_ISystemProperties( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = &impl->ISystemMusicProperties_iface;
    return S_OK;
}

static HRESULT WINAPI system_properties_get_Photo( ISystemProperties *iface, ISystemPhotoProperties **value )
{
    struct system_properties *impl = impl_from_ISystemProperties( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = &impl->ISystemPhotoProperties_iface;
    return S_OK;
}

static HRESULT WINAPI system_properties_get_Video( ISystemProperties *iface, ISystemVideoProperties **value )
{
    struct system_properties *impl = impl_from_ISystemProperties( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = &impl->ISystemVideoProperties_iface;
    return S_OK;
}

static HRESULT WINAPI system_properties_get_Image( ISystemProperties *iface, ISystemImageProperties **value )
{
    struct system_properties *impl = impl_from_ISystemProperties( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    if ( !value ) return E_POINTER;
    *value = &impl->ISystemImageProperties_iface;
    return S_OK;
}

static const struct ISystemPropertiesVtbl system_properties_vtbl =
{
    system_properties_QueryInterface,
    system_properties_AddRef,
    system_properties_Release,
    /* IInspectable methods */
    system_properties_GetIids,
    system_properties_GetRuntimeClassName,
    system_properties_GetTrustLevel,
    /* ISystemProperties methods */
    system_properties_get_Author,
    system_properties_get_Comment,
    system_properties_get_ItemNameDisplay,
    system_properties_get_Keywords,
    system_properties_get_Rating,
    system_properties_get_Title,
    system_properties_get_Audio,
    system_properties_get_GPS,
    system_properties_get_Media,
    system_properties_get_Music,
    system_properties_get_Photo,
    system_properties_get_Video,
    system_properties_get_Image
};

static struct system_properties system_properties =
{
    {&factory_vtbl},
    {&system_properties_vtbl},
    {&system_audio_properties_vtbl},
    {&system_gps_properties_vtbl},
    {&system_media_properties_vtbl},
    {&system_music_properties_vtbl},
    {&system_photo_properties_vtbl},
    {&system_video_properties_vtbl},
    {&system_image_properties_vtbl},
    1,
};

IActivationFactory *system_properties_factory = &system_properties.IActivationFactory_iface;
