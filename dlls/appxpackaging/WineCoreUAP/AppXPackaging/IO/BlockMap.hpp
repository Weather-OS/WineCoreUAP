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

#ifndef __WINE_APPX_BLOCKMAP_H
#define __WINE_APPX_BLOCKMAP_H

#include <atomic>

#include "../../../private.h"

class AppxBlockMapBlock
    : public IAppxBlockMapBlock
{
public:
    AppxBlockMapBlock() = default;
    AppxBlockMapBlock(const AppxBlockMapBlock&) = delete;
    AppxBlockMapBlock& operator=(const AppxBlockMapBlock&) = delete;

    AppxBlockMapBlock( IXMLDOMElement *element );

    virtual ~AppxBlockMapBlock() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxBlockMapFile Methods */
    HRESULT WINAPI 
    GetHash( UINT32 *bufferSize, BYTE **buffer ) noexcept override;

    HRESULT WINAPI 
    GetCompressedSize( UINT32 *size ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMElement *xmlElement{ nullptr };
    DWORD hashMethod; // <-- Hash method needs to be passed to AppxBlockMapBlock
};

class AppxBlockMapBlocksEnumerator
    : public IAppxBlockMapBlocksEnumerator
{
public:
    AppxBlockMapBlocksEnumerator() = default;
    AppxBlockMapBlocksEnumerator(const AppxBlockMapBlocksEnumerator&) = delete;
    AppxBlockMapBlocksEnumerator& operator=(const AppxBlockMapBlocksEnumerator&) = delete;

    AppxBlockMapBlocksEnumerator( IXMLDOMNodeList *nodes );

    virtual ~AppxBlockMapBlocksEnumerator() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxBlockMapBlocksEnumerator Methods */
    HRESULT WINAPI 
    GetCurrent( IAppxBlockMapBlock **block ) noexcept override;

    HRESULT WINAPI 
    GetHasCurrent( BOOL *hasCurrent ) noexcept override;

    HRESULT WINAPI 
    MoveNext( BOOL *hasNext ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMNodeList *blockNodes{ nullptr };
    DWORD hashMethod; // <-- Hash method needs to be passed to AppxBlockMapBlock
};

class AppxBlockMapFile
    : public IAppxBlockMapFile
{
public:
    AppxBlockMapFile() = default;
    AppxBlockMapFile(const AppxBlockMapFile&) = delete;
    AppxBlockMapFile& operator=(const AppxBlockMapFile&) = delete;

    AppxBlockMapFile( IXMLDOMElement *element );

    virtual ~AppxBlockMapFile() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxBlockMapFile Methods */
    HRESULT WINAPI 
    GetBlocks( IAppxBlockMapBlocksEnumerator **blocks ) noexcept override;

    HRESULT WINAPI 
    GetLocalFileHeaderSize( UINT32 *lfhSize ) noexcept override;

    HRESULT WINAPI 
    GetName( LPWSTR *name ) noexcept override;

    HRESULT WINAPI 
    GetUncompressedSize( UINT64 *size ) noexcept override;

    HRESULT WINAPI 
    ValidateFileHash( IStream *fileStream, BOOL *isValid ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMElement *xmlElement{ nullptr };
    DWORD hashMethod; // <-- Hash method needs to be passed to AppxBlockMapBlock
};

class AppxBlockMapFilesEnumerator
    : public IAppxBlockMapFilesEnumerator
{
public:
    AppxBlockMapFilesEnumerator() = default;
    AppxBlockMapFilesEnumerator(const AppxBlockMapFilesEnumerator&) = delete;
    AppxBlockMapFilesEnumerator& operator=(const AppxBlockMapFilesEnumerator&) = delete;

    AppxBlockMapFilesEnumerator( IXMLDOMNodeList *nodes );

    virtual ~AppxBlockMapFilesEnumerator() = default;

    /* IUnknown Methods (Shared) */
    HRESULT WINAPI
    QueryInterface( REFIID iid, void** out ) noexcept override;

    ULONG WINAPI 
    AddRef() noexcept override;

    ULONG WINAPI 
    Release() noexcept override;

    /* IAppxBlockMapFilesEnumerator Methods */
    HRESULT WINAPI 
    GetCurrent( IAppxBlockMapFile **file ) noexcept override;

    HRESULT WINAPI 
    GetHasCurrent( BOOL *hasCurrent ) noexcept override;

    HRESULT WINAPI 
    MoveNext( BOOL *hasNext ) noexcept override;

private:
    std::atomic<LONG> ref{ 1 };
    IXMLDOMNodeList *fileNodes{ nullptr };
    DWORD hashMethod; // <-- Hash method needs to be passed to AppxBlockMapBlock
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

#endif