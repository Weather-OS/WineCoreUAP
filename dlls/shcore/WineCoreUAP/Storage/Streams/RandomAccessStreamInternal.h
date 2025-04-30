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

#ifndef RANDOM_ACCESS_STREAM_INTERNAL_H
#define RANDOM_ACCESS_STREAM_INTERNAL_H

#include "../../../private.h"
#include "provider.h"

#include "wine/debug.h"

#include "InputOutputStreamInternal.h"

extern const struct IRandomAccessStreamVtbl random_access_stream_vtbl;

#define BUFFER_SIZE 4096

struct random_access_stream_statics
{
    //Derivatives
    IActivationFactory IActivationFactory_iface;
    IRandomAccessStreamStatics IRandomAccessStreamStatics_iface;
    LONG ref;
};

struct random_access_stream
{
    //Derivatives
    IRandomAccessStream IRandomAccessStream_iface;

    //IClosable Derivatives
    IClosable IClosable_iface;
        IStream *stream;
        LONG closableRef;

    UINT64 Position;
    BOOLEAN CanRead;
    BOOLEAN CanWrite;

    LONG ref;
};


struct random_access_stream_with_content_type
{
    //Derivatives
    IRandomAccessStreamWithContentType IRandomAccessStreamWithContentType_iface;

    //IRandomAccessStream Derivatives
    IRandomAccessStream IRandomAccessStream_iface;
        //IClosable Derivatives
        IClosable IClosable_iface;
            IStream *stream;
            LONG closableRef;

        UINT64 Position;
        BOOLEAN CanRead;
        BOOLEAN CanWrite;
        LONG randomAccessStreamRef;

    //IContentTypeProvider Derivatives
    IContentTypeProvider IContentTypeProvider_iface;
        HSTRING ContentType;
        LONG contentTypeRef;

    LONG ref;
};

HRESULT WINAPI random_access_stream_statics_Copy( IInputStream *source, IOutputStream *destination, UINT64 bytesToCopy, IAsyncOperationWithProgress_UINT64_UINT64 **operation );


#endif