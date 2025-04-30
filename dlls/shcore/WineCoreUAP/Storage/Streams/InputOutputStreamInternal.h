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

#ifndef INPUT_OUTPUT_STREAM_INTERNAL_H
#define INPUT_OUTPUT_STREAM_INTERNAL_H

#include "../../../private.h"
#include "provider.h"

#include "wine/debug.h"

#define BUFFER_SIZE 4096

extern const struct IInputStreamVtbl input_stream_vtbl;
extern const struct IOutputStreamVtbl output_stream_vtbl;
extern const struct IClosableVtbl closable_stream_vtbl;

struct closable_stream
{
    //Derivates
    IClosable IClosable_iface;
    IStream *stream;

    LONG ref;
};

struct input_stream
{
    //Derivatives
    IInputStream IInputStream_iface;

    //IClosable Derivatives
    IClosable IClosable_iface;
        IStream *stream;
        LONG closableRef;

    UINT64 *headPosition;
    UINT64 updatePos; // Workaround for when the client uses both streams at the same time

    LONG ref;
};

struct output_stream
{
    //Derivatives
    IOutputStream IOutputStream_iface;

    //IClosable Derivatives
    IClosable IClosable_iface;
        IStream *stream;
        LONG closableRef;

    UINT64 *headPosition;
    UINT64 updatePos; // Workaround for when the client uses both streams at the same time

    IAsyncOperationWithProgress_UINT32_UINT32 *currentOperation; //Flushing purposes
    
    LONG ref;
};

struct input_stream_options
{    
    IBuffer *buffer;
    UINT32 count;
    InputStreamOptions options;
};

struct input_stream *impl_from_IInputStream( IInputStream *iface );
struct output_stream *impl_from_IOutputStream( IOutputStream *iface );

HRESULT WINAPI input_stream_Read( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress );
HRESULT WINAPI output_stream_Write( IUnknown *invoker, IUnknown *param, PROPVARIANT *result, IWineAsyncOperationProgressHandler *progress );
HRESULT WINAPI output_stream_Flush( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif