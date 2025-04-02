/* WinRT Windows.Storage.Streams.DataWriter Implementation
 *
 * Written by Weather
 *
 * This is a reverse engineered implementation of Microsoft's OneCoreUAP binaries.
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
    printf("why is this getting called?\n");
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
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 1 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ] = value;
        impl->UnstoredBufferLength--;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteBytes( IDataWriter *iface, UINT32 __valueSize, BYTE* value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );

    TRACE( "iface %p, __valueSize %d, value %p\n", iface, __valueSize, value );
    
    if ( impl->UnstoredBufferLength < __valueSize )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {    
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], value, __valueSize );
        impl->UnstoredBufferLength -= __valueSize;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteBuffer( IDataWriter *iface, IBuffer* buffer )
{
    BYTE *returnedBufferBytes;
    BYTE *bufferBytes;
    UINT32 writeAhead;
    UINT32 bufferLength;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    

    TRACE( "iface %p, buffer %p\n", iface, buffer );

    hr = IBuffer_get_Length( buffer, &bufferLength );
    if ( FAILED( hr ) ) return hr;
    
    if ( impl->UnstoredBufferLength < bufferLength )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );
    if ( FAILED( hr ) ) return hr;
    IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
    IBufferByteAccess_Release( returnedBufferByteAccess );

    hr = IBuffer_QueryInterface( buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &bufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], bufferBytes, bufferLength );
        impl->UnstoredBufferLength -= bufferLength;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteBufferRange( IDataWriter *iface, IBuffer* buffer, UINT32 start, UINT32 count )
{
    BYTE *returnedBufferBytes;
    BYTE *bufferBytes;
    UINT32 writeAhead;
    UINT32 bufferLength;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    

    TRACE( "iface %p, buffer %p, start %d, count %d\n", iface, buffer, start, count );

    hr = IBuffer_get_Length( buffer, &bufferLength );
    if ( FAILED( hr ) ) return hr;

    //Checking if the range exceeds the given buffer boundries
    if ( bufferLength < start + count )
        return E_BOUNDS;
    
    if ( impl->UnstoredBufferLength < count )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );
    if ( FAILED( hr ) ) return hr;
    IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
    IBufferByteAccess_Release( returnedBufferByteAccess );

    hr = IBuffer_QueryInterface( buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &bufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], bufferBytes + start, count );
        impl->UnstoredBufferLength -= count;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}


static HRESULT WINAPI data_writer_WriteBoolean( IDataWriter *iface, boolean value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 1 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {    
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ] = value;    
        impl->UnstoredBufferLength--;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteGuid( IDataWriter *iface, GUID value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );

    TRACE( "iface %p, value %p\n", iface, &value );
    
    if ( impl->UnstoredBufferLength < 16 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );

        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value.Data1, 4 );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength + 4 ], &value.Data2, 2 );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength + 6 ], &value.Data3, 2 );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength + 8 ], &value.Data4, 8 );
        impl->UnstoredBufferLength -= 16;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }
    
    return hr;
}

static HRESULT WINAPI data_writer_WriteInt16( IDataWriter *iface, INT16 value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    

    TRACE( "iface %p, value %p\n", iface, &value );
    
    if ( impl->UnstoredBufferLength < 2 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 2 );
        impl->UnstoredBufferLength -= 2;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteInt32( IDataWriter *iface, INT32 value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 4 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 4 );
        impl->UnstoredBufferLength -= 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteInt64( IDataWriter *iface, INT64 value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 8 );
        impl->UnstoredBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteUInt16( IDataWriter *iface, UINT16 value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 2 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 2 );
        impl->UnstoredBufferLength -= 2;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteUInt32( IDataWriter *iface, UINT32 value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 4 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 4 );
        impl->UnstoredBufferLength -= 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteUInt64( IDataWriter *iface, UINT64 value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 8 );
        impl->UnstoredBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteSingle( IDataWriter *iface, FLOAT value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 4 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 4 );
        impl->UnstoredBufferLength -= 4;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteDouble( IDataWriter *iface, DOUBLE value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value, 8 );
        impl->UnstoredBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteDateTime( IDataWriter *iface, DateTime value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value.UniversalTime, 8 );
        impl->UnstoredBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteTimeSpan( IDataWriter *iface, TimeSpan value )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );    
    
    TRACE( "iface %p, value %p\n", iface, &value );

    if ( impl->UnstoredBufferLength < 8 )
        return E_BOUNDS;

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );
        memcpy( &returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], &value.Duration, 8 );
        impl->UnstoredBufferLength -= 8;

        IBufferByteAccess_Release( returnedBufferByteAccess );
    }

    return hr;
}

static HRESULT WINAPI data_writer_WriteString( IDataWriter *iface, HSTRING value, UINT32 *codeUnitCount )
{
    BYTE *returnedBufferBytes;
    UINT32 writeAhead;
    HRESULT hr;

    IBufferByteAccess *returnedBufferByteAccess;

    struct data_writer *impl = impl_from_IDataWriter( iface );  

    TRACE( "iface %p, value %p, codeUnitCount %p\n", iface, value, codeUnitCount );

    IBuffer_get_Length( impl->buffer, &writeAhead );

    hr = IBuffer_QueryInterface( impl->buffer, &IID_IBufferByteAccess, (void **)&returnedBufferByteAccess );

    if ( SUCCEEDED( hr ) )
    {
        IBufferByteAccess_get_Buffer( returnedBufferByteAccess, &returnedBufferBytes );

        switch ( impl->Encoding )
        {
            case UnicodeEncoding_Utf8:
                if ( impl->UnstoredBufferLength < WindowsGetStringLen( value ) )
                    return E_BOUNDS;
                memcpy( (LPWSTR)&returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], WindowsGetStringRawBuffer( value , NULL ), WindowsGetStringLen( value ) );
                impl->UnstoredBufferLength -= WindowsGetStringLen( value );
                *codeUnitCount = WindowsGetStringLen( value );
                break;

            default:
                if ( impl->UnstoredBufferLength < WindowsGetStringLen( value ) * 2 )
                    return E_BOUNDS;
                memcpy( (LPWSTR)&returnedBufferBytes[ writeAhead - impl->UnstoredBufferLength ], WindowsGetStringRawBuffer( value , NULL ), WindowsGetStringLen( value ) * 2 );
                impl->UnstoredBufferLength -= WindowsGetStringLen( value ) * 2;
                *codeUnitCount = WindowsGetStringLen( value ) * 2;
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

    hr = IOutputStream_WriteAsync( impl->outputStream, impl->buffer, &operation );
    if ( FAILED(hr) ) return hr;
    
    res = await_IAsyncOperationWithProgress_UINT32_UINT32( operation, INFINITE );
    if ( res ) return E_ABORT;

    hr = IAsyncOperationWithProgress_UINT32_UINT32_GetResults( operation, &bytesWritten );
    if ( FAILED(hr) ) return hr;

    if ( SUCCEEDED( hr ) )
    {
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
    hr = async_operation_uint32_create( (IUnknown *)iface, NULL, data_writer_Store, iids, operation );
    return hr;
}

static HRESULT WINAPI data_writer_FlushAsync( IDataWriter *iface, IAsyncOperation_boolean **operation )
{
    HRESULT hr;
    struct data_writer *impl = impl_from_IDataWriter( (IDataWriter *)iface );
    TRACE( "iface %p, value %p\n", iface, operation );
    hr = IOutputStream_FlushAsync( impl->outputStream, operation );
    return hr;
}

static HRESULT WINAPI data_writer_DetachBuffer( IDataWriter *iface, IBuffer **buffer )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, buffer %p\n", iface, buffer );

    if ( !buffer ) return E_POINTER;
    if ( !impl->buffer ) return E_FAIL;

    *buffer = impl->buffer;
    IBuffer_AddRef( *buffer );
    impl->buffer = NULL;

    return S_OK;
}

static HRESULT WINAPI data_writer_DetachStream( IDataWriter *iface, IOutputStream **stream )
{
    struct data_writer *impl = impl_from_IDataWriter( iface );
    TRACE( "iface %p, stream %p\n", iface, stream );

    if ( !stream ) return E_POINTER;
    if ( !impl->outputStream ) return E_FAIL;

    *stream = impl->outputStream;
    IOutputStream_AddRef( *stream );
    impl->outputStream = NULL;

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

static HRESULT WINAPI data_writer_factory_CreateDataWriter( IDataWriterFactory *iface, IOutputStream *outputStream, IDataWriter **dataReader )
{
    struct data_writer *writer;

    TRACE( "iface %p, outputStream %p, dataReader %p\n", iface, outputStream, dataReader );

    if (!(writer = calloc( 1, sizeof(*writer) ))) return E_OUTOFMEMORY;

    writer->IDataWriter_iface.lpVtbl = &data_writer_vtbl;
    writer->Encoding = UnicodeEncoding_Utf8;
    writer->Order = ByteOrder_LittleEndian;
    writer->outputStream = outputStream;
    writer->ref = 1;

    *dataReader = &writer->IDataWriter_iface;

    return S_OK;
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