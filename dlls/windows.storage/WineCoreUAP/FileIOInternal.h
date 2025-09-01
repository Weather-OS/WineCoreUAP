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

#ifndef FILE_IO_INTERNAL_H
#define FILE_IO_INTERNAL_H

#include "StorageFileInternal.h"

#include "../private.h"
#include "wine/debug.h"

#define MAX_BUFFER 4096

struct file_io_statics
{
    IActivationFactory IActivationFactory_iface;
    IFileIOStatics IFileIOStatics_iface;
    LONG ref;
};

/**
 * Parametized structs
 */
struct file_io_read_text_options
{
    IStorageFile *file;
    UnicodeEncoding encoding;
};

struct file_io_write_text_options
{
    HSTRING contents;
    BOOLEAN withEncoding;
    IStorageFile *file;
    UnicodeEncoding encoding;
};

struct file_io_write_buffer_options
{
    IStorageFile *file;
    IBuffer *buffer;
};

struct file_io_write_bytes_options
{
    IStorageFile *file;
    BYTE *buffer;
    UINT32 bufferSize;
};

HRESULT WINAPI Unpack_IIterable_HSTRING( IN IIterable_HSTRING *iter, OUT HSTRING *out );

HRESULT WINAPI file_io_statics_ReadText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI file_io_statics_WriteText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI file_io_statics_AppendText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI file_io_statics_ReadLines( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI file_io_statics_ReadBuffer( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI file_io_statics_WriteBuffer( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI file_io_statics_WriteBytes( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif