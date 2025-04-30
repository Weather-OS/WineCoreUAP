/* WinRT InputOutputStream Implementation
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

#include "InputOutputStreamInternal.h"

_ENABLE_DEBUGGING_

static struct closable_stream *impl_from_IClosable( IClosable *iface )
{
    return CONTAINING_RECORD( iface, struct closable_stream, IClosable_iface );
}

static HRESULT WINAPI closable_stream_QueryInterface( IClosable *iface, REFIID iid, void **out )
{
    struct closable_stream *impl = impl_from_IClosable( iface );

    //Inheritance
    struct input_stream *inheritedInput = CONTAINING_RECORD( &impl->IClosable_iface, struct input_stream, IClosable_iface );
    struct output_stream *inheritedOutput = CONTAINING_RECORD( &impl->IClosable_iface, struct output_stream, IClosable_iface );

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

    if ( inheritedInput->IInputStream_iface.lpVtbl == &input_stream_vtbl )
    {
        return IInputStream_QueryInterface( &inheritedInput->IInputStream_iface, iid, out );
    }

    if ( inheritedOutput->IOutputStream_iface.lpVtbl == &output_stream_vtbl )
    {
        return IOutputStream_QueryInterface( &inheritedOutput->IOutputStream_iface, iid, out );
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI closable_stream_AddRef( IClosable *iface )
{
    struct closable_stream *impl = impl_from_IClosable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI closable_stream_Release( IClosable *iface )
{
    struct closable_stream *impl = impl_from_IClosable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI closable_stream_GetIids( IClosable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI closable_stream_GetRuntimeClassName( IClosable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI closable_stream_GetTrustLevel( IClosable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI closable_stream_Close( IClosable *iface )
{
    struct closable_stream *impl = impl_from_IClosable( iface );

    struct input_stream *inheritedInput = CONTAINING_RECORD( &impl->IClosable_iface, struct input_stream, IClosable_iface );
    struct output_stream *inheritedOutput = CONTAINING_RECORD( &impl->IClosable_iface, struct output_stream, IClosable_iface );

    if ( inheritedInput->IInputStream_iface.lpVtbl == &input_stream_vtbl )
    {
        IStream_Release( inheritedInput->stream );
        IInputStream_Release( &inheritedInput->IInputStream_iface );
        return S_OK;
    }

    if ( inheritedOutput->IOutputStream_iface.lpVtbl == &output_stream_vtbl )
    {
        IStream_Release( inheritedOutput->stream );
        IOutputStream_Release( &inheritedOutput->IOutputStream_iface );
        return S_OK;
    }

    return E_ABORT;
}

const struct IClosableVtbl closable_stream_vtbl =
{
    closable_stream_QueryInterface,
    closable_stream_AddRef,
    closable_stream_Release,
    /* IInspectable methods */
    closable_stream_GetIids,
    closable_stream_GetRuntimeClassName,
    closable_stream_GetTrustLevel,
    /* IClosable methods */
    closable_stream_Close
};

struct input_stream *impl_from_IInputStream( IInputStream *iface )
{
    return CONTAINING_RECORD( iface, struct input_stream, IInputStream_iface );
}

