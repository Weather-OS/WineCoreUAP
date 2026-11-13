/* 
 * AppX Deployment & Management Service Test
 *
 * Written by Weather.
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

#include <stdarg.h>
#include <stdio.h>

#define COBJMACROS

#include <initguid.h>
#include <appx.h>
#include <winsvc.h>

#include <wine/test.h>

static void test_AppXPackageManager(void)
{
    IClassFactory *factory = NULL;
    IAppXPackageManager *manager = NULL;
    HRESULT hr;

    hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    ok( SUCCEEDED( hr ), "CoInitializeEx failed: %#lx\n", hr );

    hr = CoGetClassObject( &CLSID_AppXPackageManager, CLSCTX_LOCAL_SERVER, NULL, &IID_IClassFactory, (void **)&factory );
    ok( SUCCEEDED( hr ), "CoGetClassObject failed: %#lx\n", hr );

    hr = IClassFactory_CreateInstance( factory, NULL, &IID_IAppXPackageManager, (void **)&manager );
    ok( SUCCEEDED( hr ), "CreateInstance failed: %#lx\n", hr );

    IAppXPackageManager_Release( manager );
    IClassFactory_Release( factory );
    CoUninitialize();
}

static void test_ServiceInstance( void )
{
    BOOL res = FALSE;
    SC_HANDLE scm = NULL;
    SC_HANDLE service = NULL;
    SERVICE_STATUS_PROCESS status;
    DWORD bytesNeeded;

    scm = OpenSCManagerW( NULL, NULL, SC_MANAGER_CONNECT );
    ok( scm != NULL, "OpenSCManagerW failed with %ld\n", GetLastError() );

    service = OpenServiceA( scm, "AppXDeploymentServer", SERVICE_QUERY_STATUS );
    ok( service != NULL, "OpenServiceA failed with %ld\n", GetLastError() );

    res = QueryServiceStatusEx( service, SC_STATUS_PROCESS_INFO, (LPBYTE)&status, sizeof(status), &bytesNeeded );
    ok( res == TRUE, "QueryServiceStatusEx failed with %ld\n", GetLastError() );

    // Service starts on startup.
    // There could be a race condition here regarding SERVICE_START_PENDING,
    // but it's very rare in this snapshot.
    ok( status.dwCurrentState == SERVICE_RUNNING, "Service is not running! Current state %ld\n", status.dwCurrentState );

    CloseServiceHandle( service );
    CloseServiceHandle( scm );
}

START_TEST(appx)
{
    test_ServiceInstance();
    test_AppXPackageManager();
}
