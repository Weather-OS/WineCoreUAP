/* 
 * AppXFactory Class Factory Implementation.
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

#include "IO/PackageIO.hpp"

#include "../../private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

class AppxFactoryImpl
    : public IAppxFactory
{
public:
    AppxFactoryImpl() = default;
    AppxFactoryImpl(const AppxFactoryImpl&) = delete;
    AppxFactoryImpl& operator=(const AppxFactoryImpl&) = delete;

    virtual ~AppxFactoryImpl() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override
    {
        TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

        if (!out) return E_POINTER;
        *out = nullptr;

        if ( iid == __uuidof( IUnknown ) ||
             iid == __uuidof( IAppxFactory ) )
        {
            AddRef();
            *out = static_cast<IAppxFactory *>(this);
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
    CreatePackageWriter( IStream *outputStream, APPX_PACKAGE_SETTINGS *settings, 
                         IAppxPackageWriter **packageWriter ) noexcept override
    {
        AppxPackageWriter *writer = nullptr;
        
        FIXME( "iface %p, outputStream %p, settings %p, packageWriter %p semi-stub!\n", this, outputStream, settings, packageWriter );
        
        if ( !packageWriter )
            return E_POINTER;

        if ( !outputStream || !settings )
            return E_INVALIDARG;

        writer = new(std::nothrow) AppxPackageWriter( outputStream );
        if ( !writer ) 
            return E_OUTOFMEMORY;

        *packageWriter = writer;

        return S_OK;
    }
    
    HRESULT WINAPI 
    CreatePackageReader( IStream *inputStream, IAppxPackageReader **packageReader ) noexcept override
    {
        AppxPackageReader *reader = nullptr;

        TRACE( "iface %p, inputStream %p, packageReader %p.\n", this, inputStream, packageReader );
        
        if ( !packageReader )
            return E_POINTER;

        if ( !inputStream )
            return E_INVALIDARG;

        reader = new(std::nothrow) AppxPackageReader( inputStream );
        if ( !reader ) 
            return E_OUTOFMEMORY;

        *packageReader = reader;

        return E_NOTIMPL;
    }
    
    HRESULT WINAPI 
    CreateManifestReader( IStream *inputStream, IAppxManifestReader **manifestReader ) noexcept override
    {
        FIXME( "iface %p, inputStream %p, manifestReader %p stub!\n", this, inputStream, manifestReader );
        return E_NOTIMPL;
    }
    
    HRESULT WINAPI 
    CreateBlockMapReader( IStream *inputStream, IAppxBlockMapReader **blockMapReader ) noexcept override
    {
        FIXME( "iface %p, inputStream %p, manifestReader %p stub!\n", this, inputStream, blockMapReader );
        return E_NOTIMPL;
    }
    
    HRESULT WINAPI 
    CreateValidatedBlockMapReader( IStream *blockMapStream, LPCWSTR signatureFileName, 
                                   IAppxBlockMapReader **blockMapReader ) noexcept override
    {
        FIXME( "iface %p, blockMapStream %p, signatureFileName %s, manifestReader %p stub!\n", 
            this, blockMapStream, debugstr_w(signatureFileName), blockMapReader );
        return E_NOTIMPL;
    }
private:
    std::atomic<LONG> ref{ 1 };
};

class AppxFactory
    : public IClassFactory
{
public:
    AppxFactory() = default;

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
    CreateInstance( IUnknown *pUnkOuter, REFIID riid, void **ppvObj ) noexcept override
    {
        HRESULT hr;
        AppxFactoryImpl *factory = nullptr;

        TRACE( "iface %p, pUnkOuter %p, riid %s, ppvObj %p.\n", this, pUnkOuter, debugstr_guid( &riid ), ppvObj );

        if ( pUnkOuter )
            return CLASS_E_NOAGGREGATION;

        if ( !ppvObj )
            return E_POINTER;

        factory = new(std::nothrow) AppxFactoryImpl();
        if ( !factory ) 
            return E_OUTOFMEMORY;

        hr = factory->QueryInterface( riid, ppvObj );
        factory->Release();

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

static AppxFactory g_appx_factory;

IClassFactory* appx_factory =
    static_cast<IClassFactory*>(&g_appx_factory);