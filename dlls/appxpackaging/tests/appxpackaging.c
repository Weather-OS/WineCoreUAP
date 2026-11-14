/* 
 * AppX Packaging Test
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
#include <appxpackaging.h>
#include <winsvc.h>
#include <shlwapi.h>

#include <wine/test.h>

#define check_interface( a, b, c, d ) check_interface_( __LINE__, a, b, c, d )
static HRESULT check_interface_( unsigned int line, void *iface, REFIID iid, BOOL supported, BOOL is_broken )
{
    HRESULT hr, expected_hr, broken_hr;
    IUnknown *unknown = iface, *out;

    if (supported)
    {
        expected_hr = S_OK;
        broken_hr = E_NOINTERFACE;
    }
    else
    {
        expected_hr = E_NOINTERFACE;
        broken_hr = S_OK;
    }

    hr = IUnknown_QueryInterface( unknown, iid, (void **)&out );
    ok_(__FILE__, line)( hr == expected_hr || broken( is_broken && hr == broken_hr ),
                         "Got unexpected hr %#lx, expected %#lx.\n", hr, expected_hr );
    if (SUCCEEDED( hr ))
        IUnknown_Release( out );
    return hr;
}

static void load_resource( const WCHAR *name, const WCHAR *type, const WCHAR *filename )
{
    DWORD written;
    HANDLE file;
    HRSRC res;
    void *ptr;

    file = CreateFileW( filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0 );
    ok( file != INVALID_HANDLE_VALUE, "failed to create %s, error %lu\n", debugstr_w(filename), GetLastError() );

    res = FindResourceW( NULL, name, type );
    ok( res != 0, "couldn't find resource\n" );
    ptr = LockResource( LoadResource( GetModuleHandleW( NULL ), res ) );
    WriteFile( file, ptr, SizeofResource( GetModuleHandleW( NULL ), res ), &written, NULL );
    ok( written == SizeofResource( GetModuleHandleW( NULL ), res ), "couldn't write resource\n" );
    CloseHandle( file );
}

static void test_IAppxPackageReader( IAppxPackageReader *reader )
{

}

static void test_AppxFactory( void )
{
    IAppxPackageReader *reader = NULL;
    IAppxFactory *factory = NULL;
    IStream *stream = NULL;
    HRESULT hr = S_OK;

#if defined(__aarch64__) || defined(_M_ARM64) || defined(__arm64ec__) || defined(_M_ARM64EC)
    LPCWSTR appxPackage = L"ARM64.appx";
#elif defined(__x86_64__) || defined(_M_X64)
    LPCWSTR appxPackage = L"x64.appx";
#elif defined(__i386__) || defined(_M_IX86)
    LPCWSTR appxPackage = L"Win32.appx";
#endif

    WCHAR temp[MAX_PATH], path[MAX_PATH + 7];

    GetTempPathW( ARRAY_SIZE(temp), temp );
    GetTempFileNameW( temp, L"winetest-appxpackaging", 0, temp );
    DeleteFileW( temp );
    CreateDirectoryW( temp, NULL );

    swprintf( path, MAX_PATH, L"%s\\%s", temp, appxPackage );
    load_resource( appxPackage, (const WCHAR *)RT_RCDATA, path );

    hr = CoCreateInstance( &CLSID_AppxFactory, NULL, CLSCTX_INPROC_SERVER, &IID_IAppxFactory, (LPVOID *)&factory );
    ok( SUCCEEDED( hr ), "CoCreateInstance failed with %#lx\n", hr );

    hr = SHCreateStreamOnFileEx( path, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, FALSE, NULL, &stream );
    ok( SUCCEEDED( hr ), "SHCreateStreamOnFileEx failed with %#lx\n", hr );

    hr = IAppxFactory_CreatePackageReader( factory, stream, &reader );
    ok( SUCCEEDED( hr ), "IAppxFactory::CreatePackageReader failed with %#lx\n", hr );
}

START_TEST(appxpackaging)
{
    CoInitializeEx( NULL, COINIT_MULTITHREADED );

    test_AppxFactory();
}
