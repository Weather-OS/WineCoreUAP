/* WinRT Windows.Storage.Streams.RandomAccessStream Implementation
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

#include "RandomAccessStreamInternal.h"

_ENABLE_DEBUGGING_

static struct random_access_stream_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct random_access_stream_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct random_access_stream_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IRandomAccessStreamStatics))
    {
        *out = &impl->IRandomAccessStreamStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct random_access_stream_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct random_access_stream_statics *impl = impl_from_IActivationFactory( iface );
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

DEFINE_IINSPECTABLE( random_access_stream_statics, IRandomAccessStreamStatics, struct random_access_stream_statics, IActivationFactory_iface )

static HRESULT WINAPI random_access_stream_statics_CopyAsync( IRandomAccessStreamStatics *iface, IInputStream *source, IOutputStream *destination, IAsyncOperationWithProgress_UINT64_UINT64 **operation )
{
    HRESULT hr;
    TRACE( "iface %p, input %p, destination %p, operation %p\n", iface, source, destination, operation );

    // Arguments    
    if ( !source || !destination ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = random_access_stream_statics_Copy( source, destination, 0, operation );
    TRACE( "created IAsyncOperationWithProgress_UINT64_UINT64 %p\n", operation );
    
    return hr;
}

static HRESULT WINAPI random_access_stream_statics_CopySizeAsync( IRandomAccessStreamStatics *iface, IInputStream *source, IOutputStream *destination, UINT64 bytesTocopy, IAsyncOperationWithProgress_UINT64_UINT64 **operation )
{
    HRESULT hr;
    TRACE( "iface %p, input %p, bytesTocopy %lld, destination %p, operation %p\n", iface, source, bytesTocopy, destination, operation );

    // Arguments    
    if ( !source || !destination || !bytesTocopy ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = random_access_stream_statics_Copy( source, destination, bytesTocopy, operation );
    TRACE( "created IAsyncOperationWithProgress_UINT64_UINT64 %p\n", operation );
    
    return hr;
}

static HRESULT WINAPI random_access_stream_statics_CopyAndCloseAsync( IRandomAccessStreamStatics *iface, IInputStream *source, IOutputStream *destination, IAsyncOperationWithProgress_UINT64_UINT64 **operation )
{
    HRESULT hr;
    IClosable *closable = NULL;
    TRACE( "iface %p, input %p, destination %p, operation %p\n", iface, source, destination, operation );

    // Arguments    
    if ( !source || !destination ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = random_access_stream_statics_Copy( source, destination, 0, operation );   
    if ( FAILED( hr ) ) return hr;

    hr = IInputStream_QueryInterface( source, &IID_IClosable, (void **)&closable );
    if ( SUCCEEDED( hr ) )
    {
        IClosable_Close( closable );
        IClosable_Release( closable );
    }

    TRACE( "created IAsyncOperationWithProgress_UINT64_UINT64 %p\n", operation );

    return hr;
}

const struct IRandomAccessStreamStaticsVtbl random_access_stream_statics_vtbl =
{
    random_access_stream_statics_QueryInterface,
    random_access_stream_statics_AddRef,
    random_access_stream_statics_Release,
    /* IInspectable methods */
    random_access_stream_statics_GetIids,
    random_access_stream_statics_GetRuntimeClassName,
    random_access_stream_statics_GetTrustLevel,
    /* IRandomAccessStreamStatics methods */
    random_access_stream_statics_CopyAsync,
    random_access_stream_statics_CopySizeAsync,
    random_access_stream_statics_CopyAndCloseAsync
};

static struct random_access_stream *impl_from_IClosable( IClosable *iface )
{
    return CONTAINING_RECORD( iface, struct random_access_stream, IClosable_iface );
}

static HRESULT WINAPI closable_random_access_stream_QueryInterface( IClosable *iface, REFIID iid, void **out )
{
    struct random_access_stream *impl = impl_from_IClosable( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IClosable ))
    {
        *out = &impl->IClosable_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    return IRandomAccessStream_QueryInterface( &impl->IRandomAccessStream_iface, iid, out );
}

