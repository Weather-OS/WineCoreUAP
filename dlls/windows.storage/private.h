/* WinRT Windows.Storage Implementation
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

#ifndef __WINE_WINDOWS_STORAGE_PRIVATE_H
#define __WINE_WINDOWS_STORAGE_PRIVATE_H

#include <stdarg.h>

#define COBJMACROS
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "shellapi.h"

#include "activation.h"

//The specific order in which header files are included here is extremely important.
#define WIDL_using_Windows_Foundation
#define WIDL_using_Windows_Foundation_Collections
#define WIDL_using_Windows_Foundation_Numerics
#include "windows.foundation.h"
#define WIDL_using_Windows_Storage_Streams
#include "windows.storage.streams.h"
#define WIDL_using_Windows_Storage_FileProperties
#include "windows.storage.fileproperties.h"
#define WIDL_using_Windows_System
#include "windows.system.h"
#define WIDL_using_Windows_Storage
#include "windows.storage.h"

#include "provider.h"

struct vector_iids
{
    const GUID *vector;
    const GUID *view;
    const GUID *iterable;
    const GUID *iterator;
};

extern IActivationFactory *app_data_paths_factory;
extern IActivationFactory *user_data_paths_factory;
extern IActivationFactory *storage_folder_factory;
extern IActivationFactory *storage_file_factory;
extern IActivationFactory *known_folders_factory;
extern IActivationFactory *basic_properties_factory;
extern IActivationFactory *storage_item_content_properties_factory;
extern IActivationFactory *downloads_folder_factory;
extern IActivationFactory *file_io_factory;
extern IActivationFactory *path_io_factory;
extern IActivationFactory *buffer_factory;
extern IActivationFactory *system_properties_factory;
extern IActivationFactory *system_data_paths_factory;

typedef HRESULT (WINAPI *async_operation_callback)( IUnknown *invoker, IUnknown *param, PROPVARIANT *result );

HRESULT async_info_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, 
                                              IInspectable *outer, IWineAsyncInfoImpl **out );

extern HRESULT async_operation_hstring_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_HSTRING **out );
extern HRESULT async_operation_hstring_vector_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IVector_HSTRING **out );                                              
extern HRESULT async_operation_known_folders_access_status_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_KnownFoldersAccessStatus **out );
extern HRESULT async_operation_storage_file_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_StorageFile **out );
extern HRESULT async_operation_storage_folder_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_StorageFolder **out );
extern HRESULT async_operation_storage_item_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IStorageItem **out );
extern HRESULT async_operation_buffer_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IBuffer **out );
extern HRESULT async_operation_storage_file_vector_view_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IVectorView_StorageFile **out );
extern HRESULT async_operation_storage_item_vector_view_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IVectorView_IStorageItem **out );
extern HRESULT async_operation_storage_folder_vector_view_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_IVectorView_StorageFolder **out );
extern HRESULT async_action_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback, 
                                              IAsyncAction **ret);

extern HRESULT async_operation_basic_properties_create( IUnknown *invoker, IUnknown *param, async_operation_callback callback,
                                              IAsyncOperation_BasicProperties **out );


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
