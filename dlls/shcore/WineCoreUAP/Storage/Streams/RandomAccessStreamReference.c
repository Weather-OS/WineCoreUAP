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

#include "RandomAccessStreamReferenceInternal.h"

_ENABLE_DEBUGGING_

static struct random_access_stream_reference_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct random_access_stream_reference_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct random_access_stream_reference_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IRandomAccessStreamReferenceStatics))
    {
        *out = &impl->IRandomAccessStreamReferenceStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct random_access_stream_reference_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct random_access_stream_reference_statics *impl = impl_from_IActivationFactory( iface );
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

DEFINE_IINSPECTABLE( random_access_stream_reference_statics, IRandomAccessStreamReferenceStatics, struct random_access_stream_reference_statics, IActivationFactory_iface )

static HRESULT WINAPI random_access_stream_reference_statics_CreateFromFile( IRandomAccessStreamReferenceStatics *iface, IStorageFile *file, IRandomAccessStreamReference **stream_reference )
{
    HRESULT hr;
    HSTRING path;

    IStorageItem *item;

    TRACE( "iface %p, file %p, stream_reference %p.\n", iface, file, stream_reference );

    // Arguments
    if ( !file ) return E_INVALIDARG;
    if ( !stream_reference ) return E_POINTER;

    hr = IStorageFile_QueryInterface( file, &IID_IStorageItem, (void **)&item );
    if ( FAILED( hr ) ) return hr;

    IStorageItem_get_Path( item, &path );

    hr = random_access_stream_reference_CreateStreamReference( path, FileAccessMode_ReadWrite, stream_reference );

    IStorageItem_Release( item );
    WindowsDeleteString( path );

    return hr;
}

static HRESULT WINAPI random_access_stream_reference_statics_CreateFromUri( IRandomAccessStreamReferenceStatics *iface, IUriRuntimeClass *uri, IRandomAccessStreamReference **stream_reference )
{
    FIXME( "iface %p uri %p reference %p stub!", iface, uri, stream_reference );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_reference_statics_CreateFromStream( IRandomAccessStreamReferenceStatics *iface, IRandomAccessStream *stream, IRandomAccessStreamReference **stream_reference )
{
    FIXME( "iface %p stream %p reference %p stub!", iface, stream, stream_reference );
    return E_NOTIMPL;
}

const struct IRandomAccessStreamReferenceStaticsVtbl random_access_stream_reference_statics_vtbl =
{
    random_access_stream_reference_statics_QueryInterface,
    random_access_stream_reference_statics_AddRef,
    random_access_stream_reference_statics_Release,
    /* IInspectable methods */
    random_access_stream_reference_statics_GetIids,
    random_access_stream_reference_statics_GetRuntimeClassName,
    random_access_stream_reference_statics_GetTrustLevel,
    /* IRandomAccessStreamReferenceStatics methods */
    random_access_stream_reference_statics_CreateFromFile,
    random_access_stream_reference_statics_CreateFromUri,
    random_access_stream_reference_statics_CreateFromStream
};

struct random_access_stream_reference *impl_from_IRandomAccessStreamReference( IRandomAccessStreamReference *iface )
{
    return CONTAINING_RECORD( iface, struct random_access_stream_reference, IRandomAccessStreamReference_iface );
}

static HRESULT WINAPI random_access_stream_reference_QueryInterface( IRandomAccessStreamReference *iface, REFIID iid, void **out )
{
    struct random_access_stream_reference *impl = impl_from_IRandomAccessStreamReference( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IRandomAccessStreamReference ))
    {
        *out = &impl->IRandomAccessStreamReference_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI random_access_stream_reference_AddRef( IRandomAccessStreamReference *iface )
{
    struct random_access_stream_reference *impl = impl_from_IRandomAccessStreamReference( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI random_access_stream_reference_Release( IRandomAccessStreamReference *iface )
{
    struct random_access_stream_reference *impl = impl_from_IRandomAccessStreamReference( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI random_access_stream_reference_GetIids( IRandomAccessStreamReference *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_reference_GetRuntimeClassName( IRandomAccessStreamReference *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_reference_GetTrustLevel( IRandomAccessStreamReference *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI random_access_stream_reference_OpenReadAsync( IRandomAccessStreamReference *iface, IAsyncOperation_IRandomAccessStreamWithContentType **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IRandomAccessStream };

    TRACE( "iface %p, operation %p.\n", iface, operation );

    // Arguments
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, NULL, random_access_stream_reference_CreateReadOnlyStream, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IRandomAccessStream %p.\n", *operation );
    
    return hr;
}

const struct IRandomAccessStreamReferenceVtbl random_access_stream_reference_vtbl =
{
    random_access_stream_reference_QueryInterface,
    random_access_stream_reference_AddRef,
    random_access_stream_reference_Release,
    /* IInspectable methods */
    random_access_stream_reference_GetIids,
    random_access_stream_reference_GetRuntimeClassName,
    random_access_stream_reference_GetTrustLevel,
    /* IRandomAccessStreamReference methods */
    random_access_stream_reference_OpenReadAsync
};

static struct random_access_stream_reference_statics random_access_stream_reference_statics =
{
    {&factory_vtbl},
    {&random_access_stream_reference_statics_vtbl},
    1,
};

IActivationFactory *random_access_stream_reference_factory = &random_access_stream_reference_statics.IActivationFactory_iface;