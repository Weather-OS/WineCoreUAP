/* WinRT Windows.Storage.FileIO Implementation
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

#include "FileIOInternal.h"

_ENABLE_DEBUGGING_

// File Input Output Operations

DEFINE_IACTIVATIONFACTORY( struct file_io_statics, IFileIOStatics_iface, IID_IFileIOStatics )

DEFINE_IINSPECTABLE( file_io_statics, IFileIOStatics, struct file_io_statics, IActivationFactory_iface )

/***********************************************************************
 *      IAsyncOperation<HSTRING> IFileIOStatics::ReadTextAsync
 *      Description: Reads text from an IStorageFile* then returns all the contents of the file
 *      within a packed HSTRING.
 */
static HRESULT WINAPI
file_io_statics_ReadTextAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    IAsyncOperation_HSTRING **textOperation
) {
    HRESULT hr;
    struct file_io_read_text_options *read_text_options;

    TRACE( "iface %p, file %p, operation %p\n", iface, file, textOperation );

    //Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    read_text_options->encoding = UnicodeEncoding_Utf8;
    read_text_options->file = file;
 
    hr = async_operation_hstring_create( 
        (IUnknown *)iface, 
        (IUnknown *)read_text_options, 
        file_io_statics_ReadText,
        textOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<HSTRING> IFileIOStatics::ReadTextWithEncodingAsync
 *      Description: Reads text from an IStorageFile* then returns all the contents of the file
 *      within a packed HSTRING with the specified encoding.
 */
static HRESULT WINAPI
file_io_statics_ReadTextWithEncodingAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    UnicodeEncoding encoding,
    IAsyncOperation_HSTRING **textOperation
) {
    HRESULT hr;
    struct file_io_read_text_options *read_text_options;

    TRACE( "iface %p, file %p, encoding %d, operation %p\n", iface, file, encoding, textOperation );

    //Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    read_text_options->encoding = encoding;
    read_text_options->file = file;

    hr = async_operation_hstring_create( 
        (IUnknown *)iface, 
        (IUnknown *)read_text_options, 
        file_io_statics_ReadText, 
        textOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::WriteTextAsync
 *      Description: Overwrites the contents of an IStorageFile* with the given
 *      packed HSTRING.
 */
static HRESULT WINAPI
file_io_statics_WriteTextAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    HSTRING contents,
    IAsyncAction **textOperation
) {
    HRESULT hr;
    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, contents %s, operation %p\n", iface, file, debugstr_hstring(contents), textOperation );

    //Arguments
    if ( !file || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    write_text_options->encoding = UnicodeEncoding_Utf8;
    write_text_options->withEncoding = FALSE;
    write_text_options->file = file;
    write_text_options->contents = contents;

    hr = async_action_create( 
        (IUnknown *)iface, 
        (IUnknown *)write_text_options, 
        file_io_statics_WriteText, 
        textOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::WriteTextWithEncodingAsync
 *      Description: Overwrites the contents of an IStorageFile* with the given
 *      packed HSTRING with the specified encoding.
 */
static HRESULT WINAPI
file_io_statics_WriteTextWithEncodingAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    HSTRING contents,
    UnicodeEncoding encoding,
    IAsyncAction **textOperation
) {
    HRESULT hr;
    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, contents %s, encoding %d, operation %p\n", iface, file, debugstr_hstring(contents), encoding, textOperation );

    //Arguments
    if ( !file || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    write_text_options->encoding = encoding;
    // A UTF-8 BOM header is forcefully appended when using this routine when the client uses `UnicodeEncoding::Utf8`
    write_text_options->withEncoding = TRUE;
    write_text_options->file = file;
    write_text_options->contents = contents;

    hr = async_action_create( 
        (IUnknown *)iface, 
        (IUnknown *)write_text_options, 
        file_io_statics_WriteText, 
        textOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::AppendTextAsync
 *      Description: Appends a packed HSTRING to the contents of an IStorageFile*.
 */
static HRESULT WINAPI
file_io_statics_AppendTextAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    HSTRING contents,
    IAsyncAction **textOperation
) {
    HRESULT hr;
    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, contents %s, operation %p\n", iface, file, debugstr_hstring(contents), textOperation );

    //Arguments
    if ( !file || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    write_text_options->encoding = UnicodeEncoding_Utf8;
    // The unicode encoding of the file is auto detected instead.
    write_text_options->withEncoding = FALSE;
    write_text_options->file = file;
    write_text_options->contents = contents;

    hr = async_action_create( 
        (IUnknown *)iface, 
        (IUnknown *)write_text_options, 
        file_io_statics_AppendText, 
        textOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::AppendTextWithEncodingAsync
 *      Description: Appends a packed HSTRING with the specified encoding to
 *      the contents of an IStorageFile*.
 */
static HRESULT WINAPI
file_io_statics_AppendTextWithEncodingAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    HSTRING contents,
    UnicodeEncoding encoding,
    IAsyncAction **textOperation
) {
    HRESULT hr;
    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, contents %s, encoding %d, operation %p\n", iface, file, debugstr_hstring(contents), encoding, textOperation );

    //Arguments
    if ( !file || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    write_text_options->encoding = encoding;
    write_text_options->withEncoding = TRUE;
    write_text_options->file = file;
    write_text_options->contents = contents;

    hr = async_action_create( 
        (IUnknown *)iface, 
        (IUnknown *)write_text_options, 
        file_io_statics_AppendText, 
        textOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<IVector <HSTRING>*> IFileIOStatics::ReadLinesAsync
 *      Description: Reads the contents of an IStorageFile* line by line and appends it to
 *      an HSTRING Vector.
 */
static HRESULT WINAPI
file_io_statics_ReadLinesAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    IAsyncOperation_IVector_HSTRING **linesOperation
) {
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVector_HSTRING };
    struct file_io_read_text_options *read_text_options;

    TRACE( "iface %p, file %p, operation %p\n", iface, file, linesOperation );

    //Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !linesOperation ) return E_POINTER;

    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    read_text_options->encoding = UnicodeEncoding_Utf8;
    read_text_options->file = file;

    hr = async_operation_create( 
        (IUnknown *)iface, 
        (IUnknown *)read_text_options,
        file_io_statics_ReadLines, 
        iids, 
        (IAsyncOperation_IInspectable **)linesOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<IVector <HSTRING>*> IFileIOStatics::ReadLinesWithEncodingAsync
 *      Description: Reads the contents of an IStorageFile* line by line and appends it to
 *      an HSTRING Vector with the specified encoding.
 */
static HRESULT WINAPI
file_io_statics_ReadLinesWithEncodingAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    UnicodeEncoding encoding,
    IAsyncOperation_IVector_HSTRING **linesOperation
) {
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVector_HSTRING };
    struct file_io_read_text_options *read_text_options;

    TRACE( "iface %p, file %p, encoding %d, operation %p\n", iface, file, encoding, linesOperation );

    //Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !linesOperation ) return E_POINTER;

    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    read_text_options->encoding = encoding;
    read_text_options->file = file;

    hr = async_operation_create( 
        (IUnknown *)iface, 
        (IUnknown *)read_text_options, 
        file_io_statics_ReadLines, 
        iids, 
        (IAsyncOperation_IInspectable **)linesOperation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::WriteLinesAsync
 *      Description: Overwrites the contents of an IStorageFile* with the given
 *      HSTRING vector.
 */
static HRESULT WINAPI
file_io_statics_WriteLinesAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    IIterable_HSTRING *lines,
    IAsyncAction **operation
) {
    HRESULT hr;

    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, lines %p, operation %p\n", iface, file, lines, operation );

    //Arguments
    if ( !file || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = Unpack_IIterable_HSTRING( lines, &write_text_options->contents );
    if ( FAILED( hr ) )
    {
        free( write_text_options );
        return hr;
    }

    write_text_options->withEncoding = FALSE;
    write_text_options->encoding = UnicodeEncoding_Utf8;
    write_text_options->file = file;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, file_io_statics_WriteText, operation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::WriteLinesWithEncodingAsync
 *      Description: Overwrites the contents of an IStorageFile* with the given
 *      HSTRING vector with the specified encoding.
 */
static HRESULT WINAPI
file_io_statics_WriteLinesWithEncodingAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    IIterable_HSTRING *lines,
    UnicodeEncoding encoding,
    IAsyncAction **operation
) {
    HRESULT hr;

    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, lines %p, encoding %d, operation %p\n", iface, file, lines, encoding, operation );

    //Arguments
    if ( !file || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = Unpack_IIterable_HSTRING( lines, &write_text_options->contents );
    if ( FAILED( hr ) )
    {
        free( write_text_options );
        return hr;
    }

    write_text_options->withEncoding = TRUE;
    write_text_options->encoding = encoding;
    write_text_options->file = file;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, file_io_statics_WriteText, operation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::AppendLinesAsync
 *      Description: Appends an HSTRING vector to the contents of an IStorageFile*.
 */
static HRESULT WINAPI
file_io_statics_AppendLinesAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    IIterable_HSTRING *lines,
    IAsyncAction **operation
) {
    HRESULT hr;

    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, lines %p, operation %p\n", iface, file, lines, operation );

    //Arguments
    if ( !file || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = Unpack_IIterable_HSTRING( lines, &write_text_options->contents );
    if ( FAILED( hr ) )
    {
        free( write_text_options );
        return hr;
    }

    write_text_options->withEncoding = FALSE;
    write_text_options->encoding = UnicodeEncoding_Utf8;
    write_text_options->file = file;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, file_io_statics_AppendText, operation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::AppendLinesWithEncodingAsync
 *      Description: Appends an HSTRING vector with the given
 *      encoding to the contents of an IStorageFile*.
 */
static HRESULT WINAPI 
file_io_statics_AppendLinesWithEncodingAsync( 
    IFileIOStatics *iface, 
    IStorageFile *file, 
    IIterable_HSTRING *lines,
    UnicodeEncoding encoding, 
    IAsyncAction **operation 
) {
    HRESULT hr;

    struct file_io_write_text_options *write_text_options;

    TRACE( "iface %p, file %p, lines %p, encoding %d, operation %p\n", iface, file, lines, encoding, operation );

    //Arguments
    if ( !file || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = Unpack_IIterable_HSTRING( lines, &write_text_options->contents );
    if ( FAILED( hr ) )
    {
        free( write_text_options );
        return hr;
    }

    write_text_options->withEncoding = TRUE;
    write_text_options->encoding = encoding;
    write_text_options->file = file;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, file_io_statics_AppendText, operation );

    return hr;
}

/***********************************************************************
 *      IAsyncOperation<IBuffer *> IFileIOStatics::ReadBufferAsync
 *      Description: Reads the contents of an IStorageFile* and
 *      appends it to an IBuffer.
 */
static HRESULT WINAPI
file_io_statics_ReadBufferAsync(
    IFileIOStatics *iface,
    IStorageFile* file,
    IAsyncOperation_IBuffer **operation
) {
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IBuffer };

    TRACE( "iface %p, file %p, operation %p\n", iface, file, operation );

    //Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)file, file_io_statics_ReadBuffer, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IBuffer %p.\n", *operation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::WriteBufferAsync
 *      Description: Overwrites the contents of an IStorageFile* with
 *      the given IBuffer.
 */
static HRESULT WINAPI
file_io_statics_WriteBufferAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    IBuffer* buffer,
    IAsyncAction **operation
) {
    HRESULT hr;
    struct file_io_write_buffer_options *write_buffer_options;

    TRACE( "iface %p, file %p, buffer %p, operation %p\n", iface, file, buffer, operation );

    //Arguments
    if ( !file || !buffer ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_buffer_options = calloc( 1, sizeof(*write_buffer_options) ))) return E_OUTOFMEMORY;

    write_buffer_options->buffer = buffer;
    write_buffer_options->file = file;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_buffer_options, file_io_statics_WriteBuffer, operation );

    return hr;
}

/***********************************************************************
 *      IAsyncAction IFileIOStatics::WriteBytesAsync
 *      Description: Overwrites the contents of an IStorageFile* with
 *      the given byte byffer.
 */
static HRESULT WINAPI
file_io_statics_WriteBytesAsync(
    IFileIOStatics *iface,
    IStorageFile *file,
    UINT32 __bufferSize,
    BYTE *buffer,
    IAsyncAction **operation
) {
    HRESULT hr;
    struct file_io_write_bytes_options *write_bytes_options;

    TRACE( "iface %p, file %p, __bufferSize %d, buffer %p, operation %p\n", iface, file, __bufferSize, &buffer, operation );

    //Arguments
    if ( !file || !buffer ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_bytes_options = calloc( 1, sizeof(*write_bytes_options) ))) return E_OUTOFMEMORY;

    write_bytes_options->buffer = buffer;
    write_bytes_options->file = file;
    write_bytes_options->bufferSize = __bufferSize;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_bytes_options, file_io_statics_WriteBytes, operation );

    return hr;
}

static const struct IFileIOStaticsVtbl file_io_statics_vtbl =
{
    file_io_statics_QueryInterface,
    file_io_statics_AddRef,
    file_io_statics_Release,
    /* IInspectable methods */
    file_io_statics_GetIids,
    file_io_statics_GetRuntimeClassName,
    file_io_statics_GetTrustLevel,
    /* IFileIOStatics methods */
    file_io_statics_ReadTextAsync,
    file_io_statics_ReadTextWithEncodingAsync,
    file_io_statics_WriteTextAsync,
    file_io_statics_WriteTextWithEncodingAsync,
    file_io_statics_AppendTextAsync,
    file_io_statics_AppendTextWithEncodingAsync,
    file_io_statics_ReadLinesAsync,
    file_io_statics_ReadLinesWithEncodingAsync,
    file_io_statics_WriteLinesAsync,
    file_io_statics_WriteLinesWithEncodingAsync,
    file_io_statics_AppendLinesAsync,
    file_io_statics_AppendLinesWithEncodingAsync,
    file_io_statics_ReadBufferAsync,
    file_io_statics_WriteBufferAsync,
    file_io_statics_WriteBytesAsync
};

static struct file_io_statics file_io_statics =
{
    {&factory_vtbl},
    {&file_io_statics_vtbl},
    1,
};

IActivationFactory *file_io_factory = &file_io_statics.IActivationFactory_iface;
