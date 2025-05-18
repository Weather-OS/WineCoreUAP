/*
 * Copyright (C) 2023 Mohamad Al-Jaf
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
#define COBJMACROS
#include "initguid.h"
#include <stdarg.h>

#include "windef.h"
#include "winbase.h"
#include "winstring.h"

#include "roapi.h"

#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#include "windows.foundation.h"
#define WIDL_using_Windows_Storage
#include "windows.storage.h"

#include "wine/test.h"

#define ACTIVATE_INSTANCE( instance_name, instance_object, instance_iid )                                                   \
    {                                                                                                                       \
        HSTRING _str;                                                                                                       \
        HRESULT _hr;                                                                                                        \
        _hr = WindowsCreateString( instance_name, wcslen( instance_name ), &_str );                                         \
        ok( _hr == S_OK, "got hr %#lx.\n", _hr );                                                                           \
                                                                                                                            \
        _hr = RoGetActivationFactory( _str, &instance_iid, (void **)&instance_object );                                     \
        WindowsDeleteString( _str );                                                                                        \
        ok( _hr == S_OK || broken( _hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", _hr );                                   \
        if (_hr == REGDB_E_CLASSNOTREG)                                                                                     \
        {                                                                                                                   \
            win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( instance_name ) );                \
        }                                                                                                                   \
        check_interface( instance_object, &IID_IUnknown );                                                                  \
        check_interface( instance_object, &IID_IInspectable );                                                              \
                                                                                                                            \
        ok( _hr == S_OK, "got hr %#lx.\n", _hr );                                                                           \
    }

WINE_DEFAULT_DEBUG_CHANNEL(data);

struct application_data_set_version_handler
{
    IApplicationDataSetVersionHandler IApplicationDataSetVersionHandler_iface;
    LONG ref;
};

static inline struct application_data_set_version_handler *impl_from_IApplicationDataSetVersionHandler( IApplicationDataSetVersionHandler *iface )
{
    return CONTAINING_RECORD( iface, struct application_data_set_version_handler, IApplicationDataSetVersionHandler_iface );
}

static HRESULT WINAPI application_data_set_version_handler_QueryInterface( IApplicationDataSetVersionHandler *iface, REFIID iid, void **out )
{
    struct application_data_set_version_handler *impl = impl_from_IApplicationDataSetVersionHandler( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IApplicationDataSetVersionHandler ))
    {
        *out = &impl->IApplicationDataSetVersionHandler_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI application_data_set_version_handler_AddRef( IApplicationDataSetVersionHandler *iface )
{
    struct application_data_set_version_handler *impl = impl_from_IApplicationDataSetVersionHandler( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI application_data_set_version_handler_Release( IApplicationDataSetVersionHandler *iface )
{
    struct application_data_set_version_handler *impl = impl_from_IApplicationDataSetVersionHandler( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI application_data_set_version_handler_Invoke( IApplicationDataSetVersionHandler *iface, ISetVersionRequest *request )
{
    HRESULT hr;

    UINT32 currentVersion;
    UINT32 desiredVersion;

    hr = ISetVersionRequest_get_CurrentVersion( request, &currentVersion );
    if (FAILED(hr)) return hr;
    hr = ISetVersionRequest_get_DesiredVersion( request, &desiredVersion );
    if (FAILED(hr)) return hr;

    if ( currentVersion < desiredVersion )
    {
        printf( "Upgrading from version %u to %u\n", currentVersion, desiredVersion );
        //Application logic would go here.
    }
    else if ( currentVersion > desiredVersion )
    {
        printf( "Downgrading from version %u to %u\n", currentVersion, desiredVersion );
        //Application logic would go here.
    }

    return S_OK;
}

static const struct IApplicationDataSetVersionHandlerVtbl application_data_set_version_handler_vtbl = {
    /*** IUnknown methods ***/
    application_data_set_version_handler_QueryInterface,
    application_data_set_version_handler_AddRef,
    application_data_set_version_handler_Release,
    /*** IApplicationDataSetVersionHandlerVtbl methods ***/
    application_data_set_version_handler_Invoke
};

static struct application_data_set_version_handler default_application_data_set_version_handler = {{&application_data_set_version_handler_vtbl}};

/**
 * IAsyncActionCompletedHandler
 */

struct async_action_handler
{
    IAsyncActionCompletedHandler IAsyncActionCompletedHandler_iface;
    IAsyncAction *async;
    AsyncStatus status;
    BOOL invoked;
    HANDLE event;
};

static inline struct async_action_handler *impl_from_IAsyncActionCompletedHandler( IAsyncActionCompletedHandler *iface )
{
    return CONTAINING_RECORD( iface, struct async_action_handler, IAsyncActionCompletedHandler_iface );
}

