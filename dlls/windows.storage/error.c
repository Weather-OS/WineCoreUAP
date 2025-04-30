/* This module is mainly used for IErrorInfo and IRestrictedErrorInfo
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

#include "private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(ole);

//There are no functions that implement IRestrictedErrorInfo for us. We need to implement it ourselves.
struct restricted_error_info
{
    IRestrictedErrorInfo IRestrictedErrorInfo_iface;
    HRESULT hresult_error;
    LPWSTR description;
    BSTR restricted_description;
    BSTR capability_sid;
    BSTR reference;
    LONG refcount;
};

static struct restricted_error_info *impl_from_IRestrictedErrorInfo_iface( IRestrictedErrorInfo *iface )
{
    return CONTAINING_RECORD( iface, struct restricted_error_info, IRestrictedErrorInfo_iface ); 
}

static HRESULT WINAPI restricted_errorinfo_QueryInterface( IRestrictedErrorInfo *iface, REFIID riid, void **obj )
{
    struct restricted_error_info *restricted_error_info = impl_from_IRestrictedErrorInfo_iface( iface );
    
    TRACE( "%p, %s, %p.\n", iface, debugstr_guid( riid ), obj );

    *obj = NULL;

    if ( IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IRestrictedErrorInfo) )
    {
        *obj = &restricted_error_info->IRestrictedErrorInfo_iface;
    }

    if ( *obj )
    {
        IUnknown_AddRef( (IUnknown *)*obj );
        return S_OK;
    }

    WARN( "Unsupported interface %s.\n", debugstr_guid( riid ) );
    return E_NOINTERFACE;
}

static ULONG WINAPI restricted_errorinfo_AddRef( IRestrictedErrorInfo *iface )
{
    struct restricted_error_info *restricted_error_info = impl_from_IRestrictedErrorInfo_iface( iface );
    ULONG refcount = InterlockedIncrement( &restricted_error_info->refcount );

    TRACE( "%p, refcount %lu.\n", iface, refcount );

    return refcount;
}

static ULONG WINAPI restricted_errorinfo_Release( IRestrictedErrorInfo *iface )
{
    struct restricted_error_info *restricted_error_info = impl_from_IRestrictedErrorInfo_iface( iface );
    ULONG refcount = InterlockedDecrement( &restricted_error_info->refcount );

    TRACE("%p, refcount %lu.\n", iface, refcount);

    if ( !refcount )
    {
        free( restricted_error_info->description );
        free( restricted_error_info->restricted_description );
        free( restricted_error_info->capability_sid );
        free( restricted_error_info->reference );
        free( restricted_error_info );
    }

    return refcount;
}

static HRESULT WINAPI restricted_errorinfo_GetErrorDetails(IRestrictedErrorInfo *iface, BSTR *description, HRESULT *error, BSTR *restrictedDescription, BSTR *capabilitySid)
{
    struct restricted_error_info *restricted_error_info = impl_from_IRestrictedErrorInfo_iface(iface);
    if( description ) *description = SysAllocString(restricted_error_info->description);
    if( restrictedDescription ) *restrictedDescription = SysAllocString(restricted_error_info->restricted_description);
    if( capabilitySid ) *capabilitySid = SysAllocString(restricted_error_info->capability_sid);
    if( error ) *error = restricted_error_info->hresult_error;
    return S_OK;
}

static HRESULT WINAPI restricted_errorinfo_GetReference(IRestrictedErrorInfo *iface, BSTR *reference)
{
    struct restricted_error_info *restricted_error_info = impl_from_IRestrictedErrorInfo_iface(iface);
    *reference = SysAllocString(restricted_error_info->reference);
    return S_OK;
}

static const IRestrictedErrorInfoVtbl restricted_errorinfo_vtbl =
{
    restricted_errorinfo_QueryInterface,
    restricted_errorinfo_AddRef,
    restricted_errorinfo_Release,
    restricted_errorinfo_GetErrorDetails,
    restricted_errorinfo_GetReference
};

HRESULT SetLastRestrictedErrorWithMessageW( HRESULT status, LPCWSTR message )
{
    struct restricted_error_info *error_info;

    if (!(error_info = calloc( 1, sizeof(*error_info) ))) return E_OUTOFMEMORY;

    error_info->IRestrictedErrorInfo_iface.lpVtbl = &restricted_errorinfo_vtbl;
    error_info->hresult_error = status;
    error_info->description = SysAllocString( message );
    error_info->refcount = 1;
    return SetRestrictedErrorInfo( &error_info->IRestrictedErrorInfo_iface );
}

HRESULT SetLastRestrictedErrorWithMessageFormattedW( HRESULT status, LPCWSTR format, ... )
{
    HRESULT hr;
    LPWSTR message;
    va_list args, args_for_length;
    size_t messageLength;

    va_start( args, format );

    va_copy( args_for_length, args );
    messageLength = _vscwprintf( format, args_for_length );
    va_end( args_for_length );

    message = (LPWSTR)malloc( ( messageLength + 1 ) * sizeof(WCHAR) );
    if ( !message ) return E_OUTOFMEMORY;
    vswprintf( message, messageLength + 1, format, args );

    va_end( args );

    hr = SetLastRestrictedErrorWithMessageW( status, message );
    free( message );
    return hr;
}

LPCWSTR GetResourceW( INT resourceId )
{
    static WCHAR resourceBuffer[1024];
    LoadStringW( GetModuleHandleW( L"windows.storage.dll" ), resourceId, resourceBuffer, sizeof(resourceBuffer) );
    return resourceBuffer;
}

LPCSTR GetResourceA( INT resourceId )
{
    static CHAR resourceBuffer[1024];
    LoadStringA( GetModuleHandleW( L"windows.storage.dll" ), resourceId, resourceBuffer, sizeof(resourceBuffer) );
    return resourceBuffer;
}