static HRESULT WINAPI input_stream_QueryInterface( IInputStream *iface, REFIID iid, void **out )
{
    struct input_stream *impl = impl_from_IInputStream( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IInputStream ))
    {
        *out = &impl->IInputStream_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IClosable ))
    {
        *out = &impl->IClosable_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI input_stream_AddRef( IInputStream *iface )
{
    struct input_stream *impl = impl_from_IInputStream( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI input_stream_Release( IInputStream *iface )
{
    struct input_stream *impl = impl_from_IInputStream( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI input_stream_GetIids( IInputStream *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI input_stream_GetRuntimeClassName( IInputStream *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI input_stream_GetTrustLevel( IInputStream *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI input_stream_ReadAsync( IInputStream *iface, IBuffer *buffer, UINT32 count, InputStreamOptions options, IAsyncOperationWithProgress_IBuffer_UINT32 **operation )
{
    HRESULT hr;
    struct input_stream_options *input_stream_options;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperationWithProgress_IBuffer_UINT32 };

    TRACE( "iface %p, operation %p\n", iface, operation );

    // Arguments    
    if ( !buffer || !count ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(input_stream_options = calloc( 1, sizeof(*input_stream_options) ))) return E_OUTOFMEMORY;

    input_stream_options->buffer = buffer;
    input_stream_options->count = count;
    input_stream_options->options = options;

    hr = async_operation_with_progress_create( (IUnknown *)iface, (IUnknown *)input_stream_options, input_stream_Read, iids, (IAsyncOperationWithProgress_IInspectable_UINT64 **)operation );
    TRACE( "created IAsyncOperationWithProgress_IBuffer_UINT32 %p\n", operation );
    return hr;
}

const struct IInputStreamVtbl input_stream_vtbl =
{
    input_stream_QueryInterface,
    input_stream_AddRef,
    input_stream_Release,
    /* IInspectable methods */
    input_stream_GetIids,
    input_stream_GetRuntimeClassName,
    input_stream_GetTrustLevel,
    /* IInputStream methods */
    input_stream_ReadAsync
};

struct output_stream *impl_from_IOutputStream( IOutputStream *iface )
{
    return CONTAINING_RECORD( iface, struct output_stream, IOutputStream_iface );
}

static HRESULT WINAPI output_stream_QueryInterface( IOutputStream *iface, REFIID iid, void **out )
{
    struct output_stream *impl = impl_from_IOutputStream( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IOutputStream ))
    {
        *out = &impl->IOutputStream_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IClosable ))
    {
        *out = &impl->IClosable_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI output_stream_AddRef( IOutputStream *iface )
{
    struct output_stream *impl = impl_from_IOutputStream( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI output_stream_Release( IOutputStream *iface )
{
    struct output_stream *impl = impl_from_IOutputStream( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI output_stream_GetIids( IOutputStream *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI output_stream_GetRuntimeClassName( IOutputStream *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI output_stream_GetTrustLevel( IOutputStream *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI output_stream_WriteAsync( IOutputStream *iface, IBuffer *buffer, IAsyncOperationWithProgress_UINT32_UINT32 **operation )
{
    IAsyncInfo *info;

    HRESULT hr;
    AsyncStatus status;
    struct output_stream *impl = impl_from_IOutputStream( iface );    
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperationWithProgress_UINT32_UINT32 };
    
    TRACE( "iface %p, operation %p\n", iface, operation );

    // Arguments    
    if ( !buffer ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if ( impl->currentOperation )
    {
        IAsyncOperationWithProgress_UINT32_UINT32_QueryInterface( impl->currentOperation, &IID_IAsyncInfo, (void **)&info );
        IAsyncInfo_get_Status( info, &status );

        if ( status == Started )
            return E_ILLEGAL_METHOD_CALL;
    }

    hr = async_operation_with_progress_uint32_create( (IUnknown *)iface, (IUnknown *)buffer, output_stream_Write, iids, operation );
    impl->currentOperation = *operation;
    
    TRACE( "created IAsyncOperationWithProgress_UINT32_UINT32 %p\n", operation );
    return hr;
}

static HRESULT WINAPI output_stream_FlushAsync( IOutputStream *iface, IAsyncOperation_boolean **operation )
{
    HRESULT hr;

    TRACE( "iface %p, operation %p\n", iface, operation );

    // Arguments    
    if ( !operation ) return E_POINTER;

    hr = async_operation_boolean_create( (IUnknown *)iface, NULL, output_stream_Flush, operation );
    TRACE( "created IAsyncOperation_boolean %p\n", operation );
    return hr;
}

const struct IOutputStreamVtbl output_stream_vtbl =
{
    output_stream_QueryInterface,
    output_stream_AddRef,
    output_stream_Release,
    /* IInspectable methods */
    output_stream_GetIids,
    output_stream_GetRuntimeClassName,
    output_stream_GetTrustLevel,
    /* IOutputStream methods */
    output_stream_WriteAsync,
    output_stream_FlushAsync
};