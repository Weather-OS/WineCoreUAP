/* WinRT Windows.Storage->Vector* Implementation
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

#include "private.h"

#ifndef STORAGE_FOLDER_VECTOR_DEFINITIONS
#define STORAGE_FOLDER_VECTOR_DEFINITIONS

#define DEFINE_IITERATOR( typename, interface_type, element_type )  \
    struct typename##_iterator                                           \
    {                                                                    \
        IIterator_##interface_type IIterator_##interface_type##_iface;   \
        IIterable_##interface_type IIterable_##interface_type##_iface;   \
                                                                         \
        UINT32 index;                                                    \
        UINT32 size;                                                     \
        element_type* elements;                                          \
                                                                         \
        LONG ref;                                                        \
    };                                                                   \
                                                                                                                       \
    static struct typename##_iterator *impl_from_IIterator_##interface_type( IIterator_##interface_type *iface )       \
    {                                                                                                                  \
        return CONTAINING_RECORD( iface, struct typename##_iterator, IIterator_##interface_type##_iface );             \
    }                                                                                                                  \
                                                                                                                       \
    static HRESULT WINAPI typename##_iterator_QueryInterface( IIterator_##interface_type *iface, REFIID iid, void **out ) \
    {                                                                                                   \
        struct typename##_iterator *impl = impl_from_IIterator_##interface_type( iface );               \
                                                                                                        \
        TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );                       \
                                                                                                        \
        if (IsEqualGUID( iid, &IID_IUnknown ) ||                                                        \
            IsEqualGUID( iid, &IID_IInspectable ) ||                                                    \
            IsEqualGUID( iid, &IID_IAgileObject ) ||                                                    \
            IsEqualGUID( iid, &IID_IIterator_##interface_type ))                                        \
        {                                                                                               \
            IInspectable_AddRef( (*out = &impl->IIterator_##interface_type##_iface) );                  \
            return S_OK;                                                                                \
        }                                                                                               \
                                                                                                        \
        FIXME( "%s not implemented, returning E_NOINTERFACE##.\n", debugstr_guid( iid ) );              \
        *out = NULL;                                                                                    \
        return E_NOINTERFACE;                                                                           \
    }                                                                                                   \
                                                                                                        \
    static ULONG WINAPI typename##_iterator_AddRef( IIterator_##interface_type *iface )                 \
    {                                                                                                   \
        struct typename##_iterator *impl = impl_from_IIterator_##interface_type( iface );               \
        ULONG ref = InterlockedIncrement( &impl->ref );                                                 \
        TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );                                  \
        return ref;                                                                                     \
    }                                                                                                   \
                                                                                                        \
    static ULONG WINAPI typename##_iterator_Release( IIterator_##interface_type *iface )                \
    {                                                                                                   \
        struct typename##_iterator *impl = impl_from_IIterator_##interface_type( iface );               \
        ULONG ref = InterlockedDecrement( &impl->ref );                                                 \
                                                                                                        \
        TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );                                  \
                                                                                                        \
        if (!ref)                                                                                       \
        {                                                                                               \
            free( impl );                                                                               \
        }                                                                                               \
                                                                                                        \
        return ref;                                                                                     \
    }                                                                                                   \
                                                                                                        \
    static HRESULT WINAPI typename##_iterator_GetIids( IIterator_##interface_type *iface, ULONG *iid_count, IID **iids )            \
    {                                                                                                                               \
        FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );                                                 \
        return E_NOTIMPL;                                                                                                           \
    }                                                                                                                               \
                                                                                                                                    \
    static HRESULT WINAPI typename##_iterator_GetRuntimeClassName( IIterator_##interface_type *iface, HSTRING *class_name )                     \
    {                                                                                                                                           \
        FIXME( "iface %p, class_name %p stub!\n", iface, class_name );                                                                          \
        return E_NOTIMPL;                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                                \
    static HRESULT WINAPI typename##_iterator_GetTrustLevel( IIterator_##interface_type *iface, TrustLevel *trust_level )                       \
    {                                                                                                                                           \
        FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );                                                                        \
        return E_NOTIMPL;                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                 \
    static HRESULT WINAPI typename##_iterator_get_Current( IIterator_##interface_type *iface, element_type* value )              \
    {                                                                                                                            \
        struct typename##_iterator *impl = impl_from_IIterator_##interface_type(iface);                                          \
        TRACE("iface %p, value %p, index %d.\n", iface, value, impl->index);                                                                            \
                                                                                                                                 \
        *value = impl->elements[impl->index];                                                                                    \
        return S_OK;                                                                                                             \
    }                                                                                                                            \
                                                                                                                                 \
    static HRESULT WINAPI typename##_iterator_get_HasCurrent( IIterator_##interface_type *iface, boolean *value )                \
    {                                                                                                                            \
        struct typename##_iterator *impl = impl_from_IIterator_##interface_type(iface);                                          \
                                                                                                                                 \
        TRACE("iface %p, value %p.\n", iface, value);                                                                            \
                                                                                                                                 \
        *value = impl->index < impl->size;                                                                                       \
        return S_OK;                                                                                                             \
    }                                                                                                                            \
                                                                                                                                 \
    static HRESULT WINAPI typename##_iterator_MoveNext( IIterator_##interface_type *iface, boolean *value )                      \
    {                                                                                                                            \
        struct typename##_iterator *impl = impl_from_IIterator_##interface_type(iface);                                          \
                                                                                                                                 \
        TRACE("iface %p, value %p.\n", iface, value);                                                                            \
                                                                                                                                 \
        if (impl->index < impl->size) impl->index++;                                                                             \
        return IIterator_##interface_type##_get_HasCurrent(iface, value);                                                        \
    }                                                                                                                            \
                                                                                                                                 \
    static HRESULT WINAPI typename##_iterator_GetMany( IIterator_##interface_type *iface, UINT32 items_size,       \
                                                    element_type* items, UINT *count )                             \
    {                                                                                                              \
        struct typename##_iterator *impl = impl_from_IIterator_##interface_type(iface);                            \
        UINT32 i;                                                                                                  \
        UINT32 start_index = impl->index;                                                                          \
                                                                                                                   \
        TRACE("iface %p, items_size %u, items %p, count %p.\n", iface, items_size, items, count);                  \
                                                                                                                   \
        if (start_index >= impl->size) return E_BOUNDS;                                                            \
                                                                                                                   \
        for (i = start_index; i < impl->size; ++i)                                                                 \
        {                                                                                                          \
            if (i - start_index >= items_size) break;                                                              \
            items[i - start_index] = impl->elements[i];                                                            \
        }                                                                                                          \
        *count = i - start_index;                                                                                  \
                                                                                                                   \
        return S_OK;                                                                                               \
    }                                                                                                              \
                                                                                                                   \
    static struct IIterator_##interface_type##Vtbl typename##_iterator_vtbl =  \
    {                                                                 \
        /* IUnknown methods */                                        \
        typename##_iterator_QueryInterface,                           \
        typename##_iterator_AddRef,                                   \
        typename##_iterator_Release,                                  \
        /* IInspectable methods */                                    \
        typename##_iterator_GetIids,                                  \
        typename##_iterator_GetRuntimeClassName,                      \
        typename##_iterator_GetTrustLevel,                            \
        /* IIterator<interface_type##> methods */                     \
        typename##_iterator_get_Current,                              \
        typename##_iterator_get_HasCurrent,                           \
        typename##_iterator_MoveNext,                                 \
        typename##_iterator_GetMany,                                  \
    };                                                                

