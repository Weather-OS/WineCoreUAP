/* 
 * AppXPackage Reader and Writer interfaces.
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

#ifndef __WINE_APPX_PACKAGE_IO_H
#define __WINE_APPX_PACKAGE_IO_H

#include <atomic>

#include "../../../private.h"

/* IOpcPart backed AppxFile */
class AppxFile
    : public IAppxFile
{
public:
    AppxFile() = default;
    AppxFile(const AppxFile&) = delete;
    AppxFile& operator=(const AppxFile&) = delete;

    AppxFile( IOpcPart *part );

    virtual ~AppxFile() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxFile Methods */
    HRESULT WINAPI 
    GetCompressionOption( APPX_COMPRESSION_OPTION *compressionOption ) noexcept override;

    HRESULT WINAPI 
    GetContentType( LPWSTR *contentType ) noexcept override;

    HRESULT WINAPI 
    GetName( LPWSTR *fileName ) noexcept override;

    HRESULT WINAPI 
    GetSize( UINT64 *size ) noexcept override;

    HRESULT WINAPI 
    GetStream( IStream **stream ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IOpcPart *filePart{ nullptr };
};

/*
 * AppxFilesEnumerator should not be IOpcPartEnumerator backed, as it
 * doesn't represent footprint files.
 */
class AppxFilesEnumerator
    : public IAppxFilesEnumerator
{
public:
    AppxFilesEnumerator() = default;
    AppxFilesEnumerator(const AppxFilesEnumerator&) = delete;
    AppxFilesEnumerator& operator=(const AppxFilesEnumerator&) = delete;

    AppxFilesEnumerator( IAppxFile **files, SIZE_T size );

    virtual ~AppxFilesEnumerator() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    HRESULT WINAPI 
    GetCurrent( IAppxFile **file ) noexcept override;

    HRESULT WINAPI 
    GetHasCurrent( BOOL *hasCurrent ) noexcept override;

    HRESULT WINAPI 
    MoveNext( BOOL *hasNext ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    SIZE_T filesCount;
    IAppxFile **files;
};

/* IAppxFile backed AppxBlockMapReader */
class AppxBlockMapReader
    : public IAppxBlockMapReader
{
public:
    AppxBlockMapReader() = default;
    AppxBlockMapReader(const AppxBlockMapReader&) = delete;
    AppxBlockMapReader& operator=(const AppxBlockMapReader&) = delete;

    AppxBlockMapReader( IAppxFile *file );

    virtual ~AppxBlockMapReader() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxBlockMapReader Methods */
    HRESULT WINAPI 
    GetFile( LPCWSTR filename, IAppxBlockMapFile **file ) noexcept override;

    HRESULT WINAPI 
    GetFiles( IAppxBlockMapFilesEnumerator **enumerator ) noexcept override;

    HRESULT WINAPI 
    GetHashMethod( IUri **hashMethod ) noexcept override;

    HRESULT WINAPI 
    GetStream( IStream **blockMapStream ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IAppxFile *appxFile{ nullptr };
};

class AppxPackageReader
    : public IAppxPackageReader
{
public:
    AppxPackageReader() = default;
    AppxPackageReader(const AppxPackageReader&) = delete;
    AppxPackageReader& operator=(const AppxPackageReader&) = delete;

    AppxPackageReader( IStream *fileStream, IOpcPackage *opcPackage );

    virtual ~AppxPackageReader() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxPackageReader Methods */
    HRESULT WINAPI 
    GetBlockMap( IAppxBlockMapReader **blockMapReader ) noexcept override;

    HRESULT WINAPI 
    GetFootprintFile( APPX_FOOTPRINT_FILE_TYPE type, 
                      IAppxFile **file ) noexcept override;

    HRESULT WINAPI 
    GetPayloadFile( LPCWSTR fileName, IAppxFile **file ) noexcept override;

    HRESULT WINAPI 
    GetPayloadFiles( IAppxFilesEnumerator **filesEnumerator ) noexcept override;

    HRESULT WINAPI 
    GetManifest( IAppxManifestReader **manifestReader ) noexcept override;

private:
    /* Internal Methods */
    HRESULT WINAPI
    GetFile( LPCWSTR fileName, IAppxFile **file ) noexcept;

    std::atomic<LONG> ref{ 1 };
    IStream *packageFileStream{ nullptr };
    IOpcPackage *package{ nullptr };
};

class AppxPackageWriter
    : public IAppxPackageWriter
{
public:
    AppxPackageWriter() = default;
    AppxPackageWriter(const AppxPackageWriter&) = delete;
    AppxPackageWriter& operator=(const AppxPackageWriter&) = delete;

    virtual ~AppxPackageWriter() = default;

    AppxPackageWriter( IStream *fileStream );

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxPackageWriter Methods */
    HRESULT WINAPI 
    AddPayloadFile( LPCWSTR fileName, LPCWSTR contentType, 
                    APPX_COMPRESSION_OPTION compressionOption, 
                    IStream *inputStream ) noexcept override;

    HRESULT WINAPI 
    Close( IStream *manifest ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IStream *packageFileStream{ nullptr };
};

#endif