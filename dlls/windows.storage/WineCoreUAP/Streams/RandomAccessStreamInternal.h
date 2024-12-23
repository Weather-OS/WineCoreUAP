/* WinRT Windows.Storage.Streams.RandomAccessStream Implementation
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

#ifndef RANDOM_ACCESS_STREAM_INTERNAL_H
#define RANDOM_ACCESS_STREAM_INTERNAL_H

#include "../../private.h"
#include "provider.h"

#include "wine/debug.h"

#include "InputOutputStreamInternal.h"

extern const struct IRandomAccessStreamVtbl random_access_stream_vtbl;

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
        HANDLE stream;
        UINT64 streamSize;
        LONG closableRef;

    UINT64 Position;
    BOOLEAN CanRead;
    BOOLEAN CanWrite;

    LONG ref;
};

HRESULT WINAPI random_access_stream_statics_Copy( IInputStream *source, IOutputStream *destination, UINT64 bytesToCopy, IAsyncOperationWithProgress_UINT64_UINT64 **operation );


#endif