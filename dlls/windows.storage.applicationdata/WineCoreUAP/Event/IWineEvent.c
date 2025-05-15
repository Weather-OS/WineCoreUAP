/* WinRT TypedEventHandler implementation
 *
 * Written by Weather
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

#include "../../private.h"

#include "wine/debug.h"
 
WINE_DEFAULT_DEBUG_CHANNEL(combase);
 
struct typed_event_handler_inspectable
{
    ITypedEventHandler_IInspectable_IInspectable ITypedEventHandler_IInspectable_IInspectable_iface;
    struct event_iids iids;
    LONG ref;
};

static inline struct typed_event_handler_inspectable *impl_from_ITypedEventHandler_IInspectable_IInspectable( ITypedEventHandler_IInspectable_IInspectable *iface )
{
    return CONTAINING_RECORD( iface, struct typed_event_handler_inspectable, ITypedEventHandler_IInspectable_IInspectable_iface );
}

static HRESULT WINAPI typed_event_handler_inspectable_QueryInterface( ITypedEventHandler_IInspectable_IInspectable *iface, REFIID iid, void **out )
{
    struct typed_event_handler_inspectable *impl = impl_from_ITypedEventHandler_IInspectable_IInspectable( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, impl->iids.event ))
    {
        *out = &impl->ITypedEventHandler_IInspectable_IInspectable_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI typed_event_handler_inspectable_AddRef( ITypedEventHandler_IInspectable_IInspectable *iface )
{
    struct typed_event_handler_inspectable *impl = impl_from_ITypedEventHandler_IInspectable_IInspectable( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI typed_event_handler_inspectable_Release( ITypedEventHandler_IInspectable_IInspectable *iface )
{
    struct typed_event_handler_inspectable *impl = impl_from_ITypedEventHandler_IInspectable_IInspectable( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI typed_event_handler_inspectable_GetIids( ITypedEventHandler_IInspectable_IInspectable *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI typed_event_handler_inspectable_GetRuntimeClassName( ITypedEventHandler_IInspectable_IInspectable *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI typed_event_handler_inspectable_GetTrustLevel( ITypedEventHandler_IInspectable_IInspectable *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}