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
#include "IO/BlockMap.hpp"
#include "IO/Manifest.hpp"

#include "wintrust.h"
#include "shlwapi.h"
#include "softpub.h"

#include "../../private.h"
#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

static const GUID AppxP7xSignatureSIP =
{
    0x5598CFF1, 0x68DB, 0x4340,
    { 0xB5, 0x7F, 0x1C, 0xAC, 0xF8, 0x8C, 0x9A, 0x51 }
};

static GUID ActionGenericVerifyV2 = WINTRUST_ACTION_GENERIC_VERIFY_V2;

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
        IOpcRelationshipEnumerator *enumerator = nullptr;
        IOpcRelationshipSet *relationshipSet = nullptr;
        IOpcPackage *package = nullptr;
        IOpcFactory *opc_factory = nullptr;
        IAppxFile *current_file = nullptr;

        HRESULT hr = S_OK;
        BOOL exists;

        TRACE( "iface %p, inputStream %p, packageReader %p.\n", this, inputStream, packageReader );
        
        if ( !packageReader )
            return E_POINTER;

        if ( !inputStream )
            return E_INVALIDARG;

        hr = CoCreateInstance( CLSID_OpcFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &opc_factory ) );
        if ( FAILED( hr ) ) 
            return hr;

        hr = opc_factory->ReadPackageFromStream( inputStream, OPC_READ_FLAGS::OPC_READ_DEFAULT, &package );
        opc_factory->Release();
        if ( FAILED( hr ) ) 
            return hr;
        
        reader = new(std::nothrow) AppxPackageReader( inputStream, package );
        package->Release();
        if ( !reader ) 
            return E_OUTOFMEMORY;

        /* Pass 1: AppX Packages should not contain relationships */
        hr = package->GetRelationshipSet(&relationshipSet);
        if ( SUCCEEDED( hr ) )
        {
            hr = relationshipSet->GetEnumerator( &enumerator );
            relationshipSet->Release();

            if ( SUCCEEDED( hr ) )
            {
                hr = enumerator->MoveNext( &exists );
                enumerator->Release();

                if ( SUCCEEDED(hr) && exists )
                {
                    reader->Release();
                    return APPX_E_RELATIONSHIPS_NOT_ALLOWED;
                }
            }
        }

        /* Pass 2: AppX Packages should contain AppxManifest.xml and AppxBlockMap.xml */
        hr = reader->GetFootprintFile( APPX_FOOTPRINT_FILE_TYPE::APPX_FOOTPRINT_FILE_TYPE_BLOCKMAP, &current_file );
        if ( FAILED( hr ) )
        {
            reader->Release();
            return hr == OPC_E_NO_SUCH_PART ? APPX_E_MISSING_REQUIRED_FILE : hr;
        }
        current_file->Release();

        hr = reader->GetFootprintFile( APPX_FOOTPRINT_FILE_TYPE::APPX_FOOTPRINT_FILE_TYPE_MANIFEST, &current_file );
        if ( FAILED( hr ) )
        {
            reader->Release();
            return hr == OPC_E_NO_SUCH_PART ? APPX_E_MISSING_REQUIRED_FILE : hr;
        }
        current_file->Release();

        *packageReader = reader;

        return S_OK;
    }
    
    HRESULT WINAPI 
    CreateManifestReader( IStream *inputStream, IAppxManifestReader **manifestReader ) noexcept override
    {
        HRESULT hr = S_OK;
        IAppxFile *manifestFile = nullptr;

        TRACE( "iface %p, inputStream %p, manifestReader %p.\n", this, inputStream, manifestReader );

        if ( !inputStream )
            return E_INVALIDARG;

        if ( !manifestFile )
            return E_POINTER;

        manifestFile = new(std::nothrow) AppxFile( inputStream );
        if ( !manifestFile )
            return E_OUTOFMEMORY;

        *manifestReader = new(std::nothrow) AppxManifestReader( manifestFile );
        manifestFile->Release();
        if ( !*manifestReader )
            return E_OUTOFMEMORY;

        return hr;
    }
    
    HRESULT WINAPI 
    CreateBlockMapReader( IStream *inputStream, IAppxBlockMapReader **blockMapReader ) noexcept override
    {
        HRESULT hr = S_OK;
        IAppxFile *blockMapFile = nullptr;

        TRACE( "iface %p, inputStream %p, blockMapReader %p.\n", this, inputStream, blockMapReader );

        if ( !inputStream )
            return E_INVALIDARG;

        if ( !blockMapReader )
            return E_POINTER;

        blockMapFile = new(std::nothrow) AppxFile( inputStream );
        if ( !blockMapFile )
            return E_OUTOFMEMORY;

        *blockMapReader = new(std::nothrow) AppxBlockMapReader( blockMapFile );
        blockMapFile->Release();
        if ( !*blockMapReader )
            return E_OUTOFMEMORY;

        return hr;
    }
    
    HRESULT WINAPI 
    CreateValidatedBlockMapReader( IStream *blockMapStream, LPCWSTR signatureFileName, 
                                   IAppxBlockMapReader **blockMapReader ) noexcept override
    {
        LONG trustStatus = 0;
        WINTRUST_FILE_INFO fileInfo{ .cbStruct = sizeof(WINTRUST_FILE_INFO),
                                     .pcwszFilePath = signatureFileName,
                                     .pgKnownSubject = const_cast<GUID*>(&AppxP7xSignatureSIP) };
        WINTRUST_DATA trustData{ .cbStruct = sizeof(WINTRUST_DATA),
                                 .dwUIChoice = WTD_UI_NONE,
                                 .fdwRevocationChecks = WTD_REVOKE_NONE,
                                 .dwUnionChoice = WTD_CHOICE_FILE,
                                 .pFile = &fileInfo,
                                 .dwStateAction = WTD_STATEACTION_VERIFY,
                                 .dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL };

        TRACE( "iface %p, blockMapStream %p, signatureFileName %s, manifestReader %p.\n", 
            this, blockMapStream, debugstr_w(signatureFileName), blockMapReader );

        if ( !blockMapStream || !signatureFileName )
            return E_INVALIDARG;

        if ( !blockMapReader )
            return E_POINTER;

        // For now, just verify the p7x content data.
        // TODO: Check the contents of blockMapStream against the p7x content data.

        trustStatus = WinVerifyTrust( nullptr, &ActionGenericVerifyV2, &trustData );

        trustData.dwStateAction = WTD_STATEACTION_CLOSE;

        WinVerifyTrust( nullptr, &ActionGenericVerifyV2, &trustData );

        if ( trustStatus != ERROR_SUCCESS )
            return HRESULT_FROM_WIN32( trustStatus );

        return CreateBlockMapReader( blockMapStream, blockMapReader );
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