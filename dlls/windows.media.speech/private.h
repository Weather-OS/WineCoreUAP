/* WinRT Windows.Media.Speech private header
 *
 * Copyright 2022 Bernhard Kölbl
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

#ifndef __WINE_WINDOWS_MEDIA_SPEECH_PRIVATE_H
#define __WINE_WINDOWS_MEDIA_SPEECH_PRIVATE_H

#include <stdarg.h>

#define COBJMACROS
#include "corerror.h"
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "objbase.h"

#include "activation.h"

#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#include "windows.foundation.h"
#define WIDL_using_Windows_Globalization
#include "windows.globalization.h"
#define WIDL_using_Windows_Media_SpeechSynthesis
#include "windows.media.speechsynthesis.h"
#define WIDL_using_Windows_Media_SpeechRecognition
#include "windows.media.speechrecognition.h"

#include "wine/list.h"

#include "async_private.h"

/*
 *
 * Windows.Media.SpeechRecognition
 *
 */

extern IActivationFactory *listconstraint_factory;
extern IActivationFactory *recognizer_factory;

/*
 *
 * Windows.Media.SpeechSynthesis
 *
 */

extern IActivationFactory *synthesizer_factory;



struct vector_iids
{
    const GUID *iterable;
    const GUID *iterator;
    const GUID *vector;
    const GUID *view;
};

HRESULT async_action_create( IUnknown *invoker, async_operation_callback callback, IAsyncAction **out );
HRESULT async_operation_inspectable_create( const GUID *iid, IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                            IAsyncOperation_IInspectable **out );

HRESULT typed_event_handlers_append( struct list *list, ITypedEventHandler_IInspectable_IInspectable *handler, EventRegistrationToken *token );
HRESULT typed_event_handlers_remove( struct list *list, EventRegistrationToken *token );
HRESULT typed_event_handlers_notify( struct list *list, IInspectable *sender, IInspectable *args );
HRESULT typed_event_handlers_clear( struct list* list );

HRESULT vector_hstring_create( IVector_HSTRING **out );
HRESULT vector_hstring_create_copy( IIterable_HSTRING *iterable, IVector_HSTRING **out );
HRESULT vector_inspectable_create( const struct vector_iids *iids, IVector_IInspectable **out );

#define DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from, iface_mem, expr )             \
    static inline impl_type *impl_from( iface_type *iface )                                        \
    {                                                                                              \
        return CONTAINING_RECORD( iface, impl_type, iface_mem );                                   \
    }                                                                                              \
    static HRESULT WINAPI pfx##_QueryInterface( iface_type *iface, REFIID iid, void **out )        \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_QueryInterface( (IInspectable *)(expr), iid, out );                    \
    }                                                                                              \
    static ULONG WINAPI pfx##_AddRef( iface_type *iface )                                          \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_AddRef( (IInspectable *)(expr) );                                      \
    }                                                                                              \
    static ULONG WINAPI pfx##_Release( iface_type *iface )                                         \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_Release( (IInspectable *)(expr) );                                     \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetIids( iface_type *iface, ULONG *iid_count, IID **iids )         \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetIids( (IInspectable *)(expr), iid_count, iids );                    \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetRuntimeClassName( iface_type *iface, HSTRING *class_name )      \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetRuntimeClassName( (IInspectable *)(expr), class_name );             \
    }                                                                                              \
    static HRESULT WINAPI pfx##_GetTrustLevel( iface_type *iface, TrustLevel *trust_level )        \
    {                                                                                              \
        impl_type *impl = impl_from( iface );                                                      \
        return IInspectable_GetTrustLevel( (IInspectable *)(expr), trust_level );                  \
    }
#define DEFINE_IINSPECTABLE( pfx, iface_type, impl_type, base_iface )                              \
    DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, &impl->base_iface )
#define DEFINE_IINSPECTABLE_OUTER( pfx, iface_type, impl_type, outer_iface )                       \
    DEFINE_IINSPECTABLE_( pfx, iface_type, impl_type, impl_from_##iface_type, iface_type##_iface, impl->outer_iface )

#endif
