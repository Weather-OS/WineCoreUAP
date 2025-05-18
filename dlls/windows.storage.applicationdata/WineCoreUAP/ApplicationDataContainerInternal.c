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

static HRESULT setSePrivilege( LPCSTR privilege, BOOL enable )
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if ( !OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
        return E_FAIL;

    if ( !LookupPrivilegeValueA( NULL, privilege, &luid ) )
    {
        CloseHandle( hToken );
        return HRESULT_FROM_WIN32( GetLastError() );
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;

    AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(tp), NULL, NULL );

    CloseHandle( hToken );
    return S_OK;
}

static HRESULT WINAPI application_data_container_ContainerChangedEventHandler( IObservableMap_HSTRING_IInspectable *invoker, IUnknown *data, IMapChangedEventArgs_HSTRING *args )
{
    HKEY rootKey;
    HKEY containerKey;
    WCHAR valueName[MAX_PATH];
    DWORD valueNameSize;
    HRESULT status;    
    HSTRING changedKey;
    LSTATUS hiveStatus;
    LSTATUS keyEnumResult = ERROR_SUCCESS;
    IInspectable *boxedProperty = NULL;
    PropertyType changedType;
    CollectionChange change;

    IPropertyValue *changedValue = NULL;
    IMap_HSTRING_IInspectable *invokerMap = NULL;

    Rect valueRect;
    Size valueSize;
    GUID valueGUID;    
    Point valuePoint;
    HSTRING valueStr = NULL;
    DateTime valueDateTime;
    TimeSpan valueTimeSpan;
    PROPVARIANT valueProp;

    /**
     * Parametized Data
     */
    struct event_handler_data *hiveData = (struct event_handler_data *)data;

    TRACE( "invoker %p, data->hivePath %s, data->containerPath %s args %p\n", invoker, debugstr_w(hiveData->hivePath), debugstr_w(hiveData->containerPath), args );

    status = IObservableMap_HSTRING_IInspectable_QueryInterface( invoker, &IID_IMap_HSTRING_IInspectable, (void **)&invokerMap );
    if ( FAILED( status ) ) goto _CLEANUP;

    status = setSePrivilege( SE_RESTORE_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    status = setSePrivilege( SE_BACKUP_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    hiveStatus = RegLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName, hiveData->hivePath );
    if ( hiveStatus != ERROR_SUCCESS )
    {
        status = HRESULT_FROM_WIN32( hiveStatus );
        goto _CLEANUP;
    }

    // Opening the root key for saving
    hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, rootKeyName, 0, KEY_READ | KEY_WRITE, &rootKey );
    if ( hiveStatus != ERROR_SUCCESS )
    {
        status = HRESULT_FROM_WIN32( hiveStatus );
        goto _CLEANUP;
    } 

    hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, hiveData->containerPath, 0, KEY_READ | KEY_WRITE, &containerKey );
    if ( hiveStatus != ERROR_SUCCESS )
    {
        status = HRESULT_FROM_WIN32( hiveStatus );
        goto _CLEANUP;
    } 

    IMapChangedEventArgs_HSTRING_get_CollectionChanged( args, &change );
    IMapChangedEventArgs_HSTRING_get_Key( args, &changedKey );

    PropVariantInit( &valueProp );

    switch ( change )
    {
        case CollectionChange_ItemInserted:
        case CollectionChange_ItemChanged:
            status = IMap_HSTRING_IInspectable_Lookup( invokerMap, changedKey, &boxedProperty );
            if ( FAILED( status ) ) goto _CLEANUP;

            // Composite values are not supported yet. The client is most likely not going to use them anyway.
            status = IInspectable_QueryInterface( boxedProperty, &IID_IPropertyValue, (void **)&changedValue );
            if ( FAILED( status ) ) goto _CLEANUP;

            IPropertyValue_get_Type( changedValue, &changedType );

            switch ( changedType )
            {
                case PropertyType_UInt8:
                    IPropertyValue_GetUInt8( changedValue, &valueProp.bVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_UINT8,
                        (const BYTE*)&valueProp.bVal,
                        1
                    );
                    break;
                case PropertyType_Int16:
                    IPropertyValue_GetInt16( changedValue, &valueProp.iVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_INT16,
                        (const BYTE*)&valueProp.iVal,
                        2
                    );
                    break;
                case PropertyType_UInt16:
                    IPropertyValue_GetUInt16( changedValue, &valueProp.uiVal );
                    hiveStatus = RegSetValueExW(
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_UINT16,
                        (const BYTE*)&valueProp.uiVal,
                        2
                    );
                    break;
                case PropertyType_Int32:
                    IPropertyValue_GetInt32( changedValue, (INT32 *)&valueProp.lVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_INT32,
                        (const BYTE*)&valueProp.lVal,
                        4
                    );
                    break;
                case PropertyType_UInt32:
                    IPropertyValue_GetUInt32( changedValue, (UINT32 *)&valueProp.ulVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_UINT32,
                        (const BYTE*)&valueProp.ulVal,
                        4
                    );
                    break;
                case PropertyType_Int64:
                    IPropertyValue_GetInt64( changedValue, (INT64 *)&valueProp.lVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_INT64,
                        (const BYTE*)&valueProp.lVal,
                        8
                    );
                    break;
                case PropertyType_UInt64:
                    IPropertyValue_GetUInt64( changedValue, (UINT64 *)&valueProp.ulVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_UINT64,
                        (const BYTE*)&valueProp.ulVal,
                        8
                    );
                    break;
                case PropertyType_Single:
                    IPropertyValue_GetSingle( changedValue, &valueProp.fltVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_FLOAT,
                        (const BYTE*)&valueProp.fltVal,
                        4
                    );
                    break;
                case PropertyType_Double:
                    IPropertyValue_GetDouble( changedValue, &valueProp.dblVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_DOUBLE,
                        (const BYTE*)&valueProp.dblVal,
                        8
                    );
                    break;
                case PropertyType_Boolean:
                    IPropertyValue_GetBoolean( changedValue, &valueProp.bVal );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_BOOL,
                        (const BYTE*)&valueProp.bVal,
                        2
                    );
                    break;
                case PropertyType_String:
                    IPropertyValue_GetString( changedValue, &valueStr );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_STRING,
                        (const BYTE *)WindowsGetStringRawBuffer( valueStr, NULL ),
                        (WindowsGetStringLen( valueStr ) + 1u) * sizeof( WCHAR )
                    );
                    break;
                case PropertyType_DateTime:
                    IPropertyValue_GetDateTime( changedValue, &valueDateTime );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_DATETIME,
                        (const BYTE*)&valueDateTime.UniversalTime,
                        8
                    );
                    break;
                case PropertyType_TimeSpan:
                    IPropertyValue_GetTimeSpan( changedValue, &valueTimeSpan );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_TIMESPAN,
                        (const BYTE*)&valueTimeSpan.Duration,
                        8
                    );
                    break;
                case PropertyType_Guid:
                    IPropertyValue_GetGuid( changedValue, &valueGUID );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_GUID,
                        (const BYTE*)&valueGUID,
                        sizeof(valueGUID)
                    );
                    break;
                case PropertyType_Point:
                    IPropertyValue_GetPoint( changedValue, &valuePoint );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_POINT,
                        (const BYTE*)&valuePoint,
                        sizeof(valuePoint)
                    );
                    break;
                case PropertyType_Size:
                    IPropertyValue_GetSize( changedValue, &valueSize );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_SIZE,
                        (const BYTE*)&valueSize,
                        sizeof(valueSize)
                    );
                    break;
                case PropertyType_Rect:
                    IPropertyValue_GetRect( changedValue, &valueRect );
                    hiveStatus = RegSetValueExW( 
                        containerKey, 
                        WindowsGetStringRawBuffer( changedKey, NULL ), 
                        0, 
                        REG_RECT,
                        (const BYTE*)&valueRect,
                        sizeof(valueRect)
                    );
                    break;
                default:
                    status = E_INVALIDARG;
                    goto _CLEANUP;
            }
            if ( hiveStatus != ERROR_SUCCESS )
            {
                status = HRESULT_FROM_WIN32( hiveStatus );
                goto _CLEANUP;
            }
            break;
        
        case CollectionChange_ItemRemoved:
            hiveStatus =  RegDeleteValueW( containerKey, WindowsGetStringRawBuffer( changedKey, NULL ) );
            if ( hiveStatus != ERROR_SUCCESS )
            {
                status = HRESULT_FROM_WIN32( hiveStatus );
                goto _CLEANUP;
            }
            break;
        
        case CollectionChange_Reset:
            while ( true )
            {
                valueNameSize = sizeof(valueName);
                keyEnumResult = RegEnumValueW( containerKey, 0, valueName, &valueNameSize, NULL, NULL, NULL, NULL );
                if ( keyEnumResult == ERROR_NO_MORE_ITEMS ) break;
                RegDeleteValueW( containerKey, valueName );
            }
            break;
    }

    hiveStatus = RegSaveKeyW( rootKey, hiveData->hivePath, NULL );
    if ( hiveStatus != ERROR_SUCCESS ) status = HRESULT_FROM_WIN32( hiveStatus );

