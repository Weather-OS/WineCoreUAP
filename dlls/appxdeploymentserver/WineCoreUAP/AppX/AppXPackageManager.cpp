/* 
 * AppXPackageManager Class Factory Implementation.
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
#include <new>

#include "../../private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxds);

class AppXPackageManagerImpl
    : public IAppXPackageManager
{
public:
    AppXPackageManagerImpl() = default;
    AppXPackageManagerImpl(const AppXPackageManagerImpl&) = delete;
    AppXPackageManagerImpl& operator=(const AppXPackageManagerImpl&) = delete;

    virtual ~AppXPackageManagerImpl() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override
    {
        TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

        if (!out) return E_POINTER;
        *out = nullptr;

        if ( iid == __uuidof( IUnknown ) ||
             iid == __uuidof( IAppXPackageManager ) )
        {
            AddRef();
            *out = static_cast<IAppXPackageManager *>(this);
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

        if ( !curr )
           delete this;
        
        return curr;
    }

    /* IAppXPackageManager Methods */
    HRESULT WINAPI 
    AddAppXPackage( LPCWSTR packagePath ) noexcept override
    {
        TRACE( "iface %p, packagePath %s stub!\n", this, debugstr_w(packagePath) );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    StageAppXPackage( LPCWSTR packagePath ) noexcept override
    {
        TRACE( "iface %p, packagePath %s stub!\n", this, debugstr_w(packagePath) );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    RegisterAppXPackage( LPCWSTR packagePath ) noexcept override
    {
        TRACE( "iface %p, packagePath %s stub!\n", this, debugstr_w(packagePath) );
        return E_NOTIMPL;
    }

    HRESULT WINAPI
    RemoveAppXPackage( LPCWSTR packageFullName ) noexcept override
    {
        TRACE( "iface %p, packagePath %s stub!\n", this, debugstr_w(packageFullName) );
        return E_NOTIMPL;
    }
private:
    std::atomic<LONG> ref{ 1 };
};

class AppXPackageManager
    : public IClassFactory
{
public:
    AppXPackageManager() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override
    {
        TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

        if (!out) return E_POINTER;
        *out = nullptr;

        if ( iid == __uuidof( IUnknown ) ||
             iid == __uuidof( IClassFactory ) )
        {
            AddRef();
            *out = static_cast<IClassFactory *>(this);
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
         * Non-heap based objects should not be deleted.
         * if ( !curr )
         *    delete this;
         */

        return curr;
    }

    /* IClassFactory Methods */
    HRESULT WINAPI
    CreateInstance( IUnknown *pUnkOuter, REFIID riid, void **ppvObj) noexcept override
    {
        HRESULT hr;
        AppXPackageManagerImpl *manager = nullptr;

        TRACE( "iface %p, pUnkOuter %p, riid %s, ppvObj %p.\n", this, pUnkOuter, debugstr_guid( &riid ), ppvObj );

        if ( pUnkOuter )
            return CLASS_E_NOAGGREGATION;

        if ( !ppvObj )
            return E_POINTER;

        manager = new(std::nothrow) AppXPackageManagerImpl();
        if ( !manager ) 
            return E_OUTOFMEMORY;

        hr = manager->QueryInterface( riid, ppvObj );
        manager->Release();

        return hr;
    }

    HRESULT WINAPI
    LockServer( BOOL fLock )
    {
        FIXME("iface %p, fLock %d stub!\n", this, fLock);
        return E_NOTIMPL;
    }
private:
    std::atomic<LONG> ref{ 1 };
};

static AppXPackageManager g_appx_package_manager;

IClassFactory* appx_package_manager_factory =
    static_cast<IClassFactory*>(&g_appx_package_manager);