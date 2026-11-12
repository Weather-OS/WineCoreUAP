/* WinRT Windows.ApplicationModel.Core Implementation
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

#include <atomic>

#include "../../private.h"
#include "provider.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(core);

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::ApplicationModel;
using namespace ABI::Windows::ApplicationModel::Activation;
using namespace ABI::Windows::ApplicationModel::Core;

class ABI::Windows::ApplicationModel::Core::CoreApplication
    : public IActivationFactory
    , public ICoreApplication
    , public ICoreApplication2
    , public ICoreApplication3
    , public ICoreApplicationExit
    , public ICoreApplicationUnhandledError
    , public ICoreApplicationUseCount
    , public ICoreImmersiveApplication
    , public ICoreImmersiveApplication2
    , public ICoreImmersiveApplication3
{
public:
    CoreApplication() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override
    {
        TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

        if (!out) return E_POINTER;
        *out = nullptr;

        if ( iid == __uuidof( IUnknown ) ||
             iid == __uuidof( IInspectable ) ||
             iid == __uuidof( IAgileObject ) ||
             iid == __uuidof( IActivationFactory ) )
        {
            AddRef();
            *out = static_cast<IActivationFactory *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreApplication ) )
        {
            AddRef();
            *out = static_cast<ICoreApplication *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreApplication2 ) )
        {
            AddRef();
            *out = static_cast<ICoreApplication2 *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreApplication2 ) )
        {
            AddRef();
            *out = static_cast<ICoreApplication2 *>(this);
            return S_OK;
        }
        
        if ( iid == __uuidof( ICoreApplication3 ) )
        {
            AddRef();
            *out = static_cast<ICoreApplication3 *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreApplicationExit ) )
        {
            AddRef();
            *out = static_cast<ICoreApplicationExit *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreApplicationUnhandledError ) )
        {
            AddRef();
            *out = static_cast<ICoreApplicationUnhandledError *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreApplicationUseCount ) )
        {
            AddRef();
            *out = static_cast<ICoreApplicationUseCount *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreImmersiveApplication ) )
        {
            AddRef();
            *out = static_cast<ICoreImmersiveApplication *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreImmersiveApplication2 ) )
        {
            AddRef();
            *out = static_cast<ICoreImmersiveApplication2 *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( ICoreImmersiveApplication3 ) )
        {
            AddRef();
            *out = static_cast<ICoreImmersiveApplication3 *>(this);
            return S_OK;
        }

        FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
        *out = nullptr;
        return E_NOINTERFACE;
    }

    ULONG WINAPI 
    AddRef() noexcept override
    {
        ULONG curr = static_cast<ULONG>(++ref);
        TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
        return curr;
    }

    ULONG WINAPI 
    Release() noexcept override
    {
        ULONG curr = static_cast<ULONG>(--ref);
        TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

        /*
         * Polymorphic Classes should not be deleted.
         * if ( !curr )
         *    delete this;
         */

        return curr;
    }

    /* IInspectable Methods (Shared) */
    HRESULT WINAPI
    GetIids( ULONG *iid_count, IID **iids ) noexcept override
    {
        TRACE( "iface %p, iid_count %p, iids %p\n", this, iid_count, iids );

        if ( !iid_count || !iids )
            return E_POINTER;
        
        *iid_count = 10;
        IID* allocated = static_cast<IID*>( CoTaskMemAlloc( sizeof(IID) * (*iid_count) ) );

        if ( !allocated )
            return E_OUTOFMEMORY;

        allocated[0] = __uuidof( IActivationFactory );
        allocated[1] = __uuidof( ICoreApplication );
        allocated[2] = __uuidof( ICoreApplication2 );
        allocated[3] = __uuidof( ICoreApplication3 );
        allocated[4] = __uuidof( ICoreApplicationExit );
        allocated[5] = __uuidof( ICoreApplicationUnhandledError );
        allocated[6] = __uuidof( ICoreApplicationUseCount );
        allocated[7] = __uuidof( ICoreImmersiveApplication );
        allocated[8] = __uuidof( ICoreImmersiveApplication2 );
        allocated[9] = __uuidof( ICoreImmersiveApplication3 );

        *iids = allocated;
        return S_OK;
    }

    HRESULT WINAPI
    GetRuntimeClassName( HSTRING *class_name ) noexcept override
    {
        TRACE( "iface %p, class_name %p\n", this, class_name );
        return WindowsCreateString( (LPCWSTR)L"Windows.ApplicationModel.Core.CoreApplication", 46, class_name );
    }

    HRESULT WINAPI
    GetTrustLevel( TrustLevel *trust_level ) noexcept override
    {
        FIXME( "iface %p, trust_level %p stub!\n", this, trust_level );
        return E_NOTIMPL;
    }

    /* IActivationFactory Methods (Shared) */
    HRESULT WINAPI
    ActivateInstance( IInspectable **instance ) noexcept override
    {
        ERR( "iface %p, This factory is not activatable!\n", this );
        return E_NOTIMPL;
    }

    /* ICoreApplication Methods */
    HRESULT WINAPI
    get_Id( HSTRING *id ) noexcept override
    {
        FIXME( "iface %p, id %p stub!\n", this, id );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    add_Suspending( IEventHandler<ABI::Windows::ApplicationModel::SuspendingEventArgs *> *handler, EventRegistrationToken *token ) noexcept override
    {
        FIXME( "iface %p, handler %p, token %p stub!\n", this, handler, token );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    remove_Suspending( EventRegistrationToken token ) noexcept override
    {
        FIXME( "iface %p, token %lld stub!\n", this, token.value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    add_Resuming( IEventHandler<IInspectable *> *handler, EventRegistrationToken *token ) noexcept override
    {
        FIXME( "iface %p, handler %p, token %p stub!\n", this, handler, token );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    remove_Resuming( EventRegistrationToken token ) noexcept override
    {
        FIXME( "iface %p, token %lld stub!\n", this, token.value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    get_Properties( IPropertySet **value ) noexcept override
    {
        FIXME( "iface %p, value %p stub!\n", this, value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    GetCurrentView( ICoreApplicationView **value ) noexcept override
    {
        FIXME( "iface %p, value %p stub!\n", this, value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    Run( IFrameworkViewSource *view_source ) noexcept override
    {
        FIXME( "iface %p, view_source %p stub!\n", this, view_source );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    RunWithActivationFactories( IGetActivationFactory *factory ) noexcept override
    {
        FIXME( "iface %p, factory %p stub!\n", this, factory );
        return E_NOTIMPL;
    }

    /* ICoreApplication2 Methods */
    HRESULT WINAPI
    add_BackgroundActivated( IEventHandler<BackgroundActivatedEventArgs *> *handler, EventRegistrationToken *token ) noexcept override
    {
        FIXME( "iface %p, handler %p, token %p stub!\n", this, handler, token );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    remove_BackgroundActivated( EventRegistrationToken token ) noexcept override
    {
        FIXME( "iface %p, token %lld stub!\n", this, token.value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    add_LeavingBackground( IEventHandler<LeavingBackgroundEventArgs *> *handler, EventRegistrationToken *token ) noexcept override
    {
        FIXME( "iface %p, handler %p, token %p stub!\n", this, handler, token );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    remove_LeavingBackground( EventRegistrationToken token ) noexcept override
    {
        FIXME( "iface %p, token %lld stub!\n", this, token.value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    add_EnteredBackground( IEventHandler<EnteredBackgroundEventArgs *> *handler, EventRegistrationToken *token ) noexcept override
    {
        FIXME( "iface %p, handler %p, token %p stub!\n", this, handler, token );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    remove_EnteredBackground( EventRegistrationToken token ) noexcept override
    {
        FIXME( "iface %p, token %lld stub!\n", this, token.value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    EnablePrelaunch( boolean value ) noexcept override
    {
        FIXME( "iface %p, value %d stub!\n", this, value );
        return E_NOTIMPL;
    }

    /* ICoreApplication3 Methods */
    HRESULT WINAPI
    RequestRestartAsync( HSTRING launch_arguments, IAsyncOperation<AppRestartFailureReason > **operation ) noexcept override
    {
        FIXME( "iface %p, launch_arguments %s, operation %p stub!\n", this, debugstr_hstring( launch_arguments ), operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    RequestRestartForUserAsync( Windows::System::IUser *user, HSTRING launch_arguments, IAsyncOperation<AppRestartFailureReason > **operation ) noexcept override
    {
        FIXME( "iface %p, user %p, launch_arguments %s, operation %p stub!\n", this, user, debugstr_hstring( launch_arguments ), operation );
        return E_NOTIMPL;
    }

    /* ICoreApplicationExit Methods */
    HRESULT WINAPI
    Exit() noexcept override
    {
        FIXME( "iface %p stub!\n", this );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    add_Exiting( IEventHandler<IInspectable *> *handler, EventRegistrationToken *token ) noexcept override
    {
        FIXME( "iface %p, handler %p, token %p stub!\n", this, handler, token );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    remove_Exiting( EventRegistrationToken token ) noexcept override
    {
        FIXME( "iface %p, token %lld stub!\n", this, token.value );
        return E_NOTIMPL;
    }

    /* ICoreApplicationUnhandledError Methods */
    HRESULT WINAPI
    add_UnhandledErrorDetected( IEventHandler<UnhandledErrorDetectedEventArgs *> *handler, EventRegistrationToken *token ) noexcept override
    {
        FIXME( "iface %p, handler %p, token %p stub!\n", this, handler, token );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    remove_UnhandledErrorDetected( EventRegistrationToken token ) noexcept override
    {
        FIXME( "iface %p, token %lld stub!\n", this, token.value );
        return E_NOTIMPL;
    }

    /* ICoreApplicationUseCount Methods */
    HRESULT WINAPI
    IncrementApplicationUseCount() noexcept override
    {
        FIXME( "iface %p stub!\n", this );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    DecrementApplicationUseCount() noexcept override
    {
        FIXME( "iface %p stub!\n", this );
        return E_NOTIMPL;
    }

    /* ICoreImmersiveApplication Methods */
    HRESULT WINAPI
    get_Views( IVectorView<CoreApplicationView *> **value ) noexcept override
    {
        FIXME( "iface %p, value %p stub!\n", this, value );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    CreateNewView( HSTRING runtime_type, HSTRING entry_point, ICoreApplicationView **view ) noexcept override
    {
        FIXME( "iface %p, runtime_type %s, entry_point %s, view %p stub!\n", this, debugstr_hstring( runtime_type ), debugstr_hstring( entry_point ), view );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    get_MainView( ICoreApplicationView **value ) noexcept override
    {
        FIXME( "iface %p, value %p stub!\n", this, value );
        return E_NOTIMPL;
    }

    /* ICoreImmersiveApplication2 Methods */
    HRESULT WINAPI
    CreateNewViewFromMainView( ICoreApplicationView **view ) noexcept override
    {
        FIXME( "iface %p, view %p stub!\n", this, view );
        return E_NOTIMPL;
    }

    /* ICoreImmersiveApplication3 Methods */
    HRESULT WINAPI
    CreateNewViewWithViewSource( IFrameworkViewSource *view_source, ICoreApplicationView **view ) noexcept override
    {
        FIXME( "iface %p, view_source %p, view %p stub!\n", this, view_source, view );
        return E_NOTIMPL;
    }

private:
    std::atomic<ULONG> ref{};
};

static CoreApplication g_core_application;

IActivationFactory* core_application_factory =
    static_cast<IActivationFactory*>(&g_core_application);