_CLEANUP:
    RegCloseKey( containerKey );
    RegCloseKey( rootKey );
    RegUnLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName );
    setSePrivilege( SE_RESTORE_NAME, FALSE );
    setSePrivilege( SE_BACKUP_NAME, FALSE );
    PropVariantClear( &valueProp );
    if ( valueStr ) WindowsDeleteString( valueStr );
    if ( changedKey ) WindowsDeleteString( changedKey );
    if ( changedValue ) IPropertyValue_Release( changedValue );
    if ( boxedProperty ) IInspectable_Release( boxedProperty );
    if ( invokerMap ) IMap_HSTRING_IInspectable_Release( invokerMap );
    return status;
}

HRESULT WINAPI application_data_AssignAndTrackContainer( HSTRING appDataPath, LPCWSTR containerPath, IApplicationDataContainer **container )
{
    /**
     * Method procedure:
     *  We create an `HKEY` for the key associated with the container.
     *  We create an `IObservableMap` packed inside `IPropertySet` to track any changes the client makes.
     *  We track any changes made to this vector and change the container's `HKEY` accordingly.
     */

    HKEY containerKey;
    BYTE *valueData = NULL;    
    DWORD maxValueNameLength = MAX_VALUE_NAME;
    DWORD valueDataSize = MAX_VALUE_SIZE;
    DWORD enumIndex = 0;
    DWORD valueType;
    WCHAR hivePath[MAX_PATH];
    WCHAR subKeyName[256];
    WCHAR subValueName[MAX_VALUE_NAME];
    LPWSTR subKeyPath;
    HSTRING subKeyNameStr;
    HSTRING subValueNameStr;
    HRESULT status;
    LSTATUS hiveStatus;
    LSTATUS keyEnumResult = ERROR_SUCCESS;
    EventRegistrationToken handlerToken;

    Rect valueRect;
    Size valueSize;
    GUID valueGUID;    
    Point valuePoint;
    HSTRING valueStr;
    DateTime valueDateTime;
    TimeSpan valueTimeSpan;
    PROPVARIANT valueProp;

    struct event_handler_data *handlerData;
    struct application_data_container *appdata_container;

    IInspectable *boxedValue = NULL;
    IPropertyValueStatics *propertyValueStatics = NULL;
    IApplicationDataContainer *subContainer = NULL;
    IMap_HSTRING_IInspectable *containerMap = NULL;
    IObservableMap_HSTRING_IInspectable *containerObservableMap = NULL;
    IMapChangedEventHandler_HSTRING_IInspectable *changedEventHandler = NULL;

    DEFINE_HSTRING_MAP_IIDS( ApplicationDataContainer );

    TRACE( "appDataPath %s, containerPath %s, container %p\n", debugstr_hstring(appDataPath), debugstr_w(containerPath), container );

    if (!(appdata_container = calloc( 1, sizeof(*appdata_container) ))) return E_OUTOFMEMORY;
    if (!(handlerData = calloc( 1, sizeof(*handlerData) ))) return E_OUTOFMEMORY;
 
    ACTIVATE_INSTANCE( RuntimeClass_Windows_Foundation_Collections_PropertySet, appdata_container->Values, IID_IPropertySet );
    ACTIVATE_INSTANCE( RuntimeClass_Windows_Foundation_PropertyValue, propertyValueStatics, IID_IPropertyValueStatics );

    appdata_container->IApplicationDataContainer_iface.lpVtbl = &application_data_container_vtbl;

    status = hstring_map_create( &ApplicationDataContainer_iids, (void **)&appdata_container->Containers );
    if ( FAILED( status ) ) return status;

    status = setSePrivilege( SE_RESTORE_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    status = setSePrivilege( SE_BACKUP_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    wcscpy( hivePath, WindowsGetStringRawBuffer( appDataPath, NULL ) );
    PathAppendW( hivePath, SETTINGS_PATH );
    PathAppendW( hivePath, SETTINGS_DATA_PATH );

    hiveStatus = RegLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName, hivePath );
    if ( hiveStatus != ERROR_SUCCESS )
    {
        status = HRESULT_FROM_WIN32( hiveStatus );
        goto _CLEANUP;
    } 

    hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, containerPath, 0, KEY_READ | KEY_WRITE, &containerKey );
    if ( hiveStatus != ERROR_SUCCESS )
    {
        status = HRESULT_FROM_WIN32( hiveStatus );
        goto _CLEANUP;
    }

    status = WindowsCreateString( wcsrchr( containerPath, L'\\' ) + 1, wcslen( wcsrchr( containerPath, L'\\' ) + 1 ), &appdata_container->Name );
    if ( FAILED( status ) ) goto _CLEANUP;

    while ( keyEnumResult != ERROR_NO_MORE_ITEMS )
    {
        keyEnumResult = RegEnumKeyW( containerKey, enumIndex, subKeyName, 256 );
        if ( keyEnumResult == ERROR_SUCCESS )
        {
            enumIndex++;
            subKeyPath = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WCHAR) * ( wcslen( containerPath ) + wcslen( subKeyName ) + 2 ) );
            wcscpy( subKeyPath, containerPath );
            wcscat( subKeyPath, L"\\" );
            wcscat( subKeyPath, subKeyName );

            status = WindowsCreateString( subKeyName, wcslen( subKeyName ), &subKeyNameStr );            
            if ( FAILED( status ) )
            {
                HeapFree( GetProcessHeap(), 0, subKeyPath );
                goto _CLEANUP;
            }

            RegCloseKey( containerKey );
            RegUnLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName );
            setSePrivilege( SE_RESTORE_NAME, FALSE );
            setSePrivilege( SE_BACKUP_NAME, FALSE );

            status = application_data_AssignAndTrackContainer( appDataPath, subKeyPath, &subContainer );
            if ( FAILED( status ) )
            {
                HeapFree( GetProcessHeap(), 0, subKeyPath );
                goto _CLEANUP;
            }

            status = setSePrivilege( SE_RESTORE_NAME, TRUE );
            if ( FAILED( status ) ) return status;

            status = setSePrivilege( SE_BACKUP_NAME, TRUE );
            if ( FAILED( status ) ) return status;

            hiveStatus = RegLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName, hivePath );
            if ( hiveStatus != ERROR_SUCCESS )
            {
                status = HRESULT_FROM_WIN32( hiveStatus );
                goto _CLEANUP;
            } 
        
            hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, containerPath, 0, KEY_READ | KEY_WRITE, &containerKey );
            if ( hiveStatus != ERROR_SUCCESS )
            {
                status = HRESULT_FROM_WIN32( hiveStatus );
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

    PropVariantInit( &valueProp );

    if (!(valueData = malloc( MAX_VALUE_SIZE * sizeof(*valueData) )))
    {
        status = E_OUTOFMEMORY;
        goto _CLEANUP;
    }

    while ( keyEnumResult != ERROR_NO_MORE_ITEMS )
    {
        maxValueNameLength = MAX_VALUE_NAME;
        valueDataSize = MAX_VALUE_SIZE;
        keyEnumResult = RegEnumValueW( containerKey, enumIndex, subValueName, &maxValueNameLength, NULL, &valueType, valueData, &valueDataSize );
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
            if ( FAILED( status ) ) goto _CLEANUP;

            WindowsCreateString( subValueName, wcslen( subValueName ), &subValueNameStr );
            status = IMap_HSTRING_IInspectable_Insert( containerMap, subValueNameStr, boxedValue, NULL );
            if ( FAILED( status ) ) goto _CLEANUP;
            WindowsDeleteString( subValueNameStr );
        }
    }

    // Setup observers
    status = IPropertySet_QueryInterface( appdata_container->Values, &IID_IObservableMap_HSTRING_IInspectable, (void **)&containerObservableMap );
    if ( FAILED( status ) ) goto _CLEANUP;

    wcscpy( handlerData->hivePath, hivePath );
    wcscpy( handlerData->containerPath, containerPath );
    wcscpy( appdata_container->hivePath, hivePath );
    wcscpy( appdata_container->containerPath, containerPath );

    WindowsDuplicateString( appDataPath, &appdata_container->appDataPath );

    status = hstring_map_event_handler_create( application_data_container_ContainerChangedEventHandler, (IUnknown *)handlerData, &changedEventHandler );
    if ( FAILED( status ) ) goto _CLEANUP;

    // In theory, not removing the event handler shouldn't be an issue.
    status = IObservableMap_HSTRING_IInspectable_add_MapChanged( containerObservableMap, changedEventHandler, &handlerToken );

