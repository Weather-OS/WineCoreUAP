/* 
 * AppX Deployment & Management Service
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

#define WIN32_LEAN_AND_MEAN
#include <initguid.h>
#include "private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxds);

static SERVICE_STATUS_HANDLE service_handle;
static SERVICE_STATUS status;
static HANDLE stop_event;

static DWORD WINAPI service_handler( DWORD ctrl, DWORD event_type, void *event_data, void *context )
{
    SERVICE_STATUS status;

    status.dwServiceType             = SERVICE_WIN32;
    status.dwControlsAccepted        = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode           = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint              = 0;
    status.dwWaitHint                = 0;

    switch(ctrl)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        TRACE( "shutting down\n" );
        status.dwCurrentState     = SERVICE_STOP_PENDING;
        status.dwControlsAccepted = 0;
        SetServiceStatus( service_handle, &status );
        SetEvent( stop_event );
        return NO_ERROR;
    default:
        FIXME( "got service ctrl %lx\n", ctrl );
        status.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus( service_handle, &status );
        return NO_ERROR;
    }
}

static VOID
UpdateStatus( DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint )
{
    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = dwCurrentState;
    if ( dwCurrentState == SERVICE_START_PENDING )
        status.dwControlsAccepted = 0;
    else
        status.dwControlsAccepted
            = (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE
               | SERVICE_ACCEPT_SHUTDOWN);
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = dwWaitHint;

    if ( !SetServiceStatus( service_handle, &status ) ) 
    {
        ERR( "failed to set service status\n" );
        SetEvent( stop_event );
    }
}

static HRESULT
RegisterCOMEntries( void )
{
    HRESULT hr;
    DWORD dwReg;

    TRACE("\n");
    
    hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    if ( FAILED( hr ) ) return hr;

    hr = CoInitializeSecurity( NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_NONE,
                               RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE,
                               NULL );
    if ( FAILED( hr ) ) return hr;

    hr = CoRegisterClassObject(&CLSID_AppXPackageManager,
                               (IUnknown *) &appx_package_manager_factory,
                               CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &dwReg);
    if ( FAILED( hr ) ) return hr;

    return TRUE;
}

/***********************************************************************
 *		ServiceMain (appxdeploymentserver.@)
 */
VOID WINAPI ServiceMain(DWORD argc, LPWSTR *argv)
{
    HRESULT hr;

    TRACE( "starting service\n" );

    stop_event = CreateEventW( NULL, TRUE, FALSE, NULL );

    service_handle = RegisterServiceCtrlHandlerExW( L"AppXDeploymentServer", service_handler, NULL );
    if (!service_handle)
        return;

    UpdateStatus( SERVICE_START_PENDING, NO_ERROR, 3000 );
    hr = RegisterCOMEntries();
    if ( FAILED( hr ) )
    {
        ERR( "Failed registring COM entries! Status %#lx\n", hr );
        UpdateStatus( SERVICE_STOPPED, HRESULT_CODE( hr ), 0 );
        return;
    }

    UpdateStatus( SERVICE_RUNNING, ERROR_SUCCESS, 0 );

    WaitForSingleObject( stop_event, INFINITE );

    UpdateStatus( SERVICE_STOPPED, NO_ERROR, 0 );

    TRACE( "service stopped\n" );

    CoUninitialize();
}