static HRESULT WINAPI async_action_handler_QueryInterface( IAsyncActionCompletedHandler *iface, REFIID iid, void **out )
{
    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IAsyncActionCompletedHandler ))
    {
        IUnknown_AddRef( iface );
        *out = iface;
        return S_OK;
    }

    trace( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI async_action_handler_AddRef( IAsyncActionCompletedHandler *iface )
{
    return 2;
}

static ULONG WINAPI async_action_handler_Release( IAsyncActionCompletedHandler *iface )
{
    return 1;
}

static HRESULT WINAPI async_action_handler_Invoke( IAsyncActionCompletedHandler *iface,
                                                 IAsyncAction *async, AsyncStatus status )
{
    struct async_action_handler *impl = impl_from_IAsyncActionCompletedHandler( iface );

    trace( "iface %p, async %p, status %u\n", iface, async, status );

    ok( !impl->invoked, "invoked twice\n" );
    impl->invoked = TRUE;
    impl->async = async;
    impl->status = status;
    if (impl->event) SetEvent( impl->event );

    return S_OK;
}

static IAsyncActionCompletedHandlerVtbl async_action_handler_vtbl =
{
    /*** IUnknown methods ***/
    async_action_handler_QueryInterface,
    async_action_handler_AddRef,
    async_action_handler_Release,
    /*** IAsyncActionCompletedHandler methods ***/
    async_action_handler_Invoke,
};

static struct async_action_handler default_async_action_handler = {{&async_action_handler_vtbl}};

#define check_interface( obj, iid ) check_interface_( __LINE__, obj, iid )
static void check_interface_( unsigned int line, void *obj, const IID *iid )
{
    IUnknown *iface = obj;
    IUnknown *unk;
    HRESULT hr;

    hr = IUnknown_QueryInterface( iface, iid, (void **)&unk );
    ok_(__FILE__, line)( hr == S_OK, "got hr %#lx.\n", hr );
    IUnknown_Release( unk );
}

static void test_ApplicationDataStatics(void)
{
    static const WCHAR *application_data_statics_name = L"Windows.Storage.ApplicationData";
    IApplicationDataContainer *container;
    IApplicationDataContainer *subContainer;
    IApplicationDataStatics *application_data_statics;
    IApplicationData *application_data = NULL;
    IAsyncAction *asyncAction;
    IPropertySet *values;
    IPropertyValueStatics *propertyValueStatics;
    IPropertyValue *boxedPropValue;
    PropertyType propertyType;
    IInspectable *boxedValue;
    IAsyncActionCompletedHandler *asyncActionCompletedHandler;
    IMap_HSTRING_IInspectable *valueMap;
    IMapView_HSTRING_ApplicationDataContainer *containerMapView;
    IActivationFactory *factory;
    HSTRING str;
    BOOLEAN replaced;
    HRESULT hr;
    UINT32 currentVersion;
    INT32 propertyValue;
    INT64 propertyValue2;
    DWORD ret;
    LONG ref;

    struct async_action_handler async_action_handler;

    hr = WindowsCreateString( application_data_statics_name, wcslen( application_data_statics_name ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = RoGetActivationFactory( str, &IID_IActivationFactory, (void **)&factory );
    WindowsDeleteString( str );
    ok( hr == S_OK || broken( hr == REGDB_E_CLASSNOTREG ), "got hr %#lx.\n", hr );
    if (hr == REGDB_E_CLASSNOTREG)
    {
        win_skip( "%s runtimeclass not registered, skipping tests.\n", wine_dbgstr_w( application_data_statics_name ) );
        return;
    }

    ACTIVATE_INSTANCE(RuntimeClass_Windows_Foundation_PropertyValue, propertyValueStatics, IID_IPropertyValueStatics);

    check_interface( factory, &IID_IUnknown );
    check_interface( factory, &IID_IInspectable );
    check_interface( factory, &IID_IAgileObject );

    hr = IActivationFactory_QueryInterface( factory, &IID_IApplicationDataStatics, (void **)&application_data_statics );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IApplicationDataStatics_get_Current( application_data_statics, NULL );
    ok( hr == E_INVALIDARG, "got hr %#lx.\n", hr );
    hr = IApplicationDataStatics_get_Current( application_data_statics, &application_data );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    /**
     * IApplicationData_SetVersionAsync
    */
    IApplicationData_get_Version( application_data, &currentVersion );
    hr = IApplicationData_SetVersionAsync( application_data, currentVersion + 1, &default_application_data_set_version_handler.IApplicationDataSetVersionHandler_iface, &asyncAction );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    check_interface( asyncAction, &IID_IUnknown );
    check_interface( asyncAction, &IID_IInspectable );
    check_interface( asyncAction, &IID_IAgileObject );
    check_interface( asyncAction, &IID_IAsyncInfo );
    check_interface( asyncAction, &IID_IAsyncAction );

    hr = IAsyncAction_get_Completed( asyncAction, &asyncActionCompletedHandler );
    ok( hr == S_OK, "get_Completed returned %#lx\n", hr );
    ok( asyncActionCompletedHandler == NULL, "got handler %p\n", asyncActionCompletedHandler );

    async_action_handler = default_async_action_handler;
    async_action_handler.event = CreateEventW( NULL, FALSE, FALSE, NULL );

    hr = IAsyncAction_put_Completed( asyncAction, &async_action_handler.IAsyncActionCompletedHandler_iface );
    ok( hr == S_OK, "put_Completed returned %#lx\n", hr );

    ret = WaitForSingleObject( async_action_handler.event, 1000 );
    ok( !ret, "WaitForSingleObject returned %#lx\n", ret );

    ret = CloseHandle( async_action_handler.event );
    ok( ret, "CloseHandle failed, error %lu\n", GetLastError() );
    ok( async_action_handler.invoked, "handler not invoked\n" );
    ok( async_action_handler.async == asyncAction, "got async %p\n", async_action_handler.async );
    ok( async_action_handler.status == Completed || broken( async_action_handler.status == Error ), "got status %u\n", async_action_handler.status );

    ref = IApplicationDataStatics_Release( application_data_statics );
    ok( ref == 2, "got ref %ld.\n", ref );
    ref = IActivationFactory_Release( factory );
    ok( ref == 1, "got ref %ld.\n", ref );

    /**
     * IApplicationData_LocalSettings
    */
    hr = IApplicationData_get_LocalSettings( application_data, &container );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    WindowsCreateString( L"subContainer", wcslen( L"subContainer" ), &str );
    hr = IApplicationDataContainer_CreateContainer( container, str, ApplicationDataCreateDisposition_Always, &subContainer );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IApplicationDataContainer_get_Values( container, &values );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IPropertySet_QueryInterface( values, &IID_IMap_HSTRING_IInspectable, (void **)&valueMap );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IPropertyValueStatics_CreateInt32( propertyValueStatics, 1337, &boxedValue );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( L"testvalue", wcslen( L"testvalue" ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IMap_HSTRING_IInspectable_Insert( valueMap, str, boxedValue, &replaced );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IApplicationDataContainer_get_Values( subContainer, &values );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IPropertySet_QueryInterface( values, &IID_IMap_HSTRING_IInspectable, (void **)&valueMap );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IPropertyValueStatics_CreateInt64( propertyValueStatics, 1234, &boxedValue );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( L"testvalue2", wcslen( L"testvalue2" ), &str );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IMap_HSTRING_IInspectable_Insert( valueMap, str, boxedValue, &replaced );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    // Re-fetch that value for verification
    hr = IApplicationData_get_LocalSettings( application_data, &container );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IApplicationDataContainer_get_Values( container, &values );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IPropertySet_QueryInterface( values, &IID_IMap_HSTRING_IInspectable, (void **)&valueMap );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = WindowsCreateString( L"testvalue", wcslen( L"testvalue" ), &str );
    hr = IMap_HSTRING_IInspectable_Lookup( valueMap, str, &boxedValue );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IInspectable_QueryInterface( boxedValue, &IID_IPropertyValue, (void **)&boxedPropValue );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IPropertyValue_get_Type( boxedPropValue, &propertyType );
    ok( propertyType == PropertyType_Int32, "unexpected property type %d\n", propertyType );

    IPropertyValue_GetInt32( boxedPropValue, &propertyValue );
    ok ( propertyValue == 1337, "unexpected property value %d\n", propertyValue );

    hr = IApplicationDataContainer_get_Containers( container, &containerMapView );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    WindowsCreateString( L"subContainer", wcslen( L"subContainer" ), &str );
    hr = IMapView_HSTRING_ApplicationDataContainer_Lookup( containerMapView, str, &subContainer );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IApplicationDataContainer_get_Values( subContainer, &values );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IPropertySet_QueryInterface( values, &IID_IMap_HSTRING_IInspectable, (void **)&valueMap );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    WindowsCreateString( L"testvalue2", wcslen( L"testvalue2" ), &str );
    hr = IMap_HSTRING_IInspectable_Lookup( valueMap, str, &boxedValue );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    hr = IInspectable_QueryInterface( boxedValue, &IID_IPropertyValue, (void **)&boxedPropValue );
    ok( hr == S_OK, "got hr %#lx.\n", hr );

    IPropertyValue_get_Type( boxedPropValue, &propertyType );
    ok( propertyType == PropertyType_Int64, "unexpected property type %d\n", propertyType );

    IPropertyValue_GetInt64( boxedPropValue, &propertyValue2 );
    ok ( propertyValue2 == 1234, "unexpected property value %lld\n", propertyValue2 );

}

START_TEST(data)
{
    HRESULT hr;

    hr = RoInitialize( RO_INIT_MULTITHREADED );
    ok( hr == S_OK, "RoInitialize failed, hr %#lx\n", hr );

    test_ApplicationDataStatics();

    RoUninitialize();
}
