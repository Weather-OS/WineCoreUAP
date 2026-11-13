/* WinRT Windows.Management.Deployment.PackageManager Implementation
 *
 * Written by Weather.
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
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appx);

using namespace ABI::Windows::Foundation;
using namespace ABI::Windows::Foundation::Collections;
using namespace ABI::Windows::ApplicationModel;
using namespace ABI::Windows::Management::Deployment;

class PackageManagerImpl
    : public IPackageManager
    , public IPackageManager2
{
public:
    PackageManagerImpl() = default;
    virtual ~PackageManagerImpl() = delete;

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
             iid == __uuidof( IPackageManager ) )
        {
            AddRef();
            *out = static_cast<IPackageManager *>(this);
            return S_OK;
        }

        if ( iid == __uuidof( IPackageManager2 ) )
        {
            AddRef();
            *out = static_cast<IPackageManager2 *>(this);
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
        
        *iid_count = 2;
        IID* allocated = static_cast<IID*>( CoTaskMemAlloc( sizeof(IID) * (*iid_count) ) );

        if ( !allocated )
            return E_OUTOFMEMORY;

        allocated[0] = __uuidof( IPackageManager );
        allocated[0] = __uuidof( IPackageManager2 );

        *iids = allocated;
        return S_OK;
    }

    HRESULT WINAPI
    GetRuntimeClassName( HSTRING *class_name ) noexcept override
    {
        TRACE( "iface %p, class_name %p\n", this, class_name );
        return WindowsCreateString( (LPCWSTR)L"Windows.Management.Deployment.PackageManager", 45, class_name );
    }

    HRESULT WINAPI
    GetTrustLevel( TrustLevel *trust_level ) noexcept override
    {
        FIXME( "iface %p, trust_level %p stub!\n", this, trust_level );
        return E_NOTIMPL;
    }

    /* IPackageManager Methods */
    HRESULT WINAPI AddPackageAsync( IUriRuntimeClass *uri, IIterable<Uri *> *dependencies, 
        DeploymentOptions options, IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, uri %p, dependencies %p, options %d, operation %p stub!\n", this, uri, dependencies, options, operation );
        return E_NOTIMPL;
    }
    
    HRESULT WINAPI UpdatePackageAsync( IUriRuntimeClass *uri, IIterable<Uri *> *dependencies,
        DeploymentOptions options, IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, uri %p, dependencies %p, options %d, operation %p stub!\n", this, uri, dependencies, options, operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI RemovePackageAsync( HSTRING name,
        IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, name %s, operation %p stub!\n", this, debugstr_hstring(name), operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI StagePackageAsync( IUriRuntimeClass *uri, IIterable<Uri *> *dependencies,
        IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, uri %p, dependencies %p, operation %p stub!\n", this, uri, dependencies, operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI RegisterPackageAsync( IUriRuntimeClass *uri, IIterable<Uri *> *dependencies,
        DeploymentOptions options, IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, uri %p, dependencies %p, options %d, operation %p stub!\n", this, uri, dependencies, options, operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackages( IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, packages %p stub!\n", this, packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByUserSecurityId( HSTRING sid, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, sid %s, packages %p stub!\n", this, debugstr_hstring(sid), packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByNamePublisher( HSTRING name, HSTRING publisher, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, name %s, publisher %s, packages %p stub!\n", this, debugstr_hstring(name), debugstr_hstring(publisher), packages );

        if (!name || !publisher) return E_INVALIDARG;
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByUserSecurityIdNamePublisher( HSTRING sid, HSTRING name, HSTRING publisher, 
        IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, sid %s, name %s, publisher %s, packages %p stub!\n", this, debugstr_hstring(sid), debugstr_hstring(name), debugstr_hstring(publisher), packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindUsers( HSTRING name, IIterable<PackageUserInformation *> **users ) noexcept override
    {
        FIXME( "iface %p, name %s, users %p stub!\n", this, debugstr_hstring(name), users );
        return E_NOTIMPL;
    }

    HRESULT WINAPI SetPackageState( HSTRING name, PackageState state ) noexcept override
    {
        FIXME("iface %p, name %s, state %d stub!\n", this, debugstr_hstring(name), state);
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackageByPackageFullName( HSTRING name, IPackage **package ) noexcept override
    {
        FIXME( "iface %p, name %s, package %p stub!\n", this, debugstr_hstring(name), package );
        return E_NOTIMPL;
    }

    HRESULT WINAPI CleanupPackageForUserAsync( HSTRING name, HSTRING sid,
        IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, name %s, sid %s, operation %p stub!\n", this, debugstr_hstring(name), debugstr_hstring(sid), operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByPackageFamilyName( HSTRING family_name, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, family_name %s, packages %p stub!\n", this, debugstr_hstring(family_name), packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByUserSecurityIdPackageFamilyName( HSTRING sid, HSTRING family_name, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, sid %s, family_name %s, packages %p stub!\n", this, debugstr_hstring(sid), debugstr_hstring(family_name), packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackageByUserSecurityIdPackageFullName( HSTRING sid, HSTRING name, IPackage **package ) noexcept override
    {
        FIXME( "iface %p, sid %s, name %s, package %p stub!\n", this, debugstr_hstring(sid), debugstr_hstring(name), package );
        return E_NOTIMPL;
    }

    /* IPackageManager2 Methods */
    HRESULT WINAPI RemovePackageWithOptionsAsync( HSTRING name, RemovalOptions options,
        IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, name %s, options %d, operation %p stub!\n", this, debugstr_hstring(name), options, operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI StagePackageWithOptionsAsync( IUriRuntimeClass *uri, IIterable<Uri *> *dependencies,
        DeploymentOptions options, IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, uri %p, dependencies %p, options %d, operation %p stub!\n", this, uri, dependencies, options, operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI RegisterPackageByFullNameAsync( HSTRING name, IIterable<HSTRING> *dependencies,
        DeploymentOptions options, IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation ) noexcept override
    {
        FIXME( "iface %p, name %s, dependencies %p, options %d, operation %p stub!\n", this, debugstr_hstring(name), dependencies, options, operation );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesWithPackageTypes( PackageTypes types, IIterable<Package *> **packages )
    {
        FIXME( "iface %p, types %d, packages %p stub!\n", this, types, packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByUserSecurityIdWithPackageTypes( HSTRING sid, PackageTypes types, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, sid %s, types %d, packages %p stub!\n", this, debugstr_hstring(sid), types, packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByNamePublisherWithPackageTypes( HSTRING name, HSTRING publisher, PackageTypes types, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, name %s, publisher %s, types %d, packages %p stub!\n", this, debugstr_hstring(name), debugstr_hstring(publisher), types, packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByUserSecurityIdNamePublisherWithPackageTypes( HSTRING sid, HSTRING name,
        HSTRING publisher, PackageTypes types, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, sid %s, name %s, publisher %s, types %d, packages %p stub!\n", this, debugstr_hstring(sid), debugstr_hstring(name), debugstr_hstring(publisher), types, packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByPackageFamilyNameWithPackageTypes( HSTRING family_name, PackageTypes types, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, family_name %s, types %d, packages %p stub!\n", this, debugstr_hstring(family_name), types, packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI FindPackagesByUserSecurityIdPackageFamilyNameWithPackageTypes( HSTRING sid, HSTRING family_name,
        PackageTypes types, IIterable<Package *> **packages ) noexcept override
    {
        FIXME( "iface %p, sid %s, family_name %s, types %d, packages %p stub!\n", this, debugstr_hstring(sid), debugstr_hstring(family_name), types, packages );
        return E_NOTIMPL;
    }

    HRESULT WINAPI StageUserDataAsync( HSTRING name,
        IAsyncOperationWithProgress<DeploymentResult *, DeploymentProgress> **operation )
    {
        FIXME( "iface %p, name %s, operation %p stub!\n", this, debugstr_hstring(name), operation );
        return E_NOTIMPL;
    }
private:
    std::atomic<LONG> ref{ 1 };
};

class ABI::Windows::Management::Deployment::PackageManager
    : public IActivationFactory
{
public:
    PackageManager() = default;

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
        
        *iid_count = 1;
        IID* allocated = static_cast<IID*>( CoTaskMemAlloc( sizeof(IID) * (*iid_count) ) );

        if ( !allocated )
            return E_OUTOFMEMORY;

        allocated[0] = __uuidof( IActivationFactory );

        *iids = allocated;
        return S_OK;
    }

    HRESULT WINAPI
    GetRuntimeClassName( HSTRING *class_name ) noexcept override
    {
        TRACE( "iface %p, class_name %p\n", this, class_name );
        return WindowsCreateString( (LPCWSTR)L"Windows.Management.Deployment.PackageManager", 45, class_name );
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
        PackageManagerImpl *packageManager;
        TRACE( "iface %p, instance %p\n", this, instance );

        if ( !instance )
            return E_POINTER;

        packageManager = new PackageManagerImpl();

        HRESULT hr = packageManager->QueryInterface( IID_PPV_ARGS( instance ) );
        packageManager->Release();

        return hr;
    }

private:
    std::atomic<LONG> ref{ 1 };
};

static PackageManager g_package_manager;

IActivationFactory* package_manager_factory =
    static_cast<IActivationFactory*>(&g_package_manager);