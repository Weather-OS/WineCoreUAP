/* WinRT Windows.Storage.Streams.DataReader Implementation
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

#include "DataReaderInternal.h"
#include "BufferInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( async_buffer_uint_handler, IAsyncOperationWithProgressCompletedHandler_IBuffer_UINT32, IAsyncOperationWithProgress_IBuffer_UINT32 )

// Data Reader

static struct data_reader_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct data_reader_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct data_reader_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IDataReaderFactory))
    {
        *out = &impl->IDataReaderFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IDataReaderStatics ))
    {
        *out = &impl->IDataReaderStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct data_reader_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct data_reader_statics *impl = impl_from_IActivationFactory( iface );
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

struct data_reader *impl_from_IDataReader( IDataReader *iface )
{
    return CONTAINING_RECORD( iface, struct data_reader, IDataReader_iface );
}

static HRESULT WINAPI data_reader_QueryInterface( IDataReader *iface, REFIID iid, void **out )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IDataReader ))
    {
        *out = &impl->IDataReader_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI data_reader_AddRef( IDataReader *iface )
{
    struct data_reader *impl = impl_from_IDataReader( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI data_reader_Release( IDataReader *iface )
{
    struct data_reader *impl = impl_from_IDataReader( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI data_reader_GetIids( IDataReader *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_reader_GetRuntimeClassName( IDataReader *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_reader_GetTrustLevel( IDataReader *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_reader_get_UnconsumedBufferLength( IDataReader *iface, UINT32* value )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    *value = impl->UnconsumedBufferLength;
    return S_OK;
}

static HRESULT WINAPI data_reader_get_UnicodeEncoding( IDataReader *iface, UnicodeEncoding* value )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    *value = impl->Encoding;
    return S_OK;
}

static HRESULT WINAPI data_reader_put_UnicodeEncoding( IDataReader *iface, UnicodeEncoding value )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %d\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    impl->Encoding = value;
    return S_OK;
}

static HRESULT WINAPI data_reader_get_ByteOrder( IDataReader *iface, ByteOrder* value )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    *value = impl->Order;
    return S_OK;
}

static HRESULT WINAPI data_reader_put_ByteOrder( IDataReader *iface, ByteOrder value )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %d\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    impl->Order = value;
    return S_OK;
}

static HRESULT WINAPI data_reader_get_InputStreamOptions( IDataReader *iface, InputStreamOptions* value )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    *value = impl->StreamOptions;
    return S_OK;
}

static HRESULT WINAPI data_reader_put_InputStreamOptions( IDataReader *iface, InputStreamOptions value )
{
    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %d\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    impl->StreamOptions = value;
    return S_OK;
}

static HRESULT WINAPI data_reader_ReadByte( IDataReader *iface, BYTE* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );    
    
    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if ( impl->UnconsumedBufferLength < 1 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) ) 
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        *value = returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ];
        impl->UnconsumedBufferLength--;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadBytes( IDataReader *iface, UINT32 __valueSize, BYTE* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );  

    // Arguments
    if ( !value ) return E_POINTER;
    
    TRACE( "iface %p, __valueSize %d, value %p\n", iface, __valueSize, value );

    if ( impl->UnconsumedBufferLength < __valueSize )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) ) 
    {    
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], __valueSize );
        impl->UnconsumedBufferLength -= __valueSize;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadBuffer( IDataReader *iface, UINT32 length, IBuffer** buffer )
{
    BYTE *returnedBufferBytes;
    BYTE *bufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );   
    
    // Arguments
    if ( !buffer ) return E_POINTER;
    
    TRACE( "iface %p, length %d, buffer %p\n", iface, length, buffer );

    if ( impl->UnconsumedBufferLength < length )
        return E_BOUNDS;

    buffer_Create( length, buffer );

    IBuffer_get_Length( impl->buffer, &readAhead );

    IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );
    hr = IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
    if ( FAILED(hr) ) return hr;
    IBufferByteAccess_Release( returnedBufferByteAccess );

    hr = IBuffer_QueryInterface( *buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &bufferBytes );
        memcpy( bufferBytes, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], length );
        impl->UnconsumedBufferLength -= length;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadBoolean( IDataReader *iface, boolean* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );   
    
    // Arguments
    if ( !value ) return E_POINTER;
    
    TRACE( "iface %p, value %p\n", iface, value );

    if ( impl->UnconsumedBufferLength < 1 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) ) 
    {    
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        if ( !returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ] )
        {
            *value = FALSE;
        } else {
            *value = TRUE;
        }
        impl->UnconsumedBufferLength--;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadGuid( IDataReader *iface, GUID *value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );    
    
    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if ( impl->UnconsumedBufferLength < 16 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &value->Data1, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 4 );
        memcpy( &value->Data2, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength + 4 ], 2 );
        memcpy( &value->Data3, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength + 6 ], 2 );
        memcpy( &value->Data4, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength + 8 ], 8 );
        impl->UnconsumedBufferLength -= 16;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadInt16( IDataReader *iface, INT16* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 2 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 2 );
        impl->UnconsumedBufferLength -= 2;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadInt32( IDataReader *iface, INT32* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );    

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 4 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 4 );
        impl->UnconsumedBufferLength -= 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadInt64( IDataReader *iface, INT64* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 8 );
        impl->UnconsumedBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadUInt16( IDataReader *iface, UINT16* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );    
    
    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;

    if ( impl->UnconsumedBufferLength < 2 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 2 );
        impl->UnconsumedBufferLength -= 2;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadUInt32( IDataReader *iface, UINT32* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );    

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 4 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 4 );
        impl->UnconsumedBufferLength -= 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadUInt64( IDataReader *iface, UINT64* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );    

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 8 );
        impl->UnconsumedBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadSingle( IDataReader *iface, FLOAT* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 4 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 4 );
        impl->UnconsumedBufferLength -= 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadDouble( IDataReader *iface, DOUBLE* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( value, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 8 );
        impl->UnconsumedBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadString( IDataReader *iface, UINT32 codeUnitCount, HSTRING* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    LPWSTR string;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );  

    TRACE( "iface %p, codeUnitCount %d, value %p\n", iface, codeUnitCount, value );

    // Arguments
    if ( !value ) return E_POINTER;
    if ( !codeUnitCount ) return E_INVALIDARG;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );
    
    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        switch ( impl->Encoding )
        {
            case UnicodeEncoding_Utf8:
                if ( impl->UnconsumedBufferLength < codeUnitCount )
                    return E_BOUNDS;
                break;

            default:
                if ( impl->UnconsumedBufferLength < codeUnitCount * 2 )
                    return E_BOUNDS;
                break;
        }

        string = (LPWSTR)malloc( codeUnitCount * sizeof( WCHAR ) );
        MultiByteToWideChar( CP_UTF8, 0, (LPSTR)&returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], -1, string, codeUnitCount );

        hr = WindowsCreateString( string, codeUnitCount, value );

        impl->UnconsumedBufferLength -= codeUnitCount;
        IBufferByteAccess_Release( returnedBufferByteAccess );
        free( string );
    }

    return hr;
}

static HRESULT WINAPI data_reader_ReadDateTime( IDataReader *iface, DateTime* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );    

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &value->UniversalTime, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 8 );
        impl->UnconsumedBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }
    

    return hr;
}

static HRESULT WINAPI data_reader_ReadTimeSpan( IDataReader *iface, TimeSpan* value )
{
    BYTE *returnedBufferBytes;
    UINT32 readAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_reader *impl = impl_from_IDataReader( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    // Arguments
    if ( !value ) return E_POINTER;
    
    if ( impl->UnconsumedBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &readAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &value->Duration, &returnedBufferBytes[ readAhead - impl->UnconsumedBufferLength ], 8 );
        impl->UnconsumedBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_reader_Load( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    DWORD res;
    HRESULT hr;

    IBuffer *buffer;
    IBuffer *returnedBuffer;
    IAsyncOperationWithProgress_IBuffer_UINT32 *operation;

    struct load_arguments *arguments = (struct load_arguments *)param;
    struct data_reader *impl = impl_from_IDataReader( (IDataReader *)invoker );
    
    TRACE( "iface %p, param %p, value %p\n", invoker, param, result );

    // Arguments
    if ( !result ) return E_POINTER;

    hr = buffer_Create( arguments->count, &buffer );
    if ( FAILED( hr ) ) return hr;

    hr = IInputStream_ReadAsync( impl->inputStream, buffer, arguments->count, impl->StreamOptions, &operation );
    if ( FAILED(hr) ) return hr;

    res = await_IAsyncOperationWithProgress_IBuffer_UINT32( operation, INFINITE );
    if ( res ) return E_ABORT;

    hr = IAsyncOperationWithProgress_IBuffer_UINT32_GetResults( operation, &returnedBuffer );
    if ( FAILED(hr) ) return hr;

    IAsyncOperationWithProgress_IBuffer_UINT32_Release( operation );

    CHECK_LAST_RESTRICTED_ERROR();

    impl->buffer = returnedBuffer;
    hr = IBuffer_get_Length( returnedBuffer, &impl->UnconsumedBufferLength );

    if ( SUCCEEDED( hr ) )
    {
        result->vt = VT_UI4;
        result->ulVal = (UINT64)impl->UnconsumedBufferLength;
    }

    return hr;
}

static HRESULT WINAPI data_reader_LoadAsync( IDataReader *iface, UINT32 count, IAsyncOperation_UINT32 **operation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_UINT32 };
    struct load_arguments *arguments;

    TRACE( "iface %p, count %d, operation %p\n", iface, count, operation );

    // Arguments
    if ( !operation ) return E_POINTER;

    if (!(arguments = calloc( 1, sizeof(*arguments) ))) return E_OUTOFMEMORY;

    arguments->count = count;

    hr = async_operation_uint32_create( (IUnknown *)iface, (IUnknown *)arguments, data_reader_Load, iids, operation );
    return hr;
}

static HRESULT WINAPI data_reader_DetachBuffer( IDataReader *iface, IBuffer **buffer )
{
    struct data_reader *impl = impl_from_IDataReader( iface );
    TRACE( "iface %p, buffer %p\n", iface, buffer );

    if ( !buffer ) return E_POINTER;

    *buffer = impl->buffer;
    impl->buffer = NULL;

    return S_OK;
}

static HRESULT WINAPI data_reader_DetachStream( IDataReader *iface, IInputStream **stream )
{
    struct data_reader *impl = impl_from_IDataReader( iface );
    TRACE( "iface %p, stream %p\n", iface, stream );

    if (!stream) return E_POINTER;

    *stream = impl->inputStream;
    impl->inputStream = NULL;

    return S_OK;
}


struct IDataReaderVtbl data_reader_vtbl =
{
    data_reader_QueryInterface,
    data_reader_AddRef,
    data_reader_Release,
    /* IInspectable methods */
    data_reader_GetIids,
    data_reader_GetRuntimeClassName,
    data_reader_GetTrustLevel,
    /* IDataReader methods */
    data_reader_get_UnconsumedBufferLength,
    data_reader_get_UnicodeEncoding,
    data_reader_put_UnicodeEncoding,
    data_reader_get_ByteOrder,
    data_reader_put_ByteOrder,
    data_reader_get_InputStreamOptions,
    data_reader_put_InputStreamOptions,
    data_reader_ReadByte,
    data_reader_ReadBytes,
    data_reader_ReadBuffer,
    data_reader_ReadBoolean,
    data_reader_ReadGuid,
    data_reader_ReadInt16,
    data_reader_ReadInt32,
    data_reader_ReadInt64,
    data_reader_ReadUInt16,
    data_reader_ReadUInt32,
    data_reader_ReadUInt64,
    data_reader_ReadSingle,
    data_reader_ReadDouble,
    data_reader_ReadString,
    data_reader_ReadDateTime,
    data_reader_ReadTimeSpan,
    data_reader_LoadAsync,
    data_reader_DetachBuffer,
    data_reader_DetachStream
};

