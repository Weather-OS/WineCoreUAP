/* WinRT Windows.Storage.PathIO Implementation
 *
 * Written by Weather
 *
 * This is an implementation of Microsoft's OneCoreUAP binaries.
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

#include "PathIOInternal.h"

_ENABLE_DEBUGGING_

// Path Input Output Operations

static struct path_io_statics *impl_from_IActivationFactory( IActivationFactory *iface )
{
    return CONTAINING_RECORD( iface, struct path_io_statics, IActivationFactory_iface );
}

static HRESULT WINAPI factory_QueryInterface( IActivationFactory *iface, REFIID iid, void **out )
{

    struct path_io_statics *impl = impl_from_IActivationFactory( iface );

    TRACE( "iface %p, iid %s, out %p.\n", iface, debugstr_guid( iid ), out );

    if (IsEqualGUID( iid, &IID_IUnknown ) ||
        IsEqualGUID( iid, &IID_IInspectable ) ||
        IsEqualGUID( iid, &IID_IAgileObject ) ||
        IsEqualGUID( iid, &IID_IActivationFactory ))
    {
        *out = &impl->IActivationFactory_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    if (IsEqualGUID( iid, &IID_IPathIOStatics ))
    {
        *out = &impl->IPathIOStatics_iface;
        IInspectable_AddRef( *out );
        return S_OK;
    }

    FIXME( "%s not implemented, returning E_NOINTERFACE.\n", debugstr_guid( iid ) );
    *out = NULL;
    return E_NOINTERFACE;
}

static ULONG WINAPI factory_AddRef( IActivationFactory *iface )
{
    struct path_io_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedIncrement( &impl->ref );
    TRACE( "iface %p increasing refcount to %lu.\n", iface, ref );
    return ref;
}

static ULONG WINAPI factory_Release( IActivationFactory *iface )
{
    struct path_io_statics *impl = impl_from_IActivationFactory( iface );
    ULONG ref = InterlockedDecrement( &impl->ref );
    TRACE( "iface %p decreasing refcount to %lu.\n", iface, ref );
    return ref;
}

static HRESULT WINAPI factory_GetIids( IActivationFactory *iface, ULONG *iid_count, IID **iids )
{
    FIXME( "iface %p, iid_count %p, iids %p stub!\n", iface, iid_count, iids );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetRuntimeClassName( IActivationFactory *iface, HSTRING *class_name )
{
    FIXME( "iface %p, class_name %p stub!\n", iface, class_name );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_GetTrustLevel( IActivationFactory *iface, TrustLevel *trust_level )
{
    FIXME( "iface %p, trust_level %p stub!\n", iface, trust_level );
    return E_NOTIMPL;
}

static HRESULT WINAPI factory_ActivateInstance( IActivationFactory *iface, IInspectable **instance )
{
    FIXME( "iface %p, instance %p stub!\n", iface, instance );
    return E_NOTIMPL;
}

static const struct IActivationFactoryVtbl factory_vtbl =
{
    factory_QueryInterface,
    factory_AddRef,
    factory_Release,
    /* IInspectable methods */
    factory_GetIids,
    factory_GetRuntimeClassName,
    factory_GetTrustLevel,
    /* IActivationFactory methods */
    factory_ActivateInstance,
};

DEFINE_IINSPECTABLE( path_io_statics, IPathIOStatics, struct path_io_statics, IActivationFactory_iface )

static HRESULT WINAPI path_io_statics_ReadTextAsync( IPathIOStatics *iface, HSTRING absolutePath, IAsyncOperation_HSTRING **textOperation )
{
    HRESULT hr;
    struct path_io_read_text_options *read_text_options;

    TRACE( "iface %p, operation %p\n", iface, textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    read_text_options->encoding = UnicodeEncoding_Utf8;
    read_text_options->absolutePath = absolutePath;

    hr = async_operation_hstring_create( (IUnknown *)iface, (IUnknown *)read_text_options, path_io_statics_ReadText, textOperation );
    
    free( read_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_ReadTextWithEncodingAsync( IPathIOStatics *iface, HSTRING absolutePath, UnicodeEncoding encoding, IAsyncOperation_HSTRING **textOperation )
{
    HRESULT hr;
    struct path_io_read_text_options *read_text_options;

    TRACE( "iface %p, operation %p\n", iface, textOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    read_text_options->encoding = encoding;
    read_text_options->absolutePath = absolutePath;

    hr = async_operation_hstring_create( (IUnknown *)iface, (IUnknown *)read_text_options, path_io_statics_ReadText, textOperation );
    
    free( read_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_WriteTextAsync( IPathIOStatics *iface, HSTRING absolutePath, HSTRING contents, IAsyncAction **textOperation )
{
    HRESULT hr;
    struct path_io_write_text_options *write_text_options;

    TRACE( "iface %p, operation %p\n", iface, textOperation );

    //Arguments
    if ( !absolutePath || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    write_text_options->encoding = UnicodeEncoding_Utf8;
    write_text_options->absolutePath = absolutePath;
    write_text_options->contents = contents;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_WriteText, textOperation );
    
    free( write_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_WriteTextWithEncodingAsync( IPathIOStatics *iface, HSTRING absolutePath, HSTRING contents, UnicodeEncoding encoding, IAsyncAction **textOperation )
{
    HRESULT hr;
    struct path_io_write_text_options *write_text_options;

    TRACE( "iface %p, operation %p\n", iface, textOperation );

    //Arguments
    if ( !absolutePath || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    write_text_options->encoding = encoding;
    write_text_options->absolutePath = absolutePath;
    write_text_options->contents = contents;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_WriteText, textOperation );
    
    free( write_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_AppendTextAsync( IPathIOStatics *iface, HSTRING absolutePath, HSTRING contents, IAsyncAction **textOperation )
{
    HRESULT hr;
    struct path_io_write_text_options *write_text_options;
    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    TRACE( "iface %p, operation %p\n", iface, textOperation );

    //Arguments
    if ( !absolutePath || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    write_text_options->encoding = UnicodeEncoding_Utf8;
    write_text_options->absolutePath = absolutePath;
    write_text_options->contents = contents;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_AppendText, textOperation );
    
    free( write_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_AppendTextWithEncodingAsync( IPathIOStatics *iface, HSTRING absolutePath, HSTRING contents, UnicodeEncoding encoding, IAsyncAction **textOperation )
{
    HRESULT hr;
    struct path_io_write_text_options *write_text_options;
    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    TRACE( "iface %p, operation %p\n", iface, textOperation );

    //Arguments
    if ( !absolutePath || !contents ) return E_INVALIDARG;
    if ( !textOperation ) return E_POINTER;

    write_text_options->encoding = encoding;
    write_text_options->absolutePath = absolutePath;
    write_text_options->contents = contents;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_AppendText, textOperation );
    
    free( write_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_ReadLinesAsync( IPathIOStatics *iface, HSTRING absolutePath, IAsyncOperation_IVector_HSTRING **linesOperation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVector_HSTRING };
    struct path_io_read_text_options *read_text_options;
    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    TRACE( "iface %p, operation %p\n", iface, linesOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !linesOperation ) return E_POINTER;

    read_text_options->encoding = UnicodeEncoding_Utf8;
    read_text_options->absolutePath = absolutePath;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)read_text_options, path_io_statics_ReadLines, iids, (IAsyncOperation_IInspectable **)linesOperation );
    
    free( read_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_ReadLinesWithEncodingAsync( IPathIOStatics *iface, HSTRING absolutePath, UnicodeEncoding encoding, IAsyncOperation_IVector_HSTRING **linesOperation )
{
    HRESULT hr;
    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IVector_HSTRING };
    struct path_io_read_text_options *read_text_options;
    if (!(read_text_options = calloc( 1, sizeof(*read_text_options) ))) return E_OUTOFMEMORY;

    TRACE( "iface %p, operation %p\n", iface, linesOperation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !linesOperation ) return E_POINTER;

    read_text_options->encoding = encoding;
    read_text_options->absolutePath = absolutePath;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)read_text_options, path_io_statics_ReadLines, iids, (IAsyncOperation_IInspectable **)linesOperation );
    
    free( read_text_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_WriteLinesAsync( IPathIOStatics *iface, HSTRING absolutePath, IIterable_HSTRING *lines, IAsyncAction **operation )
{
    //Convert IIterable_HSTRING to HSTRING
    HRESULT hr;
    HSTRING *strings;
    LPWSTR combinedString = NULL;
    LPWSTR tmpStr;
    UINT32 vectorSize = 0;
    UINT32 totalSize = 0;
    UINT32 i;
    boolean strExists;
    
    IIterator_HSTRING *hstringIterator;

    struct path_io_write_text_options *write_text_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !absolutePath || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = IIterable_HSTRING_First( lines, &hstringIterator );
    if ( SUCCEEDED( hr ) )
    {
        IIterator_HSTRING_get_HasCurrent( hstringIterator, &strExists );
        while ( strExists )
        {
            vectorSize++;
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );
        }

        strings = (HSTRING *)malloc( vectorSize * sizeof( HSTRING ) );
        IIterable_HSTRING_First( lines, &hstringIterator );
        
        for ( i = 0; i < vectorSize; i++ )
        {
            IIterator_HSTRING_get_Current( hstringIterator, &strings[i] );
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );

            totalSize += WindowsGetStringLen( strings[i] ) + 1; 
            combinedString = (LPWSTR)malloc( ( totalSize + 1 ) * sizeof( WCHAR ) );
            combinedString[0] = L'\0';
            tmpStr = (LPWSTR)malloc( ( WindowsGetStringLen( strings[i] ) + 1 ) * sizeof( WCHAR ) );

            wcscpy( tmpStr, WindowsGetStringRawBuffer( strings[i], NULL ) );
            wcscat( tmpStr, L"\n" );
            wcscat( combinedString, tmpStr );
            
            free( tmpStr );
        }
    } else {
        free( write_text_options );
        return hr;
    }

    //Remove trailing nextspace
    combinedString[ wcslen(combinedString) - 1 ] = L'\0'; 

    write_text_options->encoding = UnicodeEncoding_Utf8;
    write_text_options->absolutePath = absolutePath;
    if ( combinedString )
        WindowsCreateString( combinedString, wcslen( combinedString ), &write_text_options->contents );

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_WriteText, operation );

    free( write_text_options );
    if ( combinedString ) 
        free( combinedString );
    IIterator_HSTRING_Release( hstringIterator );
    for ( i = 0; i < vectorSize; i++ )
    {
        if ( strings[i] != NULL )
        {
            WindowsDeleteString( strings[i] );
        }
    }
    free( strings );

    return hr;
}

static HRESULT WINAPI path_io_statics_WriteLinesWithEncodingAsync( IPathIOStatics *iface, HSTRING absolutePath, IIterable_HSTRING *lines, UnicodeEncoding encoding, IAsyncAction **operation )
{
    //Convert IIterable_HSTRING to HSTRING
    HRESULT hr;
    HSTRING *strings;
    LPWSTR combinedString = NULL;
    LPWSTR tmpStr;
    UINT32 vectorSize = 0;
    UINT32 totalSize = 0;
    UINT32 i;
    boolean strExists;
    IIterator_HSTRING *hstringIterator;

    struct path_io_write_text_options *write_text_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !absolutePath || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = IIterable_HSTRING_First( lines, &hstringIterator );
    if ( SUCCEEDED( hr ) )
    {
        IIterator_HSTRING_get_HasCurrent( hstringIterator, &strExists );
        while ( strExists )
        {
            vectorSize++;
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );
        }

        strings = (HSTRING *)malloc( vectorSize * sizeof( HSTRING ) );
        IIterable_HSTRING_First( lines, &hstringIterator );
        
        for ( i = 0; i < vectorSize; i++ )
        {
            IIterator_HSTRING_get_Current( hstringIterator, &strings[i] );
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );

            totalSize += WindowsGetStringLen( strings[i] ) + 1; 
            combinedString = (LPWSTR)malloc( ( totalSize + 1 ) * sizeof( WCHAR ) );
            combinedString[0] = L'\0';
            tmpStr = (LPWSTR)malloc( ( WindowsGetStringLen( strings[i] ) + 1 ) * sizeof( WCHAR ) );

            wcscpy( tmpStr, WindowsGetStringRawBuffer( strings[i], NULL ) );
            wcscat( tmpStr, L"\n" );
            wcscat( combinedString, tmpStr );
            
            free( tmpStr );
        }
    } else {
        free( write_text_options );
        return hr;
    }

    //Remove trailing nextspace
    combinedString[ wcslen(combinedString) - 1 ] = L'\0'; 

    write_text_options->encoding = encoding;
    write_text_options->absolutePath = absolutePath;
    if ( combinedString )
        WindowsCreateString( combinedString, wcslen( combinedString ), &write_text_options->contents );

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_WriteText, operation );

    free( write_text_options );
    if ( combinedString ) 
        free( combinedString );
    IIterator_HSTRING_Release( hstringIterator );
    for ( i = 0; i < vectorSize; i++ )
    {
        if ( strings[i] != NULL )
        {
            WindowsDeleteString( strings[i] );
        }
    }
    free( strings );
    
    return hr;
}

static HRESULT WINAPI path_io_statics_AppendLinesAsync( IPathIOStatics *iface, HSTRING absolutePath, IIterable_HSTRING *lines, IAsyncAction **operation )
{
    //Convert IIterable_HSTRING to HSTRING
    HRESULT hr;
    HSTRING *strings;
    LPWSTR combinedString = NULL;
    LPWSTR tmpStr;
    UINT32 vectorSize = 0;
    UINT32 totalSize = 0;
    UINT32 i;
    boolean strExists;
    IIterator_HSTRING *hstringIterator;

    struct path_io_write_text_options *write_text_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !absolutePath || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = IIterable_HSTRING_First( lines, &hstringIterator );
    if ( SUCCEEDED( hr ) )
    {
        IIterator_HSTRING_get_HasCurrent( hstringIterator, &strExists );
        while ( strExists )
        {
            vectorSize++;
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );
        }

        strings = (HSTRING *)malloc( vectorSize * sizeof( HSTRING ) );
        IIterable_HSTRING_First( lines, &hstringIterator );
        
        for ( i = 0; i < vectorSize; i++ )
        {
            IIterator_HSTRING_get_Current( hstringIterator, &strings[i] );
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );

            totalSize += WindowsGetStringLen( strings[i] ) + 1; 
            combinedString = (LPWSTR)malloc( ( totalSize + 1 ) * sizeof( WCHAR ) );
            combinedString[0] = L'\0';
            tmpStr = (LPWSTR)malloc( ( WindowsGetStringLen( strings[i] ) + 1 ) * sizeof( WCHAR ) );

            wcscpy( tmpStr, WindowsGetStringRawBuffer( strings[i], NULL ) );
            wcscat( tmpStr, L"\n" );
            wcscat( combinedString, tmpStr );
            
            free( tmpStr );
        }
    } else {
        free( write_text_options );
        return hr;
    }

    //Remove trailing nextspace
    combinedString[ wcslen(combinedString) - 1 ] = L'\0'; 

    write_text_options->encoding = UnicodeEncoding_Utf8;
    write_text_options->absolutePath = absolutePath;
    if ( combinedString )
        WindowsCreateString( combinedString, wcslen( combinedString ), &write_text_options->contents );

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_AppendText, operation );

    free( write_text_options );
    if ( combinedString ) 
        free( combinedString );
    IIterator_HSTRING_Release( hstringIterator );
    for ( i = 0; i < vectorSize; i++ )
    {
        if ( strings[i] != NULL )
        {
            WindowsDeleteString( strings[i] );
        }
    }
    free( strings );

    return hr;
}

static HRESULT WINAPI path_io_statics_AppendLinesWithEncodingAsync( IPathIOStatics *iface, HSTRING absolutePath, IIterable_HSTRING *lines, UnicodeEncoding encoding, IAsyncAction **operation )
{
    //Convert IIterable_HSTRING to HSTRING
    HRESULT hr;
    HSTRING *strings;
    LPWSTR combinedString = NULL;
    LPWSTR tmpStr;
    UINT32 vectorSize = 0;
    UINT32 totalSize = 0;
    UINT32 i;
    boolean strExists;
    IIterator_HSTRING *hstringIterator;

    struct path_io_write_text_options *write_text_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !absolutePath || !lines ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_text_options = calloc( 1, sizeof(*write_text_options) ))) return E_OUTOFMEMORY;

    hr = IIterable_HSTRING_First( lines, &hstringIterator );
    if ( SUCCEEDED( hr ) )
    {
        IIterator_HSTRING_get_HasCurrent( hstringIterator, &strExists );
        while ( strExists )
        {
            vectorSize++;
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );
        }

        strings = (HSTRING *)malloc( vectorSize * sizeof( HSTRING ) );
        IIterable_HSTRING_First( lines, &hstringIterator );
        
        for ( i = 0; i < vectorSize; i++ )
        {
            IIterator_HSTRING_get_Current( hstringIterator, &strings[i] );
            IIterator_HSTRING_MoveNext( hstringIterator, &strExists );

            totalSize += WindowsGetStringLen( strings[i] ) + 1; 
            combinedString = (LPWSTR)malloc( ( totalSize + 1 ) * sizeof( WCHAR ) );
            combinedString[0] = L'\0';
            tmpStr = (LPWSTR)malloc( ( WindowsGetStringLen( strings[i] ) + 1 ) * sizeof( WCHAR ) );

            wcscpy( tmpStr, WindowsGetStringRawBuffer( strings[i], NULL ) );
            wcscat( tmpStr, L"\n" );
            wcscat( combinedString, tmpStr );
            
            free( tmpStr );
        }
    } else {
        free( write_text_options );
        return hr;
    }

    //Remove trailing nextspace
    combinedString[ wcslen(combinedString) - 1 ] = L'\0'; 

    write_text_options->encoding = encoding;
    write_text_options->absolutePath = absolutePath;
    if ( combinedString )
        WindowsCreateString( combinedString, wcslen( combinedString ), &write_text_options->contents );

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_text_options, path_io_statics_AppendText, operation );

    free( write_text_options );
    if ( combinedString ) 
        free( combinedString );
    IIterator_HSTRING_Release( hstringIterator );
    for ( i = 0; i < vectorSize; i++ )
    {
        if ( strings[i] != NULL )
        {
            WindowsDeleteString( strings[i] );
        }
    }
    free( strings );

    return hr;
}

static HRESULT WINAPI path_io_statics_ReadBufferAsync( IPathIOStatics *iface, HSTRING absolutePath, IAsyncOperation_IBuffer **operation )
{
    HRESULT hr;

    struct async_operation_iids iids = { .operation = &IID_IAsyncOperation_IBuffer };

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !absolutePath ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    hr = async_operation_create( (IUnknown *)iface, (IUnknown *)absolutePath, path_io_statics_ReadBuffer, iids, (IAsyncOperation_IInspectable **)operation );
    TRACE( "created IAsyncOperation_IBuffer %p.\n", *operation );

    return hr;
}

static HRESULT WINAPI path_io_statics_WriteBufferAsync( IPathIOStatics *iface, HSTRING absolutePath, IBuffer* buffer, IAsyncAction **operation )
{
    HRESULT hr;
    struct path_io_write_buffer_options *write_buffer_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    //Arguments
    if ( !absolutePath || !buffer ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    if (!(write_buffer_options = calloc( 1, sizeof(*write_buffer_options) ))) return E_OUTOFMEMORY;

    write_buffer_options->buffer = buffer;
    write_buffer_options->absolutePath = absolutePath;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_buffer_options, path_io_statics_WriteBuffer, operation );

    free( write_buffer_options );
    return hr;
}

