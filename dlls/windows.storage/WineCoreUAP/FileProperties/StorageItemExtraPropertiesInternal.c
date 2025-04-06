/* WinRT Windows.Storage.FileProperties.IStorageItemExtraProperties Implementation
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

#include "StorageItemExtraPropertiesInternal.h"

#include <initguid.h>
#include <propkey.h>

_ENABLE_DEBUGGING_

HRESULT WINAPI storage_item_extra_properties_FetchPropertiesAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    IIterator_HSTRING *propertiesRequested = NULL;
    IMap_HSTRING_IInspectable *propertiesRetrieved = NULL;

    IInspectable *currentValue;
    HRESULT status = S_OK;
    HSTRING currentString;
    boolean strExists;
    boolean replacedKey;

    struct storage_item_extra_properties *extra_properties = impl_from_IStorageItemExtraProperties( (IStorageItemExtraProperties *)invoker );

    DEFINE_HSTRING_MAP_IIDS( IInspectable )

    TRACE( "iface %p param %p.\n", invoker, param );

    status = hstring_map_create( &IInspectable_iids, (void **)&propertiesRetrieved );
    if ( FAILED( status ) ) return status; 

    status = IIterable_HSTRING_First( (IIterable_HSTRING *)param, &propertiesRequested );
    
    if ( SUCCEEDED( status ) )
    {
        IIterator_HSTRING_get_HasCurrent( propertiesRequested, &strExists );
        while( strExists )
        {
            IIterator_HSTRING_get_Current( propertiesRequested, &currentString );
            status = IMap_HSTRING_IInspectable_Lookup( extra_properties->Properties, currentString, &currentValue );
            if ( SUCCEEDED( status ) )
            {
                status = IMap_HSTRING_IInspectable_Insert( propertiesRetrieved, currentString, currentValue, &replacedKey );
                if ( FAILED( status ) ) return status; 
            }
            IIterator_HSTRING_MoveNext( propertiesRequested, &strExists );
            WindowsDeleteString( currentString );
        }
    }

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)propertiesRetrieved;
    } else 
    {
        IMap_HSTRING_IInspectable_Release( propertiesRetrieved );
    }

    return status;
}

HRESULT WINAPI storage_item_extra_properties_SubmitPropertiesAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    DateTime time;
    IStorageFile *file;    
    IPropertyValue *propertyValue;
    ISystemProperties *systemProperties;
    ISystemGPSProperties *systemGPSProperties;
    ISystemMediaProperties *systemMediaProperties;
    ISystemMusicProperties *systemMusicProperties;
    ISystemPhotoProperties *systemPhotoProperties;
    ISystemVideoProperties *systemVideoProperties;
    ISystemImageProperties *systemImageProperties;
    IKeyValuePair_HSTRING_IInspectable *propertyKey;
    IIterator_IKeyValuePair_HSTRING_IInspectable *propertyKeyIterator;
    IIterable_IKeyValuePair_HSTRING_IInspectable *propertyKeyIterable;

    IPropertyStore *propertyStore = NULL;    
    IInspectable *boxedPropertyValue;
    PROPVARIANT itemPropertyValue;
    IShellItem2 *shellItem = NULL;
    HRESULT status = S_OK;
    HSTRING key;
    HSTRING fileContentType;
    HSTRING itemPath;
    HSTRING systemPropertyString;
    HSTRING tempPropertyString;
    HSTRING *tempPropertyStringArray;    
    BOOLEAN isFile;
    BOOLEAN keyExists;
    LPWSTR fileContentTypeStr;
    UINT16 tempPropertyUINT16;
    UINT32 arraySize;
    UINT32 iterator;   

    INT32 comparisonResult;

    // Possible File Content Types.
    boolean isImage = FALSE;
    boolean isMusic = FALSE;
    boolean isDocument = FALSE;
    boolean isVideo = FALSE;
    boolean supportsGeoTagging = FALSE;

    struct storage_item_extra_properties *extra_properties = impl_from_IStorageItemExtraProperties( (IStorageItemExtraProperties *)invoker );

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_SystemProperties, systemProperties, IID_ISystemProperties );

    ISystemProperties_get_GPS( systemProperties, &systemGPSProperties );
    ISystemProperties_get_Media( systemProperties, &systemMediaProperties );
    ISystemProperties_get_Music( systemProperties, &systemMusicProperties );
    ISystemProperties_get_Photo( systemProperties, &systemPhotoProperties );
    ISystemProperties_get_Video( systemProperties, &systemVideoProperties );
    ISystemProperties_get_Image( systemProperties, &systemImageProperties );

    status = IStorageItem_IsOfType( extra_properties->Item, StorageItemTypes_File, &isFile );
    if ( SUCCEEDED( status ) )
    {
        IStorageItem_get_Path( extra_properties->Item, &itemPath );
        status = SHCreateItemFromParsingName( WindowsGetStringRawBuffer( itemPath, NULL ), NULL, &IID_IShellItem2, (void **)&shellItem );
        if ( FAILED( status ) ) goto _CLEANUP;
        status = IShellItem2_GetPropertyStore( shellItem, GPS_READWRITE, &IID_IPropertyStore, (void **)&propertyStore );
        if ( FAILED( status ) ) goto _CLEANUP;

        if( isFile )
        {
            status = IStorageItem_QueryInterface( extra_properties->Item, &IID_IStorageFile, (void **)&file );
            if ( FAILED( status ) ) goto _CLEANUP;
            IStorageFile_get_ContentType( file, &fileContentType );
            fileContentTypeStr = (LPWSTR)malloc( WindowsGetStringLen( fileContentType ) * sizeof(WCHAR) );
            wcscpy( fileContentTypeStr, WindowsGetStringRawBuffer( fileContentType, NULL ) );

            if ( fileContentTypeStr )
            {
                if ( !wcsncmp( fileContentTypeStr, L"image", 5 ) )
                {
                    isImage = TRUE;
                    supportsGeoTagging = TRUE; // Images support GeoTagging.
                } else if ( !wcsncmp( fileContentTypeStr, L"audio", 5 ) )
                {
                    isMusic = TRUE;
                } else if ( !wcsncmp( fileContentTypeStr, L"video", 5 ) )
                {
                    isVideo = TRUE;
                    supportsGeoTagging = TRUE; // Videos support GeoTagging.
                } else if ( !wcsncmp( fileContentTypeStr, L"text", 4 ) || !wcsncmp( fileContentTypeStr, L"application", 11 ) )
                {
                    isDocument = TRUE;
                }
            }
        }

        if ( param )
        {
            //Unsubmitted properties are ignored if we were called from SavePropertiesAsync
            status = IIterable_IKeyValuePair_HSTRING_IInspectable_First( (IIterable_IKeyValuePair_HSTRING_IInspectable *)param, &propertyKeyIterator );
            if ( FAILED( status ) ) goto _CLEANUP;
        } else
        {
            //Unsubmitted properties are submitted if we were called from SavePropertiesAsyncOverloadDefault
            status = IMap_HSTRING_IInspectable_QueryInterface( extra_properties->Properties, &IID_IIterable_IKeyValuePair_HSTRING_IInspectable, (void **)&propertyKeyIterable );
            if ( FAILED( status ) ) goto _CLEANUP;
            status = IIterable_IKeyValuePair_HSTRING_IInspectable_First( propertyKeyIterable, &propertyKeyIterator );
            if ( FAILED( status ) ) goto _CLEANUP;
        }

        IIterator_IKeyValuePair_HSTRING_IInspectable_get_HasCurrent( propertyKeyIterator, &keyExists );
        while ( keyExists )
        {
            PropVariantInit( &itemPropertyValue );

            IIterator_IKeyValuePair_HSTRING_IInspectable_get_Current( propertyKeyIterator, &propertyKey );            
            IIterator_IKeyValuePair_HSTRING_IInspectable_MoveNext( propertyKeyIterator, &keyExists );
            IKeyValuePair_HSTRING_IInspectable_get_Key( propertyKey, &key );
            IKeyValuePair_HSTRING_IInspectable_get_Value( propertyKey, &boxedPropertyValue );

            status = IInspectable_QueryInterface( boxedPropertyValue, &IID_IPropertyValue, (void **)&propertyValue );
            if ( FAILED( status ) ) goto _CLEANUP;

            // The behavior calls for not submitting anything if even one of
            // the provided values is incorrect. For compatibility reasons,
            // we would try and submit every possible value.
            //
            // Values that can be modified by the client are provided by ISystemProperties and
            // ISystemProperties ONLY! Nothing else can be modified and thus is readonly
            // to a winrt context.
            if( isFile )
            {
                // System.GPS
                if ( supportsGeoTagging )
                {
                    ISystemGPSProperties_get_LatitudeDecimal( systemGPSProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_GPS_LatitudeDecimal, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemGPSProperties_get_LongitudeDecimal( systemGPSProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_GPS_LongitudeDecimal, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }
                }

                //System.Media
                if ( isMusic || isVideo )
                {
                    ISystemMediaProperties_get_Duration( systemMediaProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UI8;
                        status = IPropertyValue_GetUInt64( propertyValue, &itemPropertyValue.uhVal.QuadPart );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Media_Duration, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMediaProperties_get_Producer( systemMediaProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Media_Producer, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMediaProperties_get_Publisher( systemMediaProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Media_Publisher, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMediaProperties_get_SubTitle( systemMediaProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( WindowsGetStringLen( tempPropertyString ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Media_SubTitle, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMediaProperties_get_Writer( systemMediaProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Media_Writer, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMediaProperties_get_Year( systemMediaProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Media_Year, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }
                }

                //System.Music
                if ( isMusic )
                {  
                    ISystemMusicProperties_get_AlbumArtist( systemMusicProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Music_AlbumArtist, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMusicProperties_get_AlbumTitle( systemMusicProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Music_AlbumTitle, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMusicProperties_get_Composer( systemMusicProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Music_Composer, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMusicProperties_get_Conductor( systemMusicProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Music_Conductor, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMusicProperties_get_DisplayArtist( systemMusicProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Music_DisplayArtist, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMusicProperties_get_Genre( systemMusicProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Music_Genre, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemMusicProperties_get_TrackNumber( systemMusicProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Music_TrackNumber, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }
                }

                //System.Photo
                if ( isImage )
                {
                    ISystemPhotoProperties_get_CameraManufacturer( systemPhotoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Photo_CameraManufacturer, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemPhotoProperties_get_CameraModel( systemPhotoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Photo_CameraModel, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemPhotoProperties_get_DateTaken( systemPhotoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_DATE;
                        status = IPropertyValue_GetDateTime( propertyValue, &time );
                        itemPropertyValue.date = (DATE)time.UniversalTime;
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Photo_DateTaken, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemPhotoProperties_get_Orientation( systemPhotoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UI2;
                        status = IPropertyValue_GetUInt16( propertyValue, &tempPropertyUINT16 );
                        itemPropertyValue.uiVal = tempPropertyUINT16;
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Photo_Orientation, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemPhotoProperties_get_PeopleNames( systemPhotoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Photo_PeopleNames, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }
                }

                //System.Video
                if ( isVideo )
                {
                    ISystemVideoProperties_get_Director( systemVideoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Video_Director, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemVideoProperties_get_FrameHeight( systemVideoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Video_FrameHeight, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemVideoProperties_get_FrameWidth( systemVideoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Video_FrameWidth, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemVideoProperties_get_Orientation( systemVideoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UI2;
                        status = IPropertyValue_GetUInt16( propertyValue, &tempPropertyUINT16 );
                        itemPropertyValue.uiVal = tempPropertyUINT16;
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Video_Orientation, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemVideoProperties_get_TotalBitrate( systemVideoProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Video_TotalBitrate, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }
                }

                //System.Image
                if ( isImage )
                {
                    ISystemImageProperties_get_HorizontalSize( systemImageProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Image_HorizontalSize, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemImageProperties_get_VerticalSize( systemImageProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_UINT;
                        status = IPropertyValue_GetUInt32( propertyValue, (UINT32 *)&itemPropertyValue.ulVal );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Image_VerticalSize, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }
                }

                //System
                if ( isDocument )
                {
                    ISystemProperties_get_Author( systemProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Author, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemProperties_get_Comment( systemProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Comment, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemProperties_get_ItemNameDisplay( systemProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_LPWSTR;
                        status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                        itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                        wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_ItemNameDisplay, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }

                    ISystemProperties_get_Keywords( systemProperties, &systemPropertyString );
                    WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                    if ( !comparisonResult )
                    {
                        itemPropertyValue.vt = VT_VECTOR | VT_LPWSTR;
                        status = IPropertyValue_GetStringArray( propertyValue, &arraySize, &tempPropertyStringArray );
                        if ( SUCCEEDED( status ) )
                        {
                            itemPropertyValue.calpwstr.cElems = arraySize;
                            itemPropertyValue.calpwstr.pElems = (LPWSTR *)malloc( arraySize * sizeof(LPWSTR) );
                            for ( iterator = 0; iterator < arraySize; iterator++ )
                            {
                                itemPropertyValue.calpwstr.pElems[iterator] = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyStringArray[iterator] ) + 1 ) * sizeof(WCHAR) );
                                wcscpy( itemPropertyValue.calpwstr.pElems[iterator], WindowsGetStringRawBuffer( tempPropertyStringArray[iterator], NULL ) );
                            }
                        } else
                            goto _CLEANUP;
                        status = IPropertyStore_SetValue( propertyStore, &PKEY_Keywords, &itemPropertyValue );
                        if ( FAILED( status ) ) goto _CLEANUP;
                        continue;
                    }
                }

                //Every file has System.Title.
                ISystemProperties_get_Title( systemProperties, &systemPropertyString );
                WindowsCompareStringOrdinal( key, systemPropertyString, &comparisonResult );
                if ( !comparisonResult )
                {
                    itemPropertyValue.vt = VT_LPWSTR;
                    status = IPropertyValue_GetString( propertyValue, &tempPropertyString );
                    itemPropertyValue.pwszVal = (LPWSTR)malloc( ( WindowsGetStringLen( tempPropertyString ) + 1 ) * sizeof(WCHAR) );
                    wcscpy( itemPropertyValue.pwszVal, WindowsGetStringRawBuffer( tempPropertyString, NULL ) );
                    if ( FAILED( status ) ) goto _CLEANUP;
                    status = IPropertyStore_SetValue( propertyStore, &PKEY_Title, &itemPropertyValue );
                    if ( FAILED( status ) ) goto _CLEANUP;
                    continue;
                }
            }

            PropVariantClear( &itemPropertyValue );
            WindowsDeleteString( systemPropertyString );
            WindowsDeleteString( key );
            WindowsDeleteString( tempPropertyString );
            free( tempPropertyStringArray );
            IInspectable_Release( boxedPropertyValue );
        }
        status = IPropertyStore_Commit( propertyStore );
        if ( FAILED( status ) ) goto _CLEANUP;
    }

_CLEANUP:
    free( fileContentTypeStr );
    PropVariantClear( &itemPropertyValue );
    WindowsDeleteString( itemPath );
    WindowsDeleteString( systemPropertyString );
    WindowsDeleteString( key );
    WindowsDeleteString( tempPropertyString );
    ISystemProperties_Release( systemProperties );
    ISystemGPSProperties_Release( systemGPSProperties );
    ISystemMediaProperties_Release( systemMediaProperties );
    ISystemMusicProperties_Release( systemMusicProperties );
    ISystemPhotoProperties_Release( systemPhotoProperties );
    ISystemVideoProperties_Release( systemVideoProperties );
    ISystemImageProperties_Release( systemImageProperties );

    return status;
}