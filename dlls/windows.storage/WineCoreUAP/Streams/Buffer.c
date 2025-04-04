/* WinRT Windows.Storage.Streams.Buffer Implementation
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

#include "BufferInternal.h"

_ENABLE_DEBUGGING_

// Buffer

static struct buffer_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct buffer_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{
    struct buffer_statics *impl = impl_from_IActivationFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IBufferFactory))
    {
        *out = &impl->IBufferFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IBufferStatics ))
    {
        *out = &impl->IBufferStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct buffer_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct buffer_statics *impl = impl_from_IActivationFactory( iface );
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

struct buffer *impl_from_IBuffer( IBuffer *iface )
{
    return CONTAINING_RECORD( iface, struct buffer, IBuffer_iface );
}

static HRESULT WINAPI buffer_QueryInterface( IBuffer *iface, REFIID iid, void **out )
{
    struct buffer *impl = impl_from_IBuffer( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IBuffer ))
    {
        *out = &impl->IBuffer_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IBufferByteAccess ))
    {
        *out = &impl->IBufferByteAccess_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI buffer_AddRef( IBuffer *iface )
{
    struct buffer *impl = impl_from_IBuffer( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI buffer_Release( IBuffer *iface )
{
    struct buffer *impl = impl_from_IBuffer( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );

    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );

    if (!ref) free( impl );
    return ref;
}

static HRESULT WINAPI buffer_GetIids( IBuffer *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_GetRuntimeClassName( IBuffer *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_GetTrustLevel( IBuffer *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_get_Capactiy( IBuffer *iface, UINT32 *value )
{
    struct buffer *impl = impl_from_IBuffer( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Capacity;
    return S_OK;
}

static HRESULT WINAPI buffer_get_Length( IBuffer *iface, UINT32 *value )
{
    struct buffer *impl = impl_from_IBuffer( iface );
    TRACE( "iface %p, value %p\n", iface, value );
    *value = impl->Length;
    return S_OK;
}

static HRESULT WINAPI buffer_put_Length( IBuffer *iface, UINT32 value )
{
    struct buffer *impl = impl_from_IBuffer( iface );
    TRACE( "iface %p, value %d\n", iface, value );
    impl->Length = value;
    return S_OK;
}

struct IBufferVtbl buffer_vtbl =
{
    buffer_QueryInterface,
    buffer_AddRef,
    buffer_Release,
    /* IInspectable methods */
    buffer_GetIids,
    buffer_GetRuntimeClassName,
    buffer_GetTrustLevel,
    /* IBuffer methods */
    buffer_get_Capactiy,
    buffer_get_Length,
    buffer_put_Length
};

DEFINE_IINSPECTABLE( buffer_statics, IBufferStatics, struct buffer_statics, IActivationFactory_iface )

static HRESULT WINAPI buffer_statics_CreateCopyFromMemoryBuffer( IBufferStatics *iface, IMemoryBuffer *input, IBuffer **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_statics_CreateMemoryBufferOverIBuffer( IBufferStatics *iface, IBuffer *input, IMemoryBuffer **value )
{
    FIXME( "iface %p, value %p stub!\n", iface, value );
    return E_NOTIMPL;
}

const struct IBufferStaticsVtbl buffer_statics_vtbl =
{
    buffer_statics_QueryInterface,
    buffer_statics_AddRef,
    buffer_statics_Release,
    /* IInspectable methods */
    buffer_statics_GetIids,
    buffer_statics_GetRuntimeClassName,
    buffer_statics_GetTrustLevel,
    /* IBufferStatics methods */
    buffer_statics_CreateCopyFromMemoryBuffer,
    buffer_statics_CreateMemoryBufferOverIBuffer
};

struct buffer_statics *impl_from_IBufferFactory( IBufferFactory *iface )
{
    return CONTAINING_RECORD( iface, struct buffer_statics, IBufferFactory_iface );
}

static HRESULT WINAPI buffer_factory_QueryInterface( IBufferFactory *iface, REFIID iid, void **out )
{
    struct buffer_statics *impl = impl_from_IBufferFactory( iface );

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

    if (IsEqualGUID( iid, &IID_IBufferFactory))
    {
        *out = &impl->IBufferFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IBufferStatics ))
    {
        *out = &impl->IBufferStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI buffer_factory_AddRef( IBufferFactory *iface )
{
    struct buffer_statics *impl = impl_from_IBufferFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI buffer_factory_Release( IBufferFactory *iface )
{
    struct buffer_statics *impl = impl_from_IBufferFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI buffer_factory_GetIids( IBufferFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_factory_GetRuntimeClassName( IBufferFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_factory_GetTrustLevel( IBufferFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI buffer_factory_Create( IBufferFactory *iface, UINT32 capacity, IBuffer **value )
{
    TRACE( "iface %p, capacity %d, value %p\n", iface, capacity, value );
    if ( !value ) return E_POINTER;
    return buffer_Create( capacity, value );
}

const struct IBufferFactoryVtbl buffer_factory_vtbl =
{
    buffer_factory_QueryInterface,
    buffer_factory_AddRef,
    buffer_factory_Release,
    /* IInspectable methods */
    buffer_factory_GetIids,
    buffer_factory_GetRuntimeClassName,
    buffer_factory_GetTrustLevel,
    /* IBufferFactory methods */
    buffer_factory_Create
};

static struct buffer_statics buffer_statics =
{
    {&factory_vtbl},
    {&buffer_factory_vtbl},
    {&buffer_statics_vtbl},
    1,
};

IActivationFactory *buffer_factory = &buffer_statics.IActivationFactory_iface;