#define DEFINE_IITERABLE( typename, interface_type, element_type )  \
    DEFINE_IITERATOR( typename, interface_type, element_type )      \
                                                                    \
    static struct typename##_iterator *impl_from_IIterable_##interface_type( IIterable_##interface_type *iface )       \
    {                                                                                                                  \
        return CONTAINING_RECORD( iface, struct typename##_iterator, IIterable_##interface_type##_iface );             \
    }                                                                                                                  \
                                                                                                                       \
    static HRESULT WINAPI typename##_iterable_QueryInterface( IIterable_##interface_type *iface, REFIID iid, void **out ) \
    {                                                                                                   \
        struct typename##_iterator *impl = impl_from_IIterable_##interface_type( iface );               \
                                                                                                        \
        TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );                       \
                                                                                                        \
        if (IsEqualGUID( iid, &IID_IUnknown ) ||                                                        \
            IsEqualGUID( iid, &IID_IInspectable ) ||                                                    \
            IsEqualGUID( iid, &IID_IAgileObject ) ||                                                    \
            IsEqualGUID( iid, &IID_IIterable_##interface_type ))                                        \
        {                                                                                               \
            IInspectable_AddRef( (*out = &impl->IIterable_##interface_type##_iface) );                  \
            return S_OK;                                                                                \
        }                                                                                               \
                                                                                                        \
        FIXME( "%s not implemented, returning E_NOINTERFACE##.\n", debugstr_guid( iid ) );              \
        *out = NULL;                                                                                    \
        return E_NOINTERFACE;                                                                           \
    }                                                                                                   \
                                                                                                        \
    static ULONG WINAPI typename##_iterable_AddRef( IIterable_##interface_type *iface )                 \
    {                                                                                                   \
        struct typename##_iterator *impl = impl_from_IIterable_##interface_type( iface );               \
        ULONG ref = InterlockedIncrement( &impl->ref );                                                 \
        TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );                                  \
        return ref;                                                                                     \
    }                                                                                                   \
                                                                                                        \
    static ULONG WINAPI typename##_iterable_Release( IIterable_##interface_type *iface )                \
    {                                                                                                   \
        struct typename##_iterator *impl = impl_from_IIterable_##interface_type( iface );               \
        ULONG ref = InterlockedDecrement( &impl->ref );                                                 \
                                                                                                        \
        TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );                                  \
                                                                                                        \
        if (!ref)                                                                                       \
        {                                                                                               \
            free( impl );                                                                               \
        }                                                                                               \
                                                                                                        \
        return ref;                                                                                     \
    }                                                                                                   \
                                                                                                        \
    static HRESULT WINAPI typename##_iterable_GetIids( IIterable_##interface_type *iface, ULONG *iid_count, IID **iids )            \
    {                                                                                                                               \
        FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );                                                 \
        return E_NOTIMPL;                                                                                                           \
    }                                                                                                                               \
                                                                                                                                    \
    static HRESULT WINAPI typename##_iterable_GetRuntimeClassName( IIterable_##interface_type *iface, HSTRING *class_name )                     \
    {                                                                                                                                           \
        FIXME( "iface %p, class_name %p stub!\n", iface, class_name );                                                                          \
        return E_NOTIMPL;                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                                \
    static HRESULT WINAPI typename##_iterable_GetTrustLevel( IIterable_##interface_type *iface, TrustLevel *trust_level )                       \
    {                                                                                                                                           \
        FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );                                                                        \
        return E_NOTIMPL;                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                                     \
    static HRESULT WINAPI typename##_iterable_First( IIterable_##interface_type *iface, IIterator_##interface_type **value )                         \
    {                                                                                                                                                \
        struct typename##_iterator *impl = impl_from_IIterable_##interface_type(iface);                                                              \
                                                                                                                                                     \
        TRACE("iface %p, value %p.\n", iface, value);                                                                                                \
                                                                                                                                                     \
        *value = &impl->IIterator_##interface_type##_iface;                                                                                          \
        return S_OK;                                                                                                                                 \
    }                                                                                                                                                \
                                                                                                                                                     \
    static struct IIterable_##interface_type##Vtbl typename##_iterable_vtbl =  \
    {                                                                 \
        typename##_iterable_QueryInterface,                           \
        typename##_iterable_AddRef,                                   \
        typename##_iterable_Release,                                  \
        /* IInspectable methods */                                    \
        typename##_iterable_GetIids,                                  \
        typename##_iterable_GetRuntimeClassName,                      \
        typename##_iterable_GetTrustLevel,                            \
        /* IIterable<interface_type##> methods */                     \
        typename##_iterable_First                                     \
    };                                                                
    
#define DEFINE_VECTOR_VIEW( typename, interface_type, element_type ) \
    DEFINE_IITERABLE( typename, interface_type, element_type )              \
    struct typename##_vector_view                                           \
    {                                                                       \
        IVectorView_##interface_type IVectorView_##interface_type##_iface;  \
                                                                            \
        IIterator_##interface_type IIterator_##interface_type##_iface;      \
            IIterable_##interface_type IIterable_##interface_type##_iface;  \
            UINT32 index;                                                   \
            UINT32 size;                                                    \
            element_type* elements;                                         \
            LONG iterableRef;                                               \
                                                                            \
        struct vector_iids iids;                                            \
        LONG ref;                                                           \
    };                                                                      \
                                                                            \
    static struct typename##_vector_view *impl_from_IVectorView_##interface_type( IVectorView_##interface_type *iface )\
    {                                                                                                                  \
        return CONTAINING_RECORD( iface, struct typename##_vector_view, IVectorView_##interface_type##_iface );        \
    }                                                                                                                  \
                                                                                                                       \
    static HRESULT WINAPI typename##_vector_view_QueryInterface( IVectorView_##interface_type *iface, REFIID iid, void **out ) \
    {                                                                                                   \
        struct typename##_vector_view *impl = impl_from_IVectorView_##interface_type( iface );          \
                                                                                                        \
        TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );                       \
                                                                                                        \
        if (IsEqualGUID( iid, &IID_IUnknown ) ||                                                        \
            IsEqualGUID( iid, &IID_IInspectable ) ||                                                    \
            IsEqualGUID( iid, &IID_IAgileObject ) ||                                                    \
            IsEqualGUID( iid, &IID_IVectorView_##interface_type ))                                      \
        {                                                                                               \
            IInspectable_AddRef( (*out = &impl->IVectorView_##interface_type##_iface) );                \
            return S_OK;                                                                                \
        }                                                                                               \
                                                                                                        \
        if ( IsEqualGUID( iid, &IID_IIterable_##interface_type ) )                                      \
        {                                                                                               \
            IInspectable_AddRef( (*out = &impl->IIterable_##interface_type##_iface) );                  \
            return S_OK;                                                                                \
        }                                                                                               \
                                                                                                        \
        FIXME( "%s not implemented, returning E_NOINTERFACE##.\n", debugstr_guid( iid ) );         \
        *out = NULL;                                                                                    \
        return E_NOINTERFACE;                                                                      \
    }                                                                                                   \
                                                                                                        \
    static ULONG WINAPI typename##_vector_view_AddRef( IVectorView_##interface_type *iface )            \
    {                                                                                                   \
        struct typename##_vector_view *impl = impl_from_IVectorView_##interface_type( iface );          \
        ULONG ref = InterlockedIncrement( &impl->ref );                                                 \
        TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );                                  \
        return ref;                                                                                     \
    }                                                                                                   \
                                                                                                        \
    static ULONG WINAPI typename##_vector_view_Release( IVectorView_##interface_type *iface )           \
    {                                                                                                   \
        struct typename##_vector_view *impl = impl_from_IVectorView_##interface_type( iface );          \
        ULONG ref = InterlockedDecrement( &impl->ref );                                                 \
                                                                                                        \
        TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );                                  \
                                                                                                        \
        if (!ref)                                                                                       \
        {                                                                                               \
            free( impl );                                                                               \
        }                                                                                               \
                                                                                                        \
        return ref;                                                                                     \
    }                                                                                                   \
                                                                                                        \
    static HRESULT WINAPI typename##_vector_view_GetIids( IVectorView_##interface_type *iface, ULONG *iid_count, IID **iids )       \
    {                                                                                                                               \
        FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );                                                 \
        return E_NOTIMPL;                                                                                                           \
    }                                                                                                                               \
                                                                                                                                    \
    static HRESULT WINAPI typename##_vector_view_GetRuntimeClassName( IVectorView_##interface_type *iface, HSTRING *class_name )                \
    {                                                                                                                                           \
        FIXME( "iface %p, class_name %p stub!\n", iface, class_name );                                                                          \
        return E_NOTIMPL;                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                                \
    static HRESULT WINAPI typename##_vector_view_GetTrustLevel( IVectorView_##interface_type *iface, TrustLevel *trust_level )                  \
    {                                                                                                                                           \
        FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );                                                                        \
        return E_NOTIMPL;                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                                \
    static HRESULT WINAPI typename##_vector_view_GetAt( IVectorView_##interface_type *iface, UINT32 index, element_type* value )                \
    {                                                                                                                                           \
        struct typename##_vector_view *impl = impl_from_IVectorView_##interface_type( iface );                                                  \
        TRACE( "iface %p, value %p.\n", iface, value );                                                                                         \
        impl->index = index;                                                                                                                    \
        return IIterator_##interface_type##_get_Current( &impl->IIterator_##interface_type##_iface, value );                                    \
    }                                                                                                                                           \
                                                                                                                                                \
    static HRESULT WINAPI typename##_vector_view_get_Size( IVectorView_##interface_type *iface, UINT32 *value )         \
    {                                                                                                                   \
        struct typename##_vector_view *impl = impl_from_IVectorView_##interface_type( iface );                          \
                                                                                                                        \
        TRACE( "iface %p, value %p.\n", iface, value );                                                                 \
                                                                                                                        \
        *value = impl->size;                                                                                            \
        return S_OK;                                                                                                    \
    }                                                                                                                   \
                                                                                                                        \
    static HRESULT WINAPI typename##_vector_view_IndexOf( IVectorView_##interface_type *iface, element_type element,        \
                                               UINT32 *index, BOOLEAN *found )                                              \
    {                                                                                                                       \
        struct typename##_vector_view *impl = impl_from_IVectorView_##interface_type( iface );                              \
        ULONG i;                                                                                                            \
                                                                                                                            \
        TRACE( "iface %p, element %p, index %p, found %p.\n", iface, element, index, found );                               \
                                                                                                                            \
        for (i = 0; i < impl->size; ++i) if (impl->elements[i] == element) break;                                           \
        if ((*found = (i < impl->size))) *index = i;                                                                        \
        else *index = 0;                                                                                                    \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_view_GetMany( IVectorView_##interface_type *iface, UINT32 start_index,          \
                                               UINT32 items_size, element_type* items, UINT *count )                        \
    {                                                                                                                       \
        struct typename##_vector_view *impl = impl_from_IVectorView_##interface_type( iface );                              \
        UINT32 i;                                                                                                           \
                                                                                                                            \
        TRACE( "iface %p, start_index %u, items_size %u, items %p, count %p.\n",                                            \
               iface, start_index, items_size, items, count );                                                              \
                                                                                                                            \
        if (start_index >= impl->size) return E_BOUNDS;                                                                     \
                                                                                                                            \
        for (i = start_index; i < impl->size; ++i)                                                                          \
        {                                                                                                                   \
            if (i - start_index >= items_size) break;                                                                       \
            items[i - start_index] = impl->elements[i];                                                                     \
        }                                                                                                                   \
        *count = i - start_index;                                                                                           \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static struct IVectorView_##interface_type##Vtbl typename##_vector_view_vtbl =   \
    {                                                                       \
        typename##_vector_view_QueryInterface,                              \
        typename##_vector_view_AddRef,                                      \
        typename##_vector_view_Release,                                     \
        /* IInspectable methods */                                          \
        typename##_vector_view_GetIids,                                     \
        typename##_vector_view_GetRuntimeClassName,                         \
        typename##_vector_view_GetTrustLevel,                               \
        /* IVectorView<interface_type##> methods */                         \
        typename##_vector_view_GetAt,                                       \
        typename##_vector_view_get_Size,                                    \
        typename##_vector_view_IndexOf,                                     \
        typename##_vector_view_GetMany,                                     \
    };

#define DEFINE_VECTOR( typename, interface_type, element_type ) \
    DEFINE_VECTOR_VIEW( typename, interface_type, element_type )            \
    struct typename##_vector                                                \
    {                                                                       \
        IVector_##interface_type IVector_##interface_type##_iface;          \
                                                                            \
        IIterator_##interface_type IIterator_##interface_type##_iface;      \
            IIterable_##interface_type IIterable_##interface_type##_iface;  \
            UINT32 index;                                                   \
            UINT32 size;                                                    \
            element_type* elements;                                         \
            LONG iterableRef;                                               \
                                                                            \
        struct vector_iids iids;                                            \
        LONG ref;                                                           \
    };                                                                      \
                                                                            \
                                                                            \
    static struct typename##_vector *impl_from_IVector_##interface_type( IVector_##interface_type *iface )     \
    {                                                                                                   \
        return CONTAINING_RECORD( iface, struct typename##_vector, IVector_##interface_type##_iface );  \
    }                                                                                                   \
                                                                                                        \
    static HRESULT WINAPI typename##_vector_QueryInterface( IVector_##interface_type *iface, REFIID iid, void **out )  \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
                                                                                                                            \
        TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );                                           \
                                                                                                                            \
        if (IsEqualGUID( iid, &IID_IUnknown ) ||                                                                            \
            IsEqualGUID( iid, &IID_IInspectable ) ||                                                                        \
            IsEqualGUID( iid, &IID_IAgileObject ) ||                                                                        \
            IsEqualGUID( iid, &IID_IVector_##interface_type ))                                                              \
        {                                                                                                                   \
            IInspectable_AddRef( (*out = &impl->IVector_##interface_type##_iface) );                                        \
            return S_OK;                                                                                                    \
        }                                                                                                                   \
                                                                                                                            \
        if (IsEqualGUID( iid, &IID_IIterable_##interface_type ))                                                            \
        {                                                                                                                   \
            IInspectable_AddRef( (*out = &impl->IIterable_##interface_type##_iface) );                                      \
            return S_OK;                                                                                                    \
        }                                                                                                                   \
                                                                                                                            \
        FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );                                    \
        *out = NULL;                                                                                                        \
        return E_NOINTERFACE;                                                                                               \
    }                                                                                                                       \
                                                                                                                            \
    static ULONG WINAPI typename##_vector_AddRef( IVector_##interface_type *iface )                                         \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        ULONG ref = InterlockedIncrement( &impl->ref );                                                                     \
        TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );                                                      \
        return ref;                                                                                                         \
    }                                                                                                                       \
                                                                                                                            \
    static ULONG WINAPI typename##_vector_Release( IVector_##interface_type *iface )                                        \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        ULONG ref = InterlockedDecrement( &impl->ref );                                                                     \
                                                                                                                            \
        TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );                                                      \
                                                                                                                            \
        if (!ref)                                                                                                           \
        {                                                                                                                   \
            free( impl );                                                                                                   \
        }                                                                                                                   \
                                                                                                                            \
        return ref;                                                                                                         \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_GetIids( IVector_##interface_type *iface, ULONG *iid_count, IID **iids )        \
    {                                                                                                                       \
        FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );                                         \
        return E_NOTIMPL;                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_GetRuntimeClassName( IVector_##interface_type *iface, HSTRING *class_name )     \
    {                                                                                                                       \
        FIXME( "iface %p, class_name %p stub!\n", iface, class_name );                                                      \
        return E_NOTIMPL;                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_GetTrustLevel( IVector_##interface_type *iface, TrustLevel *trust_level )       \
    {                                                                                                                       \
        FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );                                                    \
        return E_NOTIMPL;                                                                                                   \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_GetAt( IVector_##interface_type *iface, UINT32 index, element_type* value )     \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
                                                                                                                            \
        TRACE( "iface %p, index %u, value %p.\n", iface, index, value );                                                    \
                                                                                                                            \
        impl->index = index;                                                                                                \
        return IIterator_##interface_type##_get_Current( &impl->IIterator_##interface_type##_iface, value );                \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_get_Size( IVector_##interface_type *iface, UINT32 *value )                      \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
                                                                                                                            \
        TRACE( "iface %p, value %p.\n", iface, value );                                                                     \
                                                                                                                            \
        *value = impl->size;                                                                                                \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_GetView( IVector_##interface_type *iface, IVectorView_##interface_type **value )\
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        struct typename##_vector_view *view;                                                                                \
                                                                                                                            \
        if (!(view = calloc( 1, sizeof(*view) ))) return E_OUTOFMEMORY;                                                     \
                                                                                                                            \
        view->IVectorView_##interface_type##_iface.lpVtbl = &typename##_vector_view_vtbl;                                   \
        view->IIterable_##interface_type##_iface.lpVtbl = &typename##_iterable_vtbl;                                        \
        view->IIterator_##interface_type##_iface.lpVtbl = &typename##_iterator_vtbl;                                        \
        view->index = 0;                                                                                                    \
        view->size = impl->size;                                                                                            \
        view->elements = impl->elements;                                                                                    \
        view->iterableRef = 2;                                                                                              \
        view->iids = impl->iids;                                                                                            \
        view->ref = 1;                                                                                                      \
                                                                                                                            \
        *value = &view->IVectorView_##interface_type##_iface;                                                               \
                                                                                                                            \
        TRACE( "iface %p, value %p\n", iface, value );                                                                      \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_IndexOf( IVector_##interface_type *iface, element_type element,                 \
                                               UINT32 *index, BOOLEAN *found )                                              \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        ULONG i;                                                                                                            \
                                                                                                                            \
        TRACE( "iface %p, element %p, index %p, found %p.\n", iface, element, index, found );                               \
                                                                                                                            \
        for (i = 0; i < impl->size; ++i) if (impl->elements[i] == element) break;                                           \
        if ((*found = (i < impl->size))) *index = i;                                                                        \
        else *index = 0;                                                                                                    \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_SetAt( IVector_##interface_type *iface, UINT32 index, element_type value )      \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
                                                                                                                            \
        TRACE( "iface %p, index %u, value %p.\n", iface, index, value );                                                    \
                                                                                                                            \
        if (index >= impl->size) return E_BOUNDS;                                                                           \
                                                                                                                            \
        impl->elements[index] = value;                                                                                      \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_InsertAt( IVector_##interface_type *iface, UINT32 index, element_type value )   \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        ULONG i;                                                                                                            \
                                                                                                                            \
        TRACE( "iface %p, index %u, value %p.\n", iface, index, value );                                                    \
                                                                                                                            \
        if (index > impl->size)                                                                                             \
            return E_BOUNDS;                                                                                                \
                                                                                                                            \
        for ( i = index; i < impl->size; i++ )                                                                              \
        {                                                                                                                   \
            impl->elements[i + 1] = impl->elements[i];                                                                      \
        }                                                                                                                   \
                                                                                                                            \
        impl->size++;                                                                                                       \
        impl->elements[index] = value;                                                                                      \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_RemoveAt( IVector_##interface_type *iface, UINT32 index )                       \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        ULONG i;                                                                                                            \
                                                                                                                            \
        TRACE( "iface %p, index %u.\n", iface, index );                                                                     \
                                                                                                                            \
        if (index >= impl->size) return E_BOUNDS;                                                                           \
                                                                                                                            \
        impl->elements[index] = NULL;                                                                                       \
                                                                                                                            \
        for ( i = index; i < impl->size - 1; i++ )                                                                          \
        {                                                                                                                   \
            impl->elements[i] = impl->elements[i + 1];                                                                      \
        }                                                                                                                   \
                                                                                                                            \
        impl->size--;                                                                                                       \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_Append( IVector_##interface_type *iface, element_type value )                   \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
                                                                                                                            \
        TRACE( "iface %p, value %p.\n", iface, value );                                                                     \
                                                                                                                            \
        if (!impl->elements) {                                                                                              \
            impl->elements = (element_type*)malloc(sizeof(element_type) * 1);                                               \
            if (!impl->elements) {                                                                                          \
                return E_OUTOFMEMORY;                                                                                       \
            }                                                                                                               \
        }                                                                                                                   \
                                                                                                                            \
        if (impl->elements) {                                                                                               \
            element_type* new_elements = (element_type*)realloc(impl->elements, sizeof(element_type) * (impl->size + 1));   \
            if (!new_elements) {                                                                                            \
                return E_OUTOFMEMORY;                                                                                       \
            }                                                                                                               \
            impl->elements = new_elements;                                                                                  \
        }                                                                                                                   \
                                                                                                                            \
        impl->elements[impl->size] = value;                                                                                 \
        impl->size++;                                                                                                       \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_RemoveAtEnd( IVector_##interface_type *iface )                                  \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
                                                                                                                            \
        TRACE( "iface %p.\n", iface );                                                                                      \
                                                                                                                            \
        impl->elements[impl->size - 1] = NULL;                                                                              \
        impl->size--;                                                                                                       \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_Clear( IVector_##interface_type *iface )                                        \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        ULONG i;                                                                                                            \
                                                                                                                            \
        TRACE( "iface %p.\n", iface );                                                                                      \
                                                                                                                            \
        for (i = 0; i < impl->size; i++)                                                                                    \
        {                                                                                                                   \
            impl->elements[i] = NULL;                                                                                       \
        }                                                                                                                   \
                                                                                                                            \
        impl->size = 0;                                                                                                     \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_GetMany( IVector_##interface_type *iface, UINT32 start_index,                   \
                                               UINT32 items_size, element_type* items, UINT *count )                        \
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        UINT32 i;                                                                                                           \
                                                                                                                            \
        TRACE( "iface %p, start_index %u, items_size %u, items %p, count %p.\n",                                            \
               iface, start_index, items_size, items, count );                                                              \
                                                                                                                            \
        if (start_index >= impl->size) return E_BOUNDS;                                                                     \
                                                                                                                            \
        for (i = start_index; i < impl->size; ++i)                                                                          \
        {                                                                                                                   \
            if (i - start_index >= items_size) break;                                                                       \
            items[i - start_index] = impl->elements[i];                                                                     \
        }                                                                                                                   \
        *count = i - start_index;                                                                                           \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static HRESULT WINAPI typename##_vector_ReplaceAll( IVector_##interface_type *iface, UINT32 count, element_type* items )\
    {                                                                                                                       \
        struct typename##_vector *impl = impl_from_IVector_##interface_type( iface );                                       \
        ULONG i;                                                                                                            \
                                                                                                                            \
        TRACE( "iface %p, count %u, items %p.\n", iface, count, items );                                                    \
                                                                                                                            \
        for (i = 0; i < impl->size; i++)                                                                                    \
        {                                                                                                                   \
            impl->elements[i] = NULL;                                                                                       \
        }                                                                                                                   \
                                                                                                                            \
        for (i = 0; i < count; i++)                                                                                         \
        {                                                                                                                   \
            impl->elements[i] = items[i];                                                                                   \
        }                                                                                                                   \
                                                                                                                            \
        impl->size = count;                                                                                                 \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }                                                                                                                       \
                                                                                                                            \
    static struct IVector_##interface_type##Vtbl typename##_vector_vtbl =                                            \
    {                                                                                                                       \
        typename##_vector_QueryInterface,                                                                                   \
        typename##_vector_AddRef,                                                                                           \
        typename##_vector_Release,                                                                                          \
        /* IInspectable methods */                                                                                          \
        typename##_vector_GetIids,                                                                                          \
        typename##_vector_GetRuntimeClassName,                                                                              \
        typename##_vector_GetTrustLevel,                                                                                    \
        /* IVector<interface_type##> methods */                                                                             \
        typename##_vector_GetAt,                                                                                            \
        typename##_vector_get_Size,                                                                                         \
        typename##_vector_GetView,                                                                                          \
        typename##_vector_IndexOf,                                                                                          \
        typename##_vector_SetAt,                                                                                            \
        typename##_vector_InsertAt,                                                                                         \
        typename##_vector_RemoveAt,                                                                                         \
        typename##_vector_Append,                                                                                           \
        typename##_vector_RemoveAtEnd,                                                                                      \
        typename##_vector_Clear,                                                                                            \
        typename##_vector_GetMany,                                                                                          \
        typename##_vector_ReplaceAll,                                                                                       \
    };                                                                                                                      \
                                                                                                                            \
    static inline HRESULT typename##_vector_create( IVector_##interface_type **value )                                                    \
    {                                                                                                                       \
        struct typename##_vector *vector;                                                                                   \
                                                                                                                            \
        if (!(vector = calloc( 1, sizeof(*vector) ))) return E_OUTOFMEMORY;                                                 \
                                                                                                                            \
        vector->iids.iterable = &IID_IIterable_##interface_type;                                                            \
        vector->iids.iterator = &IID_IIterator_##interface_type;                                                            \
        vector->iids.vector = &IID_IVector_##interface_type;                                                                \
        vector->iids.view = &IID_IVectorView_##interface_type;                                                              \
                                                                                                                            \
        vector->IIterable_##interface_type##_iface.lpVtbl = &typename##_iterable_vtbl;                                      \
        vector->IIterator_##interface_type##_iface.lpVtbl = &typename##_iterator_vtbl;                                      \
        vector->IVector_##interface_type##_iface.lpVtbl = &typename##_vector_vtbl;                                          \
                                                                                                                            \
        vector->index = 0;                                                                                                  \
        vector->size = 0;                                                                                                   \
                                                                                                                            \
        vector->ref = 1;                                                                                                    \
        vector->iterableRef = 2;                                                                                            \
                                                                                                                            \
        *value = &vector->IVector_##interface_type##_iface;                                                                 \
                                                                                                                            \
        return S_OK;                                                                                                        \
    }

DEFINE_VECTOR( hstring, HSTRING, HSTRING )
DEFINE_VECTOR( storage_folder, StorageFolder, IStorageFolder* )
DEFINE_VECTOR( storage_file, StorageFile, IStorageFile* )

#undef IStorageItem
DEFINE_VECTOR( storage_item, IStorageItem, __x_ABI_CWindows_CStorage_CIStorageItem* )
#define IStorageItem __x_ABI_CWindows_CStorage_CIStorageItem

#endif