/* WinRT Windows.Storage.PathIO Implementation
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

#ifndef PATH_IO_INTERNAL_H
#define PATH_IO_INTERNAL_H

#include "StorageFileInternal.h"
#include "Vector/HSTRINGVector.h"

#include "../private.h"
#include "wine/debug.h"

#define MAX_BUFFER 4096

extern struct IVector_HSTRINGVtbl hstring_vector_vtbl;

struct path_io_statics
{
    IActivationFactory IActivationFactory_iface;
    IPathIOStatics IPathIOStatics_iface;
    LONG ref;
};

/**
 * Parametizred structs
 */
struct path_io_read_text_options
{
    HSTRING absolutePath;
    UnicodeEncoding encoding;
};

struct path_io_write_text_options
{
    HSTRING absolutePath;
    HSTRING contents;
    UnicodeEncoding encoding;
};

HRESULT WINAPI path_io_statics_ReadText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI path_io_statics_WriteText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI path_io_statics_AppendText( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI path_io_statics_ReadLines( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif