/* 
 * AppX Block Map Implementation
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

#include "BlockMap.hpp"

#include "wincrypt.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

// ------------- IAppxBlockMapBlock ------------- //
AppxBlockMapBlock::AppxBlockMapBlock( IXMLDOMElement *element )
    : xmlElement( element )
{
    element->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapBlock::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapBlock ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapBlock *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapBlock::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapBlock::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( xmlElement ) xmlElement->Release();

        delete this;
    }
    
    return curr;
}

/* IAppxBlockMapBlock Methods */
HRESULT WINAPI 
AppxBlockMapBlock::GetHash( UINT32 *bufferSize, BYTE **buffer ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, bufferSize %p, buffer %p.\n", this, bufferSize, buffer );

    if ( !bufferSize || !buffer )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"Hash" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for blockmap block %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            if ( !CryptStringToBinaryW( V_BSTR( &val ), 0, CRYPT_STRING_BASE64, *buffer, reinterpret_cast<DWORD *>(bufferSize), NULL, NULL ) )
            {
                ERR( "for blockmap block %p, failed to calculate hash! return value was %ld\n", this, GetLastError() );
                hr = HRESULT_FROM_WIN32( GetLastError() );
            }
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI 
AppxBlockMapBlock::GetCompressedSize( UINT32 *size ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, lfhSize %p.\n", this, size );

    if ( !size )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"Size" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for blockmap block %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_I4 );
        if ( SUCCEEDED( hr ) )
            *size = V_I4( &val );
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

// ------------- IAppxBlockMapBlocksEnumerator ------------- //
AppxBlockMapBlocksEnumerator::AppxBlockMapBlocksEnumerator( IXMLDOMNodeList *nodes )
    : blockNodes( nodes )
{
    nodes->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapBlocksEnumerator::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapBlocksEnumerator ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapBlocksEnumerator *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapBlocksEnumerator::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapBlocksEnumerator::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if( blockNodes ) blockNodes->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxBlockMapBlocksEnumerator Methods */
HRESULT WINAPI 
AppxBlockMapBlocksEnumerator::GetCurrent( IAppxBlockMapBlock **block ) noexcept
{
    LONG len;
    HRESULT hr = S_OK;
    IXMLDOMNode *node;
    IXMLDOMElement *elem;

    TRACE( "iface %p, block %p.\n", this, block );

    if ( !block )
        return E_POINTER;

    if ( FAILED( hr = blockNodes->get_length( &len ) ) )
        return hr;
    
    if ( iterator >= len )
        return E_BOUNDS;

    if ( FAILED( hr = blockNodes->get_item( iterator, &node ) ) )
        return hr;
        
    hr = node->QueryInterface( &elem );
    node->Release();
    if ( FAILED( hr ) )
        return hr;

    *block = new(std::nothrow) AppxBlockMapBlock( elem );
    elem->Release();
    if ( !*block ) 
        hr = E_OUTOFMEMORY;

    return hr;
}

HRESULT WINAPI 
AppxBlockMapBlocksEnumerator::GetHasCurrent( BOOL *hasCurrent ) noexcept
{
    LONG len;
    HRESULT hr = S_OK;

    TRACE( "iface %p, hasCurrent %p stub!\n", this, hasCurrent );

    if ( !hasCurrent )
        return E_POINTER;

    if ( FAILED( hr = blockNodes->get_length( &len ) ) )
        return hr;
    
    if ( iterator >= len )
        *hasCurrent = FALSE;
    else
        *hasCurrent = TRUE;

    return hr;
}

HRESULT WINAPI
AppxBlockMapBlocksEnumerator::MoveNext( BOOL *hasNext ) noexcept
{
    LONG len;
    HRESULT hr = S_OK;

    TRACE( "iface %p, hasNext %p.\n", this, hasNext );

    if ( !hasNext )
        return E_POINTER;

    if ( FAILED( hr = blockNodes->get_length( &len ) ) )
        return hr;

    if ( iterator < len )
        iterator++;

    *hasNext = ( iterator < len );

    return S_OK;
}

// ------------- IAppxBlockMapFile ------------- //
AppxBlockMapFile::AppxBlockMapFile( IXMLDOMElement *element )
    : xmlElement( element )
{
    element->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapFile::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapFile ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapFile *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapFile::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapFile::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( xmlElement ) xmlElement->Release();

        delete this;
    }
    
    return curr;
}

/* IAppxBlockMapFile Methods */
HRESULT WINAPI 
AppxBlockMapFile::GetBlocks( IAppxBlockMapBlocksEnumerator **blocks ) noexcept
{
    HRESULT hr;
    IXMLDOMNode *node;
    IXMLDOMNodeList *blockList;

    TRACE( "iface %p, blocks %p.\n", this, blocks );

    if ( !blocks )
        return E_POINTER;

    if ( FAILED( hr = xmlElement->QueryInterface( &node ) ) )
        return hr;

    hr = node->selectNodes( (BSTR)L"bm:Block", &blockList );
    node->Release();
    if ( FAILED( hr ) )
        return hr;

    *blocks = new(std::nothrow) AppxBlockMapBlocksEnumerator( blockList );
    blockList->Release();
    if ( !*blocks )
        hr = E_OUTOFMEMORY;

    return hr;
}

HRESULT WINAPI 
AppxBlockMapFile::GetLocalFileHeaderSize( UINT32 *lfhSize ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, lfhSize %p.\n", this, lfhSize );

    if ( !lfhSize )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"LfhSize" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for blockmap file %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_UI4 );
        if ( SUCCEEDED( hr ) )
            *lfhSize = V_UI4( &val );
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI 
AppxBlockMapFile::GetName( LPWSTR *name ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, name %p.\n", this, name );

    if ( !name )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"Name" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for blockmap file %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            *name = (LPWSTR)CoTaskMemAlloc( SysStringByteLen( V_BSTR( &val ) ) + sizeof(WCHAR) );
            if ( *name )
                lstrcpynW( *name, V_BSTR( &val ), SysStringLen( V_BSTR( &val ) ) + 1 );
            else
                hr = E_OUTOFMEMORY;
        }
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI 
AppxBlockMapFile::GetUncompressedSize( UINT64 *size ) noexcept
{
    BSTR attributeName;
    HRESULT hr = S_OK;
    VARIANT val;

    TRACE( "iface %p, size %p.\n", this, size );

    if ( !size )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"Size" );
    if ( !attributeName )
        return E_OUTOFMEMORY;
    
    VariantInit( &val );
    
    hr = xmlElement->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for blockmap file %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_UI8 );
        if ( SUCCEEDED( hr ) )
            *size = V_UI8( &val );
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI 
AppxBlockMapFile::ValidateFileHash( IStream *fileStream, BOOL *isValid ) noexcept
{
    FIXME( "iface %p, fileStream %p, isValid %p stub!\n", this, fileStream, isValid );
    return E_NOTIMPL;
}

