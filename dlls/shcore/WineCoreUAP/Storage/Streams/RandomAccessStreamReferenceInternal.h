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

#ifndef RANDOM_ACCESS_STREAM_REFERENCE_INTERNAL_H
#define RANDOM_ACCESS_STREAM_REFERENCE_INTERNAL_H

#include "../../../private.h"
#include "provider.h"

#include "wine/debug.h"

#include <shlwapi.h>

extern const struct IRandomAccessStreamWithContentTypeVtbl random_access_stream_with_content_type_vtbl;
extern const struct IRandomAccessStreamReferenceVtbl random_access_stream_reference_vtbl;
extern const struct IClosableVtbl closable_random_access_stream_vtbl;
extern const struct IContentTypeProviderVtbl content_type_provider_vtbl;

//it's easier to import this through extern.
extern HRESULT WINAPI SHCreateStreamOnFileEx(const WCHAR *path, DWORD mode, DWORD attributes, BOOL create, IStream *template, IStream **ret);

struct random_access_stream_reference_statics
{
    //Derivatives
    IActivationFactory IActivationFactory_iface;
    IRandomAccessStreamReferenceStatics IRandomAccessStreamReferenceStatics_iface;
    LONG ref;
};

struct random_access_stream_reference
{
    //Derivatives
    IRandomAccessStreamReference IRandomAccessStreamReference_iface;
    BOOLEAN canRead;
    BOOLEAN canWrite;
    HSTRING handlePath;
    HSTRING contentType; //For IContentTypeProvider
    UINT64 streamSize;
    LONG ref;
};

struct random_access_stream_reference *impl_from_IRandomAccessStreamReference( IRandomAccessStreamReference *iface );

HRESULT WINAPI random_access_stream_reference_CreateStream( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );
HRESULT WINAPI random_access_stream_reference_CreateStreamReference( HSTRING path, FileAccessMode accessMode, IRandomAccessStreamReference **value );
HRESULT WINAPI random_access_stream_reference_CreateReadOnlyStream( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

#endif