DEFINE_IINSPECTABLE( data_reader_statics, IDataReaderStatics, struct data_reader_statics, IActivationFactory_iface )

static HRESULT WINAPI data_reader_statics_FromBuffer( IDataReaderStatics *iface, IBuffer* buffer, IDataReader **dataReader )
{
    HRESULT hr;
    struct data_reader *reader;
    TRACE( "iface %p, buffer %p, dataReader %p\n", iface, buffer, dataReader );

    if (!(reader = calloc( 1, sizeof(*reader) ))) return E_OUTOFMEMORY;

    reader->IDataReader_iface.lpVtbl = &data_reader_vtbl;
    reader->Encoding = UnicodeEncoding_Utf8;
    reader->Order = ByteOrder_LittleEndian;
    reader->StreamOptions = InputStreamOptions_None;
    reader->buffer = buffer;
    hr = IBuffer_get_Length( buffer, &reader->UnconsumedBufferLength );
    reader->ref = 1;

    *dataReader = &reader->IDataReader_iface;

    return hr;
}

const struct IDataReaderStaticsVtbl data_reader_statics_vtbl =
{
    data_reader_statics_QueryInterface,
    data_reader_statics_AddRef,
    data_reader_statics_Release,
    /* IInspectable methods */
    data_reader_statics_GetIids,
    data_reader_statics_GetRuntimeClassName,
    data_reader_statics_GetTrustLevel,
    /* IDataReaderStatics methods */
    data_reader_statics_FromBuffer
};