// ------------- IAppxBlockMapFilesEnumerator ------------- //
AppxBlockMapFilesEnumerator::AppxBlockMapFilesEnumerator( IXMLDOMNodeList *nodes )
    : fileNodes( nodes )
{
    nodes->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapFilesEnumerator::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapFilesEnumerator ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapFilesEnumerator *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapFilesEnumerator::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapFilesEnumerator::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if( fileNodes ) fileNodes->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxBlockMapFilesEnumerator Methods */
HRESULT WINAPI 
AppxBlockMapFilesEnumerator::GetCurrent( IAppxBlockMapFile **file ) noexcept
{
    LONG len;
    HRESULT hr = S_OK;
    IXMLDOMNode *node;
    IXMLDOMElement *elem;

    TRACE( "iface %p, file %p.\n", this, file );

    if ( !file )
        return E_POINTER;

    if ( FAILED( hr = fileNodes->get_length( &len ) ) )
        return hr;
    
    if ( iterator >= len )
        return E_BOUNDS;

    if ( FAILED( hr = fileNodes->get_item( iterator, &node ) ) )
        return hr;
        
    hr = node->QueryInterface( &elem );
    node->Release();
    if ( FAILED( hr ) )
        return hr;

    *file = new(std::nothrow) AppxBlockMapFile( elem );
    elem->Release();
    if ( !*file ) 
        hr = E_OUTOFMEMORY;

    return hr;
}

HRESULT WINAPI 
AppxBlockMapFilesEnumerator::GetHasCurrent( BOOL *hasCurrent ) noexcept
{
    LONG len;
    HRESULT hr = S_OK;

    TRACE( "iface %p, hasCurrent %p stub!\n", this, hasCurrent );

    if ( !hasCurrent )
        return E_POINTER;

    if ( FAILED( hr = fileNodes->get_length( &len ) ) )
        return hr;
    
    if ( iterator >= len )
        *hasCurrent = FALSE;
    else
        *hasCurrent = TRUE;

    return hr;
}

HRESULT WINAPI
AppxBlockMapFilesEnumerator::MoveNext( BOOL *hasNext ) noexcept
{
    LONG len;
    HRESULT hr = S_OK;

    TRACE( "iface %p, hasNext %p.\n", this, hasNext );

    if ( !hasNext )
        return E_POINTER;

    if ( FAILED( hr = fileNodes->get_length( &len ) ) )
        return hr;

    if ( iterator < len )
        iterator++;

    *hasNext = ( iterator < len );

    return S_OK;
}

// ------------- IAppxBlockMapReader ------------- //
AppxBlockMapReader::AppxBlockMapReader( IAppxFile *file )
    : appxFile( file )
{
    file->AddRef();
}

/* IUnknown Methods (Shared) */
HRESULT WINAPI
AppxBlockMapReader::QueryInterface( REFIID iid, void** out ) noexcept
{
    TRACE( "iface %p, iid %s, out %p.\n", this, debugstr_guid( &iid ), out );

    if (!out) return E_POINTER;
    *out = nullptr;

    if ( iid == __uuidof( IUnknown ) ||
         iid == __uuidof( IAppxBlockMapReader ) )
    {
        AddRef();
        *out = static_cast<IAppxBlockMapReader *>(this);
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( &iid ) );
    *out = nullptr;
    return E_NOINTERFACE;
}

ULONG WINAPI 
AppxBlockMapReader::AddRef() noexcept
{
    ULONG curr = static_cast<ULONG>(++ref);
    TRACE( "iface %p increasing refcount to %lu.\n", this, curr );
    return curr;
}

ULONG WINAPI 
AppxBlockMapReader::Release() noexcept
{
    ULONG curr = static_cast<ULONG>(--ref);
    TRACE( "iface %p decreasing refcount to %lu.\n", this, curr );

    if ( !curr )
    {
        if ( appxFile ) appxFile->Release();
        delete this;
    }
    
    return curr;
}

/* IAppxBlockMapReader Methods */
HRESULT WINAPI
AppxBlockMapReader::GetFile( LPCWSTR filename, IAppxBlockMapFile **file ) noexcept
{
    BOOL exists;
    LPWSTR current_name = nullptr;
    HRESULT hr = S_OK;
    IAppxBlockMapFile *current_file = nullptr;
    IAppxBlockMapFilesEnumerator *enumerator = nullptr;

    TRACE( "iface %p, filename %s, file %p.\n", this, debugstr_w( filename ), file );

    if ( FAILED( hr = GetFiles( &enumerator ) ) )
        return hr;

    hr = enumerator->GetHasCurrent( &exists );
    if ( FAILED( hr ) )
    {
        enumerator->Release();
        return hr;
    }
    if ( !exists )
    {
        enumerator->Release();
        return APPX_E_INVALID_BLOCKMAP;
    }

    do
    {
        if ( FAILED( hr = enumerator->GetCurrent( &current_file ) ) )
            break;

        if ( FAILED( hr = current_file->GetName( &current_name ) ) )
            break;

        if ( lstrcmpW( current_name, filename ) == 0 )
        {
            *file = current_file;
            break;
        }

        current_file->Release();

        if ( FAILED( hr = enumerator->MoveNext( &exists ) ) )
            break;
        
    } while ( exists );

    enumerator->Release();

    return hr;
}

HRESULT WINAPI
AppxBlockMapReader::GetFiles( IAppxBlockMapFilesEnumerator **enumerator ) noexcept
{
    IXMLDOMDocument3 *document = nullptr;
    IXMLDOMNodeList *nodeList = nullptr;
    IXMLDOMElement *rootElem = nullptr;
    HRESULT hr = S_OK;
    VARIANT val;
    VARIANT nsval;
    BSTR attributeName;

    TRACE( "iface %p, enumerator %p.\n", this, enumerator );

    if ( !enumerator )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"xmlns" );
    if ( !attributeName )
        return E_OUTOFMEMORY;

    hr = FetchDocument( &document );
    if ( FAILED( hr ) )
    {
        SysFreeString( attributeName );
        return hr;
    }

    hr = document->get_documentElement( &rootElem );
    if ( FAILED( hr ) || !rootElem )
    {
        if ( rootElem )
            rootElem->Release();

        document->Release();
        SysFreeString( attributeName );

        return FAILED( hr ) ? hr : APPX_E_INVALID_BLOCKMAP;
    }

    VariantInit( &val );
    
    hr = rootElem->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for blockmap reader %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
        {
            // uri needs validation
            if ( lstrcmpiW( V_BSTR( &val ), GetResourceW( IDS_XMLNS_URI ) ) == 0 )
            {
                VariantInit( &nsval );
                nsval.vt = VT_BSTR;
                nsval.bstrVal = SysAllocStringLen( nullptr, lstrlenW( (LPWSTR)L"xmlns:bm='" ) + SysStringLen( V_BSTR( &val ) ) + lstrlenW( (LPWSTR)L"'" ) );
                if ( !nsval.bstrVal )
                {
                    hr = E_OUTOFMEMORY;
                }
                else
                {
                    lstrcpyW( nsval.bstrVal, (LPWSTR)L"xmlns:bm='" );
                    lstrcatW( nsval.bstrVal, V_BSTR( &val ) );
                    lstrcatW( nsval.bstrVal, (LPWSTR)L"'" );
                }

                if ( SUCCEEDED( hr ) )
                {
                    document->setProperty( (BSTR)L"SelectionNameSpaces", nsval );
                    VariantClear( &nsval );

                    hr = document->selectNodes( (BSTR)L"/bm:BlockMap/bm:File", &nodeList );
                    if ( SUCCEEDED( hr ) && nodeList )
                    {
                        *enumerator = new(std::nothrow) AppxBlockMapFilesEnumerator( nodeList );
                        nodeList->Release();
                        if ( !*enumerator ) 
                            hr = E_OUTOFMEMORY;
                    } else if ( !nodeList  )
                    {
                        hr = APPX_E_INVALID_BLOCKMAP;
                    }
                }
            } else 
            {
                ERR( "for blockmap reader %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
                hr = APPX_E_INVALID_BLOCKMAP;
            }
        }
    }

    document->Release();
    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI
AppxBlockMapReader::GetHashMethod( IUri **hashMethod ) noexcept
{
    IXMLDOMDocument3 *document = nullptr;
    IXMLDOMElement *rootElem = nullptr;
    HRESULT hr = S_OK;
    VARIANT val;
    BSTR attributeName;

    TRACE( "iface %p, hashMethod %p.\n", this, hashMethod );

    if ( !hashMethod )
        return E_POINTER;

    attributeName = SysAllocString( (OLECHAR *)L"HashMethod" );
    if ( !attributeName )
        return E_OUTOFMEMORY;

    hr = FetchDocument( &document );
    if ( FAILED( hr ) )
    {
        SysFreeString( attributeName );
        return hr;
    }

    hr = document->get_documentElement( &rootElem );
    document->Release();
    if ( FAILED( hr ) )
    {
        SysFreeString( attributeName );
        return hr;
    }

    VariantInit( &val );
    
    hr = rootElem->getAttribute( attributeName, &val );
    if ( hr == S_FALSE || (SUCCEEDED( hr ) && ( V_VT( &val ) == VT_EMPTY || V_VT( &val ) == VT_NULL )) )
    {
        ERR( "for blockmap reader %p, its XML element does not include %s!\n", this, debugstr_w( attributeName ) );
        hr = APPX_E_INVALID_BLOCKMAP;
    }

    if ( SUCCEEDED( hr ) )
    {
        hr = VariantChangeType( &val, &val, 0, VT_BSTR );
        if ( SUCCEEDED( hr ) )
            hr = CreateUri( V_BSTR( &val ), Uri_CREATE_CANONICALIZE, 0, hashMethod );
    }

    SysFreeString( attributeName );
    VariantClear( &val );

    return hr;
}

HRESULT WINAPI
AppxBlockMapReader::GetStream( IStream **blockMapStream ) noexcept
{
    TRACE( "iface %p, blockMapStream %p.\n", this, blockMapStream );

    return appxFile->GetStream( blockMapStream );
}

/* Internal Methods */
HRESULT WINAPI 
AppxBlockMapReader::FetchDocument( IXMLDOMDocument3 **document ) noexcept
{
    IStream *blockMapStream = nullptr;
    HRESULT hr = S_OK;
    VARIANT var;
    VARIANT_BOOL bLoaded = VARIANT_FALSE;

    TRACE( "iface %p, document %p.\n", this, document );

    if ( !document )
        return E_POINTER;
    
    if ( !xmlDocument )
    {
        hr = CoCreateInstance( __uuidof(DOMDocument60), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &xmlDocument ) );
        if ( FAILED( hr ) ) 
            return hr;

        hr = xmlDocument->put_async( VARIANT_FALSE );
        if ( FAILED( hr ) ) 
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return hr;
        }

        hr = GetStream( &blockMapStream );
        if ( FAILED( hr ) ) 
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return hr;
        }

        VariantInit( &var );
        var.vt = VT_UNKNOWN;
        var.punkVal = static_cast<IUnknown *>( blockMapStream );
        blockMapStream->AddRef();

        hr = xmlDocument->load( var, &bLoaded );
        blockMapStream->Release();
        VariantClear( &var );
        if ( FAILED( hr ) || bLoaded != VARIANT_TRUE ) 
        {
            xmlDocument->Release();
            xmlDocument = nullptr;
            return hr;
        }
    }
       
    hr = xmlDocument->QueryInterface( document );

    return hr;
}