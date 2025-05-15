/* WinRT Windows.Storage.Streams.DataWriter Implementation
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

#include "DataWriterInternal.h"
#include "BufferInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( async_uint_uint_handler, IAsyncOperationWithProgressCompletedHandler_UINT32_UINT32, IAsyncOperationWithProgress_UINT32_UINT32 )

// Data Writer

static struct data_writer_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct data_writer_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct data_writer_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IDataWriterFactory))
    {
        *out = &impl->IDataWriterFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct data_writer_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct data_writer_statics *impl = impl_from_IActivationFactory( iface );
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

struct data_writer *impl_from_IDataWriter( IDataWriter *iface )
{
    return CONTAINING_RECORD( iface, struct data_writer, IDataWriter_iface );
}

static HRESULT WINAPI data_writer_QueryInterface( IDataWriter *iface, REFIID iid, void **out )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IDataWriter ))
    {
        *out = &impl->IDataWriter_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI data_writer_AddRef( IDataWriter *iface )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI data_writer_Release( IDataWriter *iface )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI data_writer_GetIids( IDataWriter *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_writer_GetRuntimeClassName( IDataWriter *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_writer_GetTrustLevel( IDataWriter *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_writer_get_UnstoredBufferLength( IDataWriter *iface, UINT32* value )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->UnstoredBufferLength;
    return S_OK;
}

static HRESULT WINAPI data_writer_get_UnicodeEncoding( IDataWriter *iface, UnicodeEncoding* value )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Encoding;
    return S_OK;
}

static HRESULT WINAPI data_writer_put_UnicodeEncoding( IDataWriter *iface, UnicodeEncoding value )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, value %d\n", iface, value );
    impl->Encoding = value;
    return S_OK;
}

static HRESULT WINAPI data_writer_get_ByteOrder( IDataWriter *iface, ByteOrder* value )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Order;
    return S_OK;
}

static HRESULT WINAPI data_writer_put_ByteOrder( IDataWriter *iface, ByteOrder value )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, value %d\n", iface, value );
    impl->Order = value;
    return S_OK;
}

static HRESULT WINAPI data_writer_WriteByte( IDataWriter *iface, BYTE value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 1 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        returnedBufferBytes[ impl->UnstoredBufferLength ] = value;
        impl->UnstoredBufferLength--;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteBytes( IDataWriter *iface, UINT32 __valueSize, BYTE* value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );

    TRACE( "iface %p, __valueSize %d, value %p\n", iface, __valueSize, value );

    // Arguments
    if ( !value || !__valueSize ) return E_INVALIDARG;

    hr = buffer_Grow( impl->buffer, __valueSize );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {    
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], value, __valueSize );
        impl->UnstoredBufferLength += __valueSize;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteBuffer( IDataWriter *iface, IBuffer* buffer )
{
    BYTE *returnedBufferBytes;
    BYTE *bufferBytes;
    UINT32 bufferLength;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    

    TRACE( "iface %p, buffer %p\n", iface, buffer );

    // Arguments
    if ( !buffer ) return E_INVALIDARG;

    hr = IBuffer_get_Length( buffer, &bufferLength );
    if ( FAILED( hr ) ) return hr;

    hr = buffer_Grow( impl->buffer, bufferLength );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );
    if ( FAILED( hr ) ) return hr;
    IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
    IBufferByteAccess_Release( returnedBufferByteAccess );

    hr = IBuffer_QueryInterface( buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &bufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], bufferBytes, bufferLength );
        impl->UnstoredBufferLength += bufferLength;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteBufferRange( IDataWriter *iface, IBuffer* buffer, UINT32 start, UINT32 count )
{
    BYTE *returnedBufferBytes;
    BYTE *bufferBytes;
    UINT32 bufferLength;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    

    TRACE( "iface %p, buffer %p, start %d, count %d\n", iface, buffer, start, count );

    // Arguments
    if ( !buffer || !start || !count ) return E_INVALIDARG;

    hr = IBuffer_get_Length( buffer, &bufferLength );
    if ( FAILED( hr ) ) return hr;

    //Checking if the range exceeds the given buffer boundries
    if ( bufferLength < start + count )
        return E_BOUNDS;

    hr = buffer_Grow( impl->buffer, count );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );
    if ( FAILED( hr ) ) return hr;
    IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
    IBufferByteAccess_Release( returnedBufferByteAccess );

    hr = IBuffer_QueryInterface( buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &bufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], bufferBytes + start, count );
        impl->UnstoredBufferLength += count;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}


static HRESULT WINAPI data_writer_WriteBoolean( IDataWriter *iface, boolean value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 1 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {    
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        returnedBufferBytes[ impl->UnstoredBufferLength ] = value;    
        impl->UnstoredBufferLength--;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteGuid( IDataWriter *iface, GUID value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );

    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 16 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );

        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value.Data1, 4 );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength + 4 ], &value.Data2, 2 );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength + 6 ], &value.Data3, 2 );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength + 8 ], &value.Data4, 8 );
        impl->UnstoredBufferLength += 16;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }
    
    return hr;
}

static HRESULT WINAPI data_writer_WriteInt16( IDataWriter *iface, INT16 value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    

    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 2 );
    if ( FAILED( hr ) ) return hr;
    
    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 2 );
        impl->UnstoredBufferLength += 2;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteInt32( IDataWriter *iface, INT32 value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );
    
    hr = buffer_Grow( impl->buffer, 4 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 4 );
        impl->UnstoredBufferLength += 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteInt64( IDataWriter *iface, INT64 value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 8 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 8 );
        impl->UnstoredBufferLength += 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteUInt16( IDataWriter *iface, UINT16 value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 2 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 2 );
        impl->UnstoredBufferLength += 2;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteUInt32( IDataWriter *iface, UINT32 value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 4 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 4 );
        impl->UnstoredBufferLength += 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteUInt64( IDataWriter *iface, UINT64 value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 8 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 8 );
        impl->UnstoredBufferLength += 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteSingle( IDataWriter *iface, FLOAT value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 4 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 4 );
        impl->UnstoredBufferLength += 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteDouble( IDataWriter *iface, DOUBLE value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 8 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value, 8 );
        impl->UnstoredBufferLength += 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteDateTime( IDataWriter *iface, DateTime value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 8 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value.UniversalTime, 8 );
        impl->UnstoredBufferLength += 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteTimeSpan( IDataWriter *iface, TimeSpan value )
{
    BYTE *returnedBufferBytes;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    hr = buffer_Grow( impl->buffer, 8 );
    if ( FAILED( hr ) ) return hr;

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], &value.Duration, 8 );
        impl->UnstoredBufferLength += 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteString( IDataWriter *iface, HSTRING value, UINT32 *codeUnitCount )
{
    BYTE *returnedBufferBytes;
    LPSTR convertedStr;
    UINT32 strSize;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );  

    TRACE( "iface %p, value %p, codeUnitCount %p\n", iface, value, codeUnitCount );

    // Arguments
    if ( !value ) return E_INVALIDARG;
    if ( !codeUnitCount ) return E_POINTER;

    switch ( impl->Encoding )
    {
        case UnicodeEncoding_Utf8:
            hr = buffer_Grow( impl->buffer, WindowsGetStringLen( value ) );    
            strSize = WideCharToMultiByte( CP_UTF8, 0, WindowsGetStringRawBuffer( value , NULL ), -1, NULL, 0, NULL, NULL );
            convertedStr = (LPSTR)malloc( strSize );
            WideCharToMultiByte( CP_UTF8, 0, WindowsGetStringRawBuffer(value, NULL), -1, convertedStr, strSize, NULL, NULL );
            if ( FAILED( hr ) ) return hr;
            break;

        default:
            hr = buffer_Grow( impl->buffer, WindowsGetStringLen( value ) * 2 );
            strSize = WideCharToMultiByte( CP_UTF7, 0, WindowsGetStringRawBuffer( value , NULL ), -1, NULL, 0, NULL, NULL );
            convertedStr = (LPSTR)malloc( strSize );
            WideCharToMultiByte( CP_UTF7, 0, WindowsGetStringRawBuffer(value, NULL), -1, convertedStr, strSize, NULL, NULL );
            if ( FAILED( hr ) ) return hr;
            break;
    }


    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_Buffer( returnedBufferByteAccess, &returnedBufferBytes );

        switch ( impl->Encoding )
        {
            case UnicodeEncoding_Utf8:
                memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], convertedStr, strSize );
                impl->UnstoredBufferLength += strlen( convertedStr );
                *codeUnitCount = strSize;
                break;

            default:
                memcpy( &returnedBufferBytes[ impl->UnstoredBufferLength ], convertedStr, strSize * 2 );
                impl->UnstoredBufferLength += strSize * 2;
                *codeUnitCount = strSize * 2;
                break;
        }
    }
    
    return hr;
}

static HRESULT WINAPI data_writer_MeasureString( IDataWriter *iface, HSTRING value, UINT32 *codeUnitCount )
{
    HRESULT hr = S_OK;

    struct data_writer *impl = impl_from_IDataWriter( iface );  

    TRACE( "iface %p, value %p, codeUnitCount %p\n", iface, value, codeUnitCount );

    // Arguments
    if ( !value ) return E_INVALIDARG;
    if ( !codeUnitCount ) return E_POINTER;


    switch ( impl->Encoding )
    {
        case UnicodeEncoding_Utf8:
            *codeUnitCount = WindowsGetStringLen( value );
            break;

        default:
            *codeUnitCount = WindowsGetStringLen( value ) * 2;
            break;
    }
    
    return hr;
}

static HRESULT WINAPI data_writer_Store( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    DWORD res;
    UINT32 bytesWritten;
    HRESULT hr;

    IAsyncOperationWithProgress_UINT32_UINT32 *operation;

    struct data_writer *impl = impl_from_IDataWriter( (IDataWriter *)invoker );

    TRACE( "iface %p, value %p\n", invoker, result );

    // Setting the length of the buffer during submission saves some cpu cycles.
    IBuffer_put_Length( impl->buffer, impl->UnstoredBufferLength );

    hr = IOutputStream_WriteAsync( impl->outputStream, impl->buffer, &operation );
    if ( FAILED( hr ) ) return hr;
    
    res = await_IAsyncOperationWithProgress_UINT32_UINT32( operation, INFINITE );
    if ( res ) return E_ABORT;

    hr = IAsyncOperationWithProgress_UINT32_UINT32_GetResults( operation, &bytesWritten );
    if ( FAILED( hr ) ) return hr;

    IAsyncOperationWithProgress_UINT32_UINT32_Release( operation );

    CHECK_LAST_RESTRICTED_ERROR();

    //Buffer is cleared after each store operation.
    //A new buffer is created afterwards.
    IBuffer_Release( impl->buffer );
    buffer_Create( 0, &impl->buffer );

    if ( SUCCEEDED( hr ) )
    {
        impl->UnstoredBufferLength = 0;
        result->vt = VT_UI4;
        result->ulVal = (UINT64)bytesWritten;
    }

    return hr;
}

static HRESULT WINAPI data_writer_StoreAsync( IDataWriter *iface, IAsyncOperation_UINT32 **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_UINT32 };

    TRACE( "iface %p, value %p\n", iface, operation );

    // Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_uint32_create( (IUnknown *)iface, NULL, data_writer_Store, iids, operation );

    return hr;
}

static HRESULT WINAPI data_writer_FlushAsync( IDataWriter *iface, IAsyncOperation_boolean **operation )
{
    HRESULT hr;

    struct data_writer *impl = impl_from_IDataWriter( (IDataWriter *)iface );

    TRACE( "iface %p, value %p\n", iface, operation );

    // Arguments
    if ( !operation ) return E_POINTER;
    
    hr = IOutputStream_FlushAsync( impl->outputStream, operation );

    return hr;
}

static HRESULT WINAPI data_writer_DetachBuffer( IDataWriter *iface, IBuffer **buffer )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, buffer %p\n", iface, buffer );

    // Arguments
    if ( !buffer ) return E_POINTER;

    if ( !impl->buffer ) return E_FAIL;

    // Setting the length of the buffer during submission saves some cpu cycles.
    IBuffer_put_Length( impl->buffer, impl->UnstoredBufferLength );

    *buffer = impl->buffer;
    IBuffer_AddRef( *buffer );

    return S_OK;
}

static HRESULT WINAPI data_writer_DetachStream( IDataWriter *iface, IOutputStream **stream )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, stream %p\n", iface, stream );

    // Arguments
    if ( !stream ) return E_POINTER;

    if ( !impl->outputStream ) return E_FAIL;

    *stream = impl->outputStream;
    IOutputStream_AddRef( *stream );

    return S_OK;
}


struct IDataWriterVtbl data_writer_vtbl =
{
    data_writer_QueryInterface,
    data_writer_AddRef,
    data_writer_Release,
    /* IInspectable methods */
    data_writer_GetIids,
    data_writer_GetRuntimeClassName,
    data_writer_GetTrustLevel,
    /* IDataWriter methods */
    data_writer_get_UnstoredBufferLength,
    data_writer_get_UnicodeEncoding,
    data_writer_put_UnicodeEncoding,
    data_writer_get_ByteOrder,
    data_writer_put_ByteOrder,
    data_writer_WriteByte,
    data_writer_WriteBytes,
    data_writer_WriteBuffer,
    data_writer_WriteBufferRange,
    data_writer_WriteBoolean,
    data_writer_WriteGuid,
    data_writer_WriteInt16,
    data_writer_WriteInt32,
    data_writer_WriteInt64,
    data_writer_WriteUInt16,
    data_writer_WriteUInt32,
    data_writer_WriteUInt64,
    data_writer_WriteSingle,
    data_writer_WriteDouble,    
    data_writer_WriteDateTime,
    data_writer_WriteTimeSpan,
    data_writer_WriteString,
    data_writer_MeasureString,
    data_writer_StoreAsync,
    data_writer_FlushAsync,    
    data_writer_DetachBuffer,
    data_writer_DetachStream,
};

