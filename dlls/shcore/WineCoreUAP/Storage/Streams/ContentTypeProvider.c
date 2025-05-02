/* WinRT Windows.Storage.Streams.IContentTypeProvider Implementation
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

#include "ContentTypeProviderInternal.h"

_ENABLE_DEBUGGING_

// Content Type Provider

static struct content_type_provider *impl_from_IContentTypeProvider( IContentTypeProvider *iface )
{
    return CONTAINING_RECORD( iface, struct content_type_provider, IContentTypeProvider_iface );
}

static HRESULT WINAPI content_type_provider_QueryInterface( IContentTypeProvider *iface, REFIID iid, void **out )
{
    struct content_type_provider *impl = impl_from_IContentTypeProvider( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IContentTypeProvider ))
    {
        *out = &impl->IContentTypeProvider_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI content_type_provider_AddRef( IContentTypeProvider *iface )
{
    struct content_type_provider *impl = impl_from_IContentTypeProvider( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI content_type_provider_Release( IContentTypeProvider *iface )
{
    struct content_type_provider *impl = impl_from_IContentTypeProvider( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI content_type_provider_GetIids( IContentTypeProvider *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI content_type_provider_GetRuntimeClassName( IContentTypeProvider *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI content_type_provider_GetTrustLevel( IContentTypeProvider *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI content_type_provider_get_ContentType( IContentTypeProvider *iface, HSTRING *value )
{
    struct content_type_provider *impl = impl_from_IContentTypeProvider( iface );
    TRACE( "iface %p, value %p.\n", iface, value );
    return WindowsDuplicateString( impl->ContentType, value );
}

const struct IContentTypeProviderVtbl content_type_provider_vtbl =
{
    content_type_provider_QueryInterface,
    content_type_provider_AddRef,
    content_type_provider_Release,
    /* IInspectable methods */
    content_type_provider_GetIids,
    content_type_provider_GetRuntimeClassName,
    content_type_provider_GetTrustLevel,
    /* IContentTypeProvider methods */
    content_type_provider_get_ContentType
};