struct data_reader_statics *impl_from_IDataReaderFactory( IDataReaderFactory *iface )
{
    return CONTAINING_RECORD( iface, struct data_reader_statics, IDataReaderFactory_iface );
}

static HRESULT WINAPI data_reader_factory_QueryInterface( IDataReaderFactory *iface, REFIID iid, void **out )
{
    struct data_reader_statics *impl = impl_from_IDataReaderFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IDataReaderFactory))
    {
        *out = &impl->IDataReaderFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IDataReaderStatics ))
    {
        *out = &impl->IDataReaderStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI data_reader_factory_AddRef( IDataReaderFactory *iface )
{
    struct data_reader_statics *impl = impl_from_IDataReaderFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI data_reader_factory_Release( IDataReaderFactory *iface )
{
    struct data_reader_statics *impl = impl_from_IDataReaderFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI data_reader_factory_GetIids( IDataReaderFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_reader_factory_GetRuntimeClassName( IDataReaderFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_reader_factory_GetTrustLevel( IDataReaderFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_reader_factory_CreateDataReader( IDataReaderFactory *iface, IInputStream *inputStream, IDataReader **dataReader )
{
    struct data_reader *reader;

    TRACE( "iface %p, inputStream %p, dataReader %p\n", iface, inputStream, dataReader );

    if (!(reader = calloc( 1, sizeof(*reader) ))) return E_OUTOFMEMORY;

    reader->IDataReader_iface.lpVtbl = &data_reader_vtbl;
    reader->Encoding = UnicodeEncoding_Utf8;
    reader->Order = ByteOrder_LittleEndian;
    reader->StreamOptions = InputStreamOptions_None;
    reader->inputStream = inputStream;
    reader->ref = 1;

    *dataReader = &reader->IDataReader_iface;

    return S_OK;
}


const struct IDataReaderFactoryVtbl data_reader_factory_vtbl =
{
    data_reader_factory_QueryInterface,
    data_reader_factory_AddRef,
    data_reader_factory_Release,
    /* IInspectable methods */
    data_reader_factory_GetIids,
    data_reader_factory_GetRuntimeClassName,
    data_reader_factory_GetTrustLevel,
    /* IDataReaderFactory methods */
    data_reader_factory_CreateDataReader
};

static struct data_reader_statics data_reader_statics =
{
    {&factory_vtbl},
    {&data_reader_factory_vtbl},
    {&data_reader_statics_vtbl},
    1,
};

IActivationFactory *data_reader_factory = &data_reader_statics.IActivationFactory_iface;