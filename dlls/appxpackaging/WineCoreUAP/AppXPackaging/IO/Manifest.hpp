/* 
 * AppX Block Map classes
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

#ifndef __WINE_APPX_MANIFEST_H
#define __WINE_APPX_MANIFEST_H

#include <atomic>

#include "../../../private.h"

class AppxManifestPackageDependency
    : public IAppxManifestPackageDependency2
    , public IAppxManifestPackageDependency3 // IAppxManifestPackageDependency3 does not statically inherit IAppxManifestPackageDependency2
{
public:
    AppxManifestPackageDependency() = default;
    AppxManifestPackageDependency(const AppxManifestPackageDependency&) = delete;
    AppxManifestPackageDependency& operator=(const AppxManifestPackageDependency&) = delete;

    AppxManifestPackageDependency( IXMLDOMElement *element );

    virtual ~AppxManifestPackageDependency() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxManifestPackageDependency Methods */
    HRESULT WINAPI 
    GetName( LPWSTR *name ) noexcept override;

    HRESULT WINAPI 
    GetPublisher( LPWSTR *publisher ) noexcept override;

    HRESULT WINAPI 
    GetMinVersion( UINT64 *minVersion ) noexcept override;

    /* IAppxManifestPackageDependency2 Methods */
    HRESULT WINAPI 
    GetMaxMajorVersionTested( UINT16 *maxMajorVersionTested ) noexcept override;

    /* IAppxManifestPackageDependency3 Methods */
    HRESULT WINAPI 
    GetIsOptional( BOOL *isOptional ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMElement *xmlElement{ nullptr };
};

class AppxManifestPackageDependenciesEnumerator
    : public IAppxManifestPackageDependenciesEnumerator
{
public:
    AppxManifestPackageDependenciesEnumerator() = default;
    AppxManifestPackageDependenciesEnumerator(const AppxManifestPackageDependenciesEnumerator&) = delete;
    AppxManifestPackageDependenciesEnumerator& operator=(const AppxManifestPackageDependenciesEnumerator&) = delete;

    AppxManifestPackageDependenciesEnumerator( IXMLDOMNodeList *nodeList );

    virtual ~AppxManifestPackageDependenciesEnumerator() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxManifestPackageDependenciesEnumerator Methods */
    HRESULT WINAPI 
    GetCurrent( IAppxManifestPackageDependency **dependency ) noexcept override;

    HRESULT WINAPI 
    GetHasCurrent( BOOL *hasCurrent ) noexcept override;

    HRESULT WINAPI 
    MoveNext( BOOL *hasNext ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMNodeList *depNodes{ nullptr };
    LONG iterator{ 0 };
};

class AppxManifestProperties
    : public IAppxManifestProperties
{
public:
    AppxManifestProperties() = default;
    AppxManifestProperties(const AppxManifestProperties&) = delete;
    AppxManifestProperties& operator=(const AppxManifestProperties&) = delete;

    AppxManifestProperties( IXMLDOMElement *element );

    virtual ~AppxManifestProperties() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxManifestProperties Methods */
    HRESULT WINAPI 
    GetBoolValue( LPCWSTR name, BOOL *value ) noexcept override;

    HRESULT WINAPI 
    GetStringValue( LPCWSTR name, LPWSTR *value ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMElement *xmlElement{ nullptr };
};

class AppxManifestPackageId
    : public IAppxManifestPackageId2
{
public:
    AppxManifestPackageId() = default;
    AppxManifestPackageId(const AppxManifestPackageId&) = delete;
    AppxManifestPackageId& operator=(const AppxManifestPackageId&) = delete;

    AppxManifestPackageId( IXMLDOMElement *element );

    virtual ~AppxManifestPackageId() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxManifestPackageId Methods */
    HRESULT WINAPI
    GetName( LPWSTR *name ) noexcept override;

    HRESULT WINAPI
    GetArchitecture( APPX_PACKAGE_ARCHITECTURE *architecture ) noexcept override;

    HRESULT WINAPI
    GetPublisher( LPWSTR *publisher ) noexcept override;

    HRESULT WINAPI
    GetVersion( UINT64 *packageVersion ) noexcept override;

    HRESULT WINAPI
    GetResourceId( LPWSTR *resourceId ) noexcept override;

    HRESULT WINAPI
    ComparePublisher( LPCWSTR other, BOOL *isSame ) noexcept override;

    HRESULT WINAPI
    GetPackageFullName( LPWSTR *packageFullName ) noexcept override;

    HRESULT WINAPI
    GetPackageFamilyName( LPWSTR *packageFamilyName ) noexcept override;

    /* IAppxManifestPackageId2 Methods */
    HRESULT WINAPI
    GetArchitecture2( APPX_PACKAGE_ARCHITECTURE2 *architecture ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMElement *xmlElement{ nullptr };
};

/* IAppxFile backed AppxBlockMapReader */
class AppxManifestReader
    : public IAppxManifestReader2
{
public:
    AppxManifestReader() = default;
    AppxManifestReader(const AppxManifestReader&) = delete;
    AppxManifestReader& operator=(const AppxManifestReader&) = delete;

    AppxManifestReader( IAppxFile *file );

    virtual ~AppxManifestReader() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxManifestReader Methods */
    HRESULT WINAPI
    GetPackageId( IAppxManifestPackageId **packageId ) noexcept override;

    HRESULT WINAPI
    GetProperties( IAppxManifestProperties **packageProperties ) noexcept override;

    HRESULT WINAPI
    GetPackageDependencies( IAppxManifestPackageDependenciesEnumerator **dependencies ) noexcept override;

    HRESULT WINAPI
    GetCapabilities( APPX_CAPABILITIES *capabilities ) noexcept override;

    HRESULT WINAPI
    GetResources( IAppxManifestResourcesEnumerator **resources ) noexcept override;

    HRESULT WINAPI
    GetDeviceCapabilities( IAppxManifestDeviceCapabilitiesEnumerator **deviceCapabilities ) noexcept override;

    HRESULT WINAPI
    GetPrerequisite( LPCWSTR name, UINT64 *value ) noexcept override;

    HRESULT WINAPI
    GetApplications( IAppxManifestApplicationsEnumerator **applications ) noexcept override;

    HRESULT WINAPI 
    GetStream( IStream **manifestStream ) noexcept override;

    /* IAppxManifestReader2 Methods */
    HRESULT WINAPI 
    GetQualifiedResources( IAppxManifestQualifiedResourcesEnumerator **resources ) noexcept override;

private:
    /* Internal Methods */
    HRESULT WINAPI 
    FetchDocument( IXMLDOMDocument3 **document ) noexcept;

    std::atomic<LONG> ref{ 1 };
    IXMLDOMDocument3 *xmlDocument{ nullptr };
    // Do not use MSXML for the reader as it can't satisfy `GetStream`.
    IAppxFile *appxFile{ nullptr };
};

#endif