static ULONG WINAPI closable_random_access_stream_AddRef( IClosable *iface )
{
    struct random_access_stream *impl = impl_from_IClosable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI closable_random_access_stream_Release( IClosable *iface )
{
    struct random_access_stream *impl = impl_from_IClosable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI closable_random_access_stream_GetIids( IClosable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI closable_random_access_stream_GetRuntimeClassName( IClosable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI closable_random_access_stream_GetTrustLevel( IClosable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI closable_random_access_stream_Close( IClosable *iface )
{
    struct random_access_stream *impl = impl_from_IClosable( iface );

    TRACE( "iface %p\n", iface );

    IStream_Release( impl->stream );
    IRandomAccessStream_Release( &impl->IRandomAccessStream_iface );

    return S_OK;
}

const struct IClosableVtbl closable_random_access_stream_vtbl =
{
    closable_random_access_stream_QueryInterface,
    closable_random_access_stream_AddRef,
    closable_random_access_stream_Release,
    /* IInspectable methods */
    closable_random_access_stream_GetIids,
    closable_random_access_stream_GetRuntimeClassName,
    closable_random_access_stream_GetTrustLevel,
    /* IClosable methods */
    closable_random_access_stream_Close
};

static struct random_access_stream *impl_from_IRandomAccessStream( IRandomAccessStream *iface )
{
    return CONTAINING_RECORD( iface, struct random_access_stream, IRandomAccessStream_iface );
}

static HRESULT WINAPI random_access_stream_QueryInterface( IRandomAccessStream *iface, REFIID iid, void **out )
{
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IRandomAccessStream ))
    {
        *out = &impl->IRandomAccessStream_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IClosable ))
    {
        *out = &impl->IClosable_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IInputStream ))
    {
        IRandomAccessStream_GetInputStreamAt( iface, impl->Position, (IInputStream **)out );
        //Position can be controlled when queried.
        impl_from_IInputStream( (IInputStream *)*out )->headPosition = &impl->Position;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IOutputStream ))
    {
        IRandomAccessStream_GetOutputStreamAt( iface, impl->Position, (IOutputStream **)out );
        //Position can be controlled when queried.
        impl_from_IOutputStream( (IOutputStream *)*out )->headPosition = &impl->Position;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI random_access_stream_AddRef( IRandomAccessStream *iface )
{
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI random_access_stream_Release( IRandomAccessStream *iface )
{
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI random_access_stream_GetIids( IRandomAccessStream *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_GetRuntimeClassName( IRandomAccessStream *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_GetTrustLevel( IRandomAccessStream *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_get_Size( IRandomAccessStream *iface, UINT64 *value )
{
    HRESULT hr;
    LARGE_INTEGER endPosition = {.QuadPart = 0};
    LARGE_INTEGER currentPosition = {0};
    LARGE_INTEGER signedOriginalPosition;
    ULARGE_INTEGER originalPosition;
    ULARGE_INTEGER size;

    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );

    TRACE( "iface %p, value %p\n", iface, value );

    hr = IStream_Seek( impl->stream, currentPosition, STREAM_SEEK_CUR, &originalPosition );
    if ( FAILED( hr ) ) return hr;

    hr = IStream_Seek( impl->stream, endPosition, STREAM_SEEK_END, &size );
    if ( FAILED( hr ) ) return hr;

    *value = size.QuadPart;

    signedOriginalPosition.QuadPart = (INT64)originalPosition.QuadPart;

    return IStream_Seek( impl->stream, signedOriginalPosition, STREAM_SEEK_SET, NULL);
}

static HRESULT WINAPI random_access_stream_put_Size( IRandomAccessStream *iface, UINT64 value )
{
    ULARGE_INTEGER size;

    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );

    TRACE( "iface %p, value %llu\n", iface, value );

    size.QuadPart = value;

    return IStream_SetSize( impl->stream, size );
}

static HRESULT WINAPI random_access_stream_GetInputStreamAt( IRandomAccessStream *iface, UINT64 position, IInputStream **stream )
{
    HRESULT hr; 
    LARGE_INTEGER liMove;

    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    struct input_stream *input;

    TRACE( "iface %p, position %llu, stream %p\n", iface, position, stream );

    if (!(input = calloc( 1, sizeof(*input) ))) return E_OUTOFMEMORY;

    liMove.QuadPart = position;
    
    hr = IStream_Seek( impl->stream, liMove, STREAM_SEEK_SET, NULL );

    if ( SUCCEEDED( hr ) )
    {
        input->IInputStream_iface.lpVtbl = &input_stream_vtbl;
        input->IClosable_iface.lpVtbl = &closable_stream_vtbl;
        input->stream = impl->stream;
        input->closableRef = 1;
        input->ref = 1;
        input->updatePos = position;

        impl->Position = position;

        *stream = &input->IInputStream_iface;
    } else {
        free( stream );
    }

    return hr;
}

static HRESULT WINAPI random_access_stream_GetOutputStreamAt( IRandomAccessStream *iface, UINT64 position, IOutputStream **stream )
{
    HRESULT hr;
    LARGE_INTEGER liMove;

    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    struct output_stream *output;

    TRACE( "iface %p, position %llu, stream %p\n", iface, position, stream );

    if (!(output = calloc( 1, sizeof(*output) ))) return E_OUTOFMEMORY;
    
    liMove.QuadPart = position;
    
    hr = IStream_Seek( impl->stream, liMove, STREAM_SEEK_SET, NULL );
    if ( SUCCEEDED( hr ) )
    {
        output->IOutputStream_iface.lpVtbl = &output_stream_vtbl;
        output->IClosable_iface.lpVtbl = &closable_stream_vtbl;
        output->stream = impl->stream;
        output->closableRef = 1;
        output->ref = 1;
        output->currentOperation = NULL;
        output->updatePos = position;

        impl->Position = position;

        *stream = &output->IOutputStream_iface;        
    } else {    
        free( output );
    }

    return hr;
}

static HRESULT WINAPI random_access_stream_get_Position( IRandomAccessStream *iface, UINT64 *value )
{
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Position;
    return S_OK;
}

static HRESULT WINAPI random_access_stream_Seek( IRandomAccessStream *iface, UINT64 position )
{
    //According to MSDN, Checking position validation is handled by the application itself.
    LARGE_INTEGER liMove;
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    TRACE( "iface %p, position %llu\n", iface, position );
    liMove.QuadPart = position;
    IStream_Seek( impl->stream, liMove, STREAM_SEEK_SET, NULL );
    impl->Position = position;
    return S_OK;
}

static HRESULT WINAPI random_access_stream_CloneStream( IRandomAccessStream *iface, IRandomAccessStream **stream )
{
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    struct random_access_stream *cloned_stream;

    TRACE( "iface %p, stream %p\n", iface, stream );

    if (!(cloned_stream = calloc( 1, sizeof(*cloned_stream) ))) return E_OUTOFMEMORY;

    cloned_stream->IRandomAccessStream_iface.lpVtbl = &random_access_stream_vtbl;
    cloned_stream->IClosable_iface.lpVtbl = &closable_random_access_stream_vtbl;
    IStream_Clone( impl->stream, &cloned_stream->stream );
    cloned_stream->closableRef = impl->closableRef;
    cloned_stream->Position = 0;
    cloned_stream->CanRead = impl->CanRead;
    cloned_stream->CanWrite = impl->CanWrite;
    cloned_stream->ref = 1;

    *stream = &cloned_stream->IRandomAccessStream_iface;
    return S_OK;
}

static HRESULT WINAPI random_access_stream_get_CanRead( IRandomAccessStream *iface, boolean *value )
{
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->CanRead;
    return S_OK;
}

static HRESULT WINAPI random_access_stream_get_CanWrite( IRandomAccessStream *iface, boolean *value )
{
    struct random_access_stream *impl = impl_from_IRandomAccessStream( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->CanWrite;
    return S_OK;
}

const struct IRandomAccessStreamVtbl random_access_stream_vtbl =
{
    random_access_stream_QueryInterface,
    random_access_stream_AddRef,
    random_access_stream_Release,
    /* IInspectable methods */
    random_access_stream_GetIids,
    random_access_stream_GetRuntimeClassName,
    random_access_stream_GetTrustLevel,
    /* IRandomAccessStream methods */
    random_access_stream_get_Size,
    random_access_stream_put_Size,
    random_access_stream_GetInputStreamAt,
    random_access_stream_GetOutputStreamAt,
    random_access_stream_get_Position,
    random_access_stream_Seek,
    random_access_stream_CloneStream,
    random_access_stream_get_CanRead,
    random_access_stream_get_CanWrite
};

static struct random_access_stream_with_content_type *impl_from_IRandomAccessStreamWithContentType( IRandomAccessStreamWithContentType *iface )
{
    return CONTAINING_RECORD( iface, struct random_access_stream_with_content_type, IRandomAccessStreamWithContentType_iface );
}

static HRESULT WINAPI random_access_stream_with_content_type_QueryInterface( IRandomAccessStreamWithContentType *iface, REFIID iid, void **out )
{
    struct random_access_stream_with_content_type *impl = impl_from_IRandomAccessStreamWithContentType( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IRandomAccessStreamWithContentType ))
    {
        *out = &impl->IRandomAccessStreamWithContentType_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IRandomAccessStream ))
    {
        *out = &impl->IRandomAccessStream_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IContentTypeProvider ))
    {
        *out = &impl->IContentTypeProvider_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return IRandomAccessStream_QueryInterface( &impl->IRandomAccessStream_iface, iid, out );
}

static ULONG WINAPI random_access_stream_with_content_type_AddRef( IRandomAccessStreamWithContentType *iface )
{
    struct random_access_stream_with_content_type *impl = impl_from_IRandomAccessStreamWithContentType( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI random_access_stream_with_content_type_Release( IRandomAccessStreamWithContentType *iface )
{
    struct random_access_stream_with_content_type *impl = impl_from_IRandomAccessStreamWithContentType( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI random_access_stream_with_content_type_GetIids( IRandomAccessStreamWithContentType *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_with_content_type_GetRuntimeClassName( IRandomAccessStreamWithContentType *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_with_content_type_GetTrustLevel( IRandomAccessStreamWithContentType *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

const struct IRandomAccessStreamWithContentTypeVtbl random_access_stream_with_content_type_vtbl =
{
    random_access_stream_with_content_type_QueryInterface,
    random_access_stream_with_content_type_AddRef,
    random_access_stream_with_content_type_Release,
    /* IInspectable methods */
    random_access_stream_with_content_type_GetIids,
    random_access_stream_with_content_type_GetRuntimeClassName,
    random_access_stream_with_content_type_GetTrustLevel,
};

static struct random_access_stream_statics random_access_stream_statics =
{
    {&factory_vtbl},
    {&random_access_stream_statics_vtbl},
    1,
};

IActivationFactory *random_access_stream_factory = &random_access_stream_statics.IActivationFactory_iface;