/* WinRT Windows.Storage.ApplicationDataContainer Implementation
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

#include "ApplicationDataContainerInternal.h"

_ENABLE_DEBUGGING_

HRESULT WINAPI application_data_AssignAndTrackContainer( IApplicationData *iface, LPWSTR containerPath, IApplicationDataContainer **container )
{
    /**
     * Method procedure:
     *  We create an `HKEY` for the key associated with the container.
     *  We create an `IObservableMap` packed inside `IPropertySet` to track any changes the client makes.
     *  We track any changes made to this vector and changes the container's `HKEY` accordingly.
     */

    BYTE valueData[MAX_VALUE_SIZE];    
    DWORD maxValueNameLength = MAX_VALUE_NAME;
    DWORD valueDataSize = MAX_VALUE_SIZE;
    DWORD enumIndex = 0;
    DWORD valueType;
    WCHAR hivePath[MAX_PATH];
    WCHAR subKeyName[256];
    WCHAR subValueName[MAX_VALUE_NAME];
    LPWSTR subKeyPath;
    HSTRING subKeyNameStr;
    HSTRING valueStr;
    HRESULT status;
    LSTATUS hiveStatus;
    LSTATUS keyEnumResult = ERROR_SUCCESS;
    DateTime valueDateTime;
    TimeSpan valueTimeSpan;
    Point valuePoint;
    Rect valueRect;
    Size valueSize;
    GUID valueGUID;

    PROPVARIANT valueProp;

    struct application_data *impl = impl_from_IApplicationData( iface );
    struct application_data_container *appdata_container;

    IInspectable *boxedValue = NULL;
    IPropertyValueStatics *propertyValueStatics = NULL;
    IApplicationDataContainer *subContainer = NULL;
    IMap_HSTRING_IInspectable *containerMap = NULL;
    IObservableMap_HSTRING_IInspectable *containerObservableMap = NULL;

    DEFINE_HSTRING_MAP_IIDS( ApplicationDataContainer );

    TRACE( "iface %p, containerPath %s, container %p\n", iface, debugstr_w(containerPath), container );

    if (!(appdata_container = calloc( 1, sizeof(*appdata_container) ))) return E_OUTOFMEMORY;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Foundation_Collections_PropertySet, appdata_container->Values, IID_IPropertySet );
    ACTIVATE_INSTANCE( RuntimeClass_Windows_Foundation_PropertyValue, propertyValueStatics, IID_IPropertyValueStatics );
    status = hstring_map_create( &ApplicationDataContainer_iids, (void **)&appdata_container->Containers );
    if ( FAILED( status ) ) return status;

    wcscpy( hivePath, WindowsGetStringRawBuffer( impl->appDataPath, NULL ) );
    PathAppendW( hivePath, SETTINGS_PATH );

    hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, containerPath, 0, KEY_READ | KEY_WRITE, &appdata_container->containerKey );
    if ( hiveStatus != ERROR_SUCCESS )
    {
        status = HRESULT_FROM_WIN32( hiveStatus );
        goto _CLEANUP;
    } 

    status = WindowsCreateString( wcsrchr( containerPath, L'\\' ), wcslen( wcsrchr( containerPath, L'\\' ) ), &appdata_container->Name );
    if ( FAILED( status ) ) goto _CLEANUP;

    while ( keyEnumResult != ERROR_NO_MORE_ITEMS )
    {
        keyEnumResult = RegEnumKeyW( appdata_container->containerKey, enumIndex, subKeyName, 256 );
        if ( keyEnumResult == ERROR_SUCCESS )
        {
            enumIndex++;
            subKeyPath = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WCHAR) * (wcslen(containerPath) + wcslen(subKeyName) + 2) );
            wcscpy( subKeyPath, containerPath );
            wcscat( subKeyPath, L"\\" );
            wcscat( subKeyPath, subKeyName );

            status = WindowsCreateString( subKeyName, wcslen(subKeyName), &subKeyNameStr );
            if ( FAILED( status ) )
            {
                HeapFree( GetProcessHeap(), 0, subKeyPath );
                goto _CLEANUP;
            }

            status = application_data_AssignAndTrackContainer( iface, subKeyPath, &subContainer );
            if ( FAILED( status ) )
            {
                HeapFree( GetProcessHeap(), 0, subKeyPath );
                goto _CLEANUP;
            }

            status = IMap_HSTRING_ApplicationDataContainer_Insert( appdata_container->Containers, subKeyNameStr, subContainer, NULL );
            if ( FAILED( status ) )
            {
                HeapFree( GetProcessHeap(), 0, subKeyPath );
                goto _CLEANUP;
            }

            HeapFree( GetProcessHeap(), 0, subKeyPath );
            IApplicationDataContainer_Release( subContainer );
        }
    }

    status = IPropertySet_QueryInterface( appdata_container->Values, &IID_IMap_HSTRING_IInspectable, (void **)&containerMap );
    if ( FAILED( status ) ) goto _CLEANUP;

    keyEnumResult = ERROR_SUCCESS;
    enumIndex = 0;

    while ( keyEnumResult != ERROR_NO_MORE_ITEMS )
    {
        maxValueNameLength = MAX_VALUE_NAME;
        valueDataSize = MAX_VALUE_SIZE;
        keyEnumResult = RegEnumValueW( appdata_container->containerKey, enumIndex, subValueName, &maxValueNameLength, NULL, &valueType, valueData, &valueDataSize );
        if ( keyEnumResult == ERROR_SUCCESS )
        {
            // Please take casting well into consideration when serializing data.
            enumIndex++;
            switch ( valueType )
            {
                case REG_UINT8:
                    memcpy( &valueProp.bVal, valueData, sizeof(valueProp.bVal) );
                    status = IPropertyValueStatics_CreateUInt8( propertyValueStatics, valueProp.bVal, &boxedValue );
                    break;
                case REG_INT16:
                    memcpy( &valueProp.iVal, valueData, sizeof(valueProp.iVal) );
                    status = IPropertyValueStatics_CreateInt16( propertyValueStatics, valueProp.iVal, &boxedValue );
                    break;
                case REG_UINT16:
                    memcpy( &valueProp.uiVal, valueData, sizeof(valueProp.uiVal) );
                    status = IPropertyValueStatics_CreateUInt16( propertyValueStatics, valueProp.uiVal, &boxedValue );
                    break;
                case REG_INT32:
                    memcpy( &valueProp.lVal, valueData, sizeof(valueProp.lVal) );
                    status = IPropertyValueStatics_CreateInt32( propertyValueStatics, (INT32)valueProp.lVal, &boxedValue );
                    break;
                case REG_UINT32:
                    memcpy( &valueProp.ulVal, valueData, sizeof(valueProp.ulVal) );
                    status = IPropertyValueStatics_CreateUInt32( propertyValueStatics, (UINT32)valueProp.ulVal, &boxedValue );
                    break;
                case REG_INT64:
                    memcpy( &valueProp.lVal, valueData, sizeof(valueProp.lVal) );
                    status = IPropertyValueStatics_CreateInt64( propertyValueStatics, valueProp.lVal, &boxedValue );
                    break;
                case REG_UINT64:
                    memcpy( &valueProp.ulVal, valueData, sizeof(valueProp.ulVal) );
                    status = IPropertyValueStatics_CreateUInt64( propertyValueStatics, valueProp.ulVal, &boxedValue );
                    break;
                case REG_FLOAT:
                    memcpy( &valueProp.fltVal, valueData, sizeof(valueProp.fltVal) );
                    status = IPropertyValueStatics_CreateSingle( propertyValueStatics, valueProp.fltVal, &boxedValue );
                    break;
                case REG_DOUBLE:
                    memcpy( &valueProp.dblVal, valueData, sizeof(valueProp.dblVal) );
                    status = IPropertyValueStatics_CreateDouble( propertyValueStatics, valueProp.dblVal, &boxedValue );
                    break;
                case REG_BOOL:
                    memcpy( &valueProp.bVal, valueData, sizeof(valueProp.bVal) );
                    status = IPropertyValueStatics_CreateBoolean( propertyValueStatics, valueProp.bVal, &boxedValue );
                    break;
                case REG_STRING:
                    WindowsCreateString( (WCHAR *)valueData, wcslen( (WCHAR *)valueData ), &valueStr );
                    status = IPropertyValueStatics_CreateString( propertyValueStatics, valueStr, &boxedValue );
                    WindowsDeleteString( valueStr );
                    break;
                case REG_COMPOSITE:
                    // Serialized data sets are not supported.
                    break;
                case REG_DATETIME:
                    memcpy( &valueDateTime.UniversalTime, valueData, sizeof(valueDateTime.UniversalTime) );
                    status = IPropertyValueStatics_CreateDateTime( propertyValueStatics, valueDateTime, &boxedValue );
                    break;
                case REG_TIMESPAN:
                    memcpy( &valueTimeSpan.Duration, valueData, sizeof(valueTimeSpan.Duration) );
                    status = IPropertyValueStatics_CreateTimeSpan( propertyValueStatics, valueTimeSpan, &boxedValue );
                    break;
                case REG_GUID:
                    memcpy( &valueGUID, valueData, sizeof(valueGUID) );
                    status = IPropertyValueStatics_CreateGuid( propertyValueStatics, valueGUID, &boxedValue );
                    break;
                case REG_POINT:
                    memcpy( &valuePoint, valueData, sizeof(valuePoint) );
                    status = IPropertyValueStatics_CreatePoint( propertyValueStatics, valuePoint, &boxedValue );
                    break;
                case REG_SIZE:
                    memcpy( &valueSize, valueData, sizeof(valueSize) );
                    status = IPropertyValueStatics_CreateSize( propertyValueStatics, valueSize, &boxedValue );
                    break;
                case REG_RECT:
                    memcpy( &valueRect, valueData, sizeof(valueRect) );
                    status = IPropertyValueStatics_CreateRect( propertyValueStatics, valueRect, &boxedValue );
                    break;
                default:
                    break;
            }
        }
    }

    hiveStatus = RegCloseKey( appdata_container->containerKey );
    if ( hiveStatus != ERROR_SUCCESS )
    {
        status = HRESULT_FROM_WIN32( hiveStatus );
        goto _CLEANUP;
    } 

    // Setup observers
    status = IPropertySet_QueryInterface( appdata_container->Values, &IID_IObservableMap_HSTRING_IInspectable, (void **)&containerObservableMap );
    if ( FAILED( status ) ) goto _CLEANUP;

_CLEANUP:
    if ( FAILED( status ) )
    {
        if ( subContainer ) IApplicationDataContainer_Release( subContainer );
        if ( appdata_container->Containers ) IMap_HSTRING_ApplicationDataContainer_Release( appdata_container->Containers );
        if ( appdata_container->Values ) IPropertySet_Release( appdata_container->Values );
        if ( appdata_container->containerKey ) RegCloseKey( appdata_container->containerKey );
        if ( appdata_container->Name ) WindowsDeleteString( appdata_container->Name );
        if ( appdata_container ) free( appdata_container );
    }
    if ( containerMap ) IMap_HSTRING_IInspectable_Release( containerMap );
    if ( containerObservableMap ) IObservableMap_HSTRING_IInspectable_Release( containerObservableMap );

    return status;
}