_CLEANUP:
    free(valueData);
    PropVariantClear( &valueProp );
    RegCloseKey( containerKey );
    RegUnLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName );
    setSePrivilege( SE_RESTORE_NAME, FALSE );
    setSePrivilege( SE_BACKUP_NAME, FALSE );
    if ( FAILED( status ) )
    {
        if ( subContainer ) IApplicationDataContainer_Release( subContainer );
        if ( appdata_container->Containers ) IMap_HSTRING_ApplicationDataContainer_Release( appdata_container->Containers );
        if ( appdata_container->Values ) IPropertySet_Release( appdata_container->Values );
        if ( changedEventHandler ) IMapChangedEventHandler_HSTRING_IInspectable_Release( changedEventHandler );
        WindowsDeleteString( appdata_container->Name );
        if ( appdata_container ) free( appdata_container );
        if ( handlerData ) free( handlerData );
    }
    if ( SUCCEEDED( status ) )
    {
        *container = &appdata_container->IApplicationDataContainer_iface;
    }
    if ( containerMap ) IMap_HSTRING_IInspectable_Release( containerMap );
    if ( containerObservableMap ) IObservableMap_HSTRING_IInspectable_Release( containerObservableMap );

    return status;
}

HRESULT WINAPI application_data_container_CreateAndTrackContainer( IApplicationDataContainer *iface, HSTRING name, ApplicationDataCreateDisposition composition, IApplicationDataContainer **outContainer )
{
    HKEY rootKey = NULL;
    HKEY tmpKey = NULL;
    HKEY containerKey = NULL;
    WCHAR newContainerPath[MAX_PATH];
    HRESULT status = S_OK;
    LSTATUS hiveStatus;
    BOOLEAN keyFound = FALSE;

    struct application_data_container *appdata_container = impl_from_IApplicationDataContainer( iface );

    TRACE( "iface %p, name %s, composition %d, outContainer %p\n", iface, debugstr_hstring(name), composition, outContainer );

    status = setSePrivilege( SE_RESTORE_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    status = setSePrivilege( SE_BACKUP_NAME, TRUE );
    if ( FAILED( status ) ) return status;

    // Check if the sub container exists or not, if so, return that.
    IMap_HSTRING_ApplicationDataContainer_HasKey( appdata_container->Containers, name, &keyFound );
    if ( !keyFound )
    {
        switch ( composition )
        {
            case ApplicationDataCreateDisposition_Always:
                hiveStatus = RegLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName, appdata_container->hivePath );
                if ( hiveStatus != ERROR_SUCCESS )
                {
                    status = HRESULT_FROM_WIN32( hiveStatus );
                    goto _CLEANUP;
                }
            
                // Opening the root key for saving
                hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, rootKeyName, 0, KEY_READ | KEY_WRITE, &rootKey );
                if ( hiveStatus != ERROR_SUCCESS )
                {
                    status = HRESULT_FROM_WIN32( hiveStatus );
                    goto _CLEANUP;
                }
            
                hiveStatus = RegOpenKeyExW( HKEY_LOCAL_MACHINE, appdata_container->containerPath, 0, KEY_READ | KEY_WRITE, &containerKey );
                if ( hiveStatus != ERROR_SUCCESS )
                {
                    status = HRESULT_FROM_WIN32( hiveStatus );
                    goto _CLEANUP;
                }

                hiveStatus = RegCreateKeyW( containerKey, WindowsGetStringRawBuffer( name, NULL ), &tmpKey );
                if ( hiveStatus != ERROR_SUCCESS )
                {
                    status = HRESULT_FROM_WIN32( hiveStatus );
                    goto _CLEANUP;
                }

                wcscpy( newContainerPath, appdata_container->containerPath );
                PathAppendW( newContainerPath, WindowsGetStringRawBuffer( name, NULL ) );

                hiveStatus = RegSaveKeyW( rootKey, appdata_container->hivePath, NULL );
                if ( hiveStatus != ERROR_SUCCESS )
                {
                    status = HRESULT_FROM_WIN32( hiveStatus );
                    goto _CLEANUP;
                }

                break;
            case ApplicationDataCreateDisposition_Existing:
                return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
        }
        if ( tmpKey ) RegCloseKey( tmpKey );
        if ( containerKey ) RegCloseKey( containerKey );
        if ( rootKey ) RegCloseKey( rootKey );
        RegUnLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName );
        setSePrivilege( SE_RESTORE_NAME, FALSE );
        setSePrivilege( SE_BACKUP_NAME, FALSE );
        status = application_data_AssignAndTrackContainer( appdata_container->appDataPath, newContainerPath, outContainer );
    } else
    {
        status = IMap_HSTRING_ApplicationDataContainer_Lookup( appdata_container->Containers, name, outContainer );
    }

_CLEANUP:
    if ( tmpKey ) RegCloseKey( tmpKey );
    if ( containerKey ) RegCloseKey( containerKey );
    if ( rootKey ) RegCloseKey( rootKey );
    RegUnLoadKeyW( HKEY_LOCAL_MACHINE, rootKeyName );
    setSePrivilege( SE_RESTORE_NAME, FALSE );
    setSePrivilege( SE_BACKUP_NAME, FALSE );

    return status;
}