static HRESULT WINAPI path_io_statics_WriteBytesAsync( IPathIOStatics *iface, HSTRING absolutePath, UINT32 __bufferSize, BYTE *buffer, IAsyncAction **operation )
{
    HRESULT hr;
    struct path_io_write_bytes_options *write_bytes_options;

    TRACE( "iface %p, operation %p\n", iface, operation );

    if (!(write_bytes_options = calloc( 1, sizeof(*write_bytes_options) ))) return E_OUTOFMEMORY;

    //Arguments
    if ( !absolutePath || !buffer ) return E_INVALIDARG;
    if ( !operation ) return E_POINTER;

    write_bytes_options->buffer = buffer;
    write_bytes_options->absolutePath = absolutePath;
    write_bytes_options->bufferSize = __bufferSize;

    hr = async_action_create( (IUnknown *)iface, (IUnknown *)write_bytes_options, path_io_statics_WriteBytes, operation );

    free( write_bytes_options );
    return hr;
}

static const struct IPathIOStaticsVtbl path_io_statics_vtbl =
{
    path_io_statics_QueryInterface,
    path_io_statics_AddRef,
    path_io_statics_Release,
    /* IInspectable methods */
    path_io_statics_GetIids,
    path_io_statics_GetRuntimeClassName,
    path_io_statics_GetTrustLevel,
    /* IPathIOStatics methods */
    path_io_statics_ReadTextAsync,
    path_io_statics_ReadTextWithEncodingAsync,
    path_io_statics_WriteTextAsync,
    path_io_statics_WriteTextWithEncodingAsync,
    path_io_statics_AppendTextAsync,
    path_io_statics_AppendTextWithEncodingAsync,
    path_io_statics_ReadLinesAsync,
    path_io_statics_ReadLinesWithEncodingAsync,
    path_io_statics_WriteLinesAsync,
    path_io_statics_WriteLinesWithEncodingAsync,
    path_io_statics_AppendLinesAsync,
    path_io_statics_AppendLinesWithEncodingAsync,
    path_io_statics_ReadBufferAsync,
    path_io_statics_WriteBufferAsync,
    path_io_statics_WriteBytesAsync
};

static struct path_io_statics path_io_statics =
{
    {&factory_vtbl},
    {&path_io_statics_vtbl},
    1,
};

IActivationFactory *path_io_factory = &path_io_statics.IActivationFactory_iface;
