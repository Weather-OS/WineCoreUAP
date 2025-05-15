/* WinRT Windows.Storage.Streams.RandomAccessStreamReference Implementation
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
#include "RandomAccessStreamReferenceInternal.h"

_ENABLE_DEBUGGING_

HRESULT WINAPI random_access_stream_reference_CreateStreamReference( HSTRING path, FileAccessMode accessMode, IRandomAccessStreamReference **value )
{
    HANDLE stream;

    struct random_access_stream_reference *reference;

    if (!(reference = calloc( 1, sizeof(*reference) ))) return E_OUTOFMEMORY;

    reference->IRandomAccessStreamReference_iface.lpVtbl = &random_access_stream_reference_vtbl;

    // Readability
    stream = CreateFileW( WindowsGetStringRawBuffer( path, NULL ), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
    reference->streamSize = GetFileSize( stream, NULL );
    if ( stream != INVALID_HANDLE_VALUE ) {
        reference->canRead = TRUE;
    }
    CloseHandle(stream);

    // Writability
    if ( accessMode == FileAccessMode_ReadWrite )
    {
        stream = CreateFileW( WindowsGetStringRawBuffer( path, NULL ), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if ( stream != INVALID_HANDLE_VALUE ) {
            reference->canWrite = TRUE;
        }
        CloseHandle(stream);
    }

    reference->ref = 1;

    WindowsDuplicateString( path, &reference->handlePath );

    *value = &reference->IRandomAccessStreamReference_iface;
    
    return S_OK;
}

HRESULT WINAPI random_access_stream_reference_CreateStream( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;

    struct random_access_stream *stream;
    struct random_access_stream_reference *reference = impl_from_IRandomAccessStreamReference( (IRandomAccessStreamReference *)invoker );

    TRACE( "invoker %p, result %p\n", invoker, result );

    if (!(stream = calloc( 1, sizeof(*stream) ))) return E_OUTOFMEMORY;

    stream->IRandomAccessStream_iface.lpVtbl = &random_access_stream_vtbl;
    stream->IClosable_iface.lpVtbl = &closable_random_access_stream_vtbl;
    stream->closableRef = 1;
    stream->Position = 0;
    stream->CanRead = reference->canRead;
    stream->CanWrite = reference->canWrite;
    stream->ref = 1;

    if ( reference->canRead && reference->canWrite )
    {
        status = SHCreateStreamOnFileEx( WindowsGetStringRawBuffer( reference->handlePath, NULL ), STGM_READWRITE, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &stream->stream );
    } else if ( reference->canRead )
    {
        status = SHCreateStreamOnFileEx( WindowsGetStringRawBuffer( reference->handlePath, NULL ), STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &stream->stream );
    } else if ( reference->canWrite )
    {
        status = SHCreateStreamOnFileEx( WindowsGetStringRawBuffer( reference->handlePath, NULL ), STGM_WRITE, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &stream->stream );
    } else {
        status = E_ACCESSDENIED;
    }

    if ( SUCCEEDED( status ) ) 
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)&stream->IRandomAccessStream_iface;
    } else {
        free( stream );
    }

    return status;
}

HRESULT WINAPI random_access_stream_reference_CreateReadOnlyStream( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    HRESULT status = S_OK;

    struct random_access_stream_with_content_type *stream;
    struct random_access_stream_reference *reference = impl_from_IRandomAccessStreamReference( (IRandomAccessStreamReference *)invoker );

    TRACE( "invoker %p, result %p\n", invoker, result );

    if (!(stream = calloc( 1, sizeof(*stream) ))) return E_OUTOFMEMORY;

    stream->IRandomAccessStreamWithContentType_iface.lpVtbl = &random_access_stream_with_content_type_vtbl;
    stream->IRandomAccessStream_iface.lpVtbl = &random_access_stream_vtbl;
    stream->IContentTypeProvider_iface.lpVtbl = &content_type_provider_vtbl;
    stream->ContentType = reference->contentType;
    stream->IClosable_iface.lpVtbl = &closable_random_access_stream_vtbl;
    stream->closableRef = 1;
    stream->Position = 0;
    stream->CanRead = reference->canRead;
    stream->CanWrite = FALSE;
    stream->ref = 1;
    stream->randomAccessStreamRef = 1;
    stream->contentTypeRef = 1;

    if ( reference->canRead )
    {
        status = SHCreateStreamOnFileEx( WindowsGetStringRawBuffer( reference->handlePath, NULL ), STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, NULL, &stream->stream );
    } else {
        status = E_ACCESSDENIED;
    }

    if ( SUCCEEDED( status ) ) 
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)&stream->IRandomAccessStreamWithContentType_iface;
    } else {
        free( stream );
    }

    return status;
}