struct data_writer_statics *impl_from_IDataWriterFactory( IDataWriterFactory *iface )
{
    return CONTAINING_RECORD( iface, struct data_writer_statics, IDataWriterFactory_iface );
}

static HRESULT WINAPI data_writer_factory_QueryInterface( IDataWriterFactory *iface, REFIID iid, void **out )
{
    struct data_writer_statics *impl = impl_from_IDataWriterFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IDataWriterFactory))
    {
        *out = &impl->IDataWriterFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI data_writer_factory_AddRef( IDataWriterFactory *iface )
{
    struct data_writer_statics *impl = impl_from_IDataWriterFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI data_writer_factory_Release( IDataWriterFactory *iface )
{
    struct data_writer_statics *impl = impl_from_IDataWriterFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI data_writer_factory_GetIids( IDataWriterFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_writer_factory_GetRuntimeClassName( IDataWriterFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_writer_factory_GetTrustLevel( IDataWriterFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI data_writer_factory_CreateDataWriter( IDataWriterFactory *iface, IOutputStream *outputStream, IDataWriter **dataWriter )
{
    struct data_writer *writer;

    TRACE( "iface %p, outputStream %p, dataWriter %p\n", iface, outputStream, dataWriter );

    // Arguments
    if ( !outputStream ) return E_INVALIDARG;
    if ( !dataWriter ) return E_POINTER;

    if (!(writer = calloc( 1, sizeof(*writer) ))) return E_OUTOFMEMORY;

    writer->IDataWriter_iface.lpVtbl = &data_writer_vtbl;
    writer->Encoding = UnicodeEncoding_Utf8;
    writer->Order = ByteOrder_LittleEndian;
    writer->outputStream = outputStream;
    writer->ref = 1;

    *dataWriter = &writer->IDataWriter_iface;

    //This buffer is dynamically reallocated 
    return buffer_Create( 0, &writer->buffer );
}


const struct IDataWriterFactoryVtbl data_writer_factory_vtbl =
{
    data_writer_factory_QueryInterface,
    data_writer_factory_AddRef,
    data_writer_factory_Release,
    /* IInspectable methods */
    data_writer_factory_GetIids,
    data_writer_factory_GetRuntimeClassName,
    data_writer_factory_GetTrustLevel,
    /* IDataWriterFactory methods */
    data_writer_factory_CreateDataWriter
};

static struct data_writer_statics data_writer_statics =
{
    {&factory_vtbl},
    {&data_writer_factory_vtbl},
    1,
};

IActivationFactory *data_writer_factory = &data_writer_statics.IActivationFactory_iface;