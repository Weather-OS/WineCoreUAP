/* WinRT Windows.Storage.Search.IStorageQueryResults Implementation
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

#include "StorageQueryResultsInternal.h"

_ENABLE_DEBUGGING_

DEFINE_ASYNC_COMPLETED_HANDLER( storage_item_vector_view, IAsyncOperationCompletedHandler_IVectorView_IStorageItem, IAsyncOperation_IVectorView_IStorageItem );

HRESULT WINAPI query_result_base_ConductSearch( IStorageQueryResultBase *iface, IVector_IStorageItem **items )
{
    struct query_result_base *impl = impl_from_IStorageQueryResultBase( iface );

    IQueryOptions *options = impl->Options;
    IStorageFolder *folder = impl->Folder;
    IStorageFolder *queryFolder = NULL;
    IStorageFile *file = NULL;
    IStorageItem *queryItem = NULL;
    FolderDepth depth;

    WIN32_FIND_DATAW findFileData;
    HRESULT status = S_OK;
    HSTRING folderPath;
    HSTRING filePath;
    HSTRING ApplicationFilter;
    HSTRING UserFilter;
    HSTRING fileName;
    HSTRING folderName;
    HANDLE hFind;
    WCHAR searchPath[MAX_PATH];
    WCHAR fullPath[MAX_PATH];

    //non-runtime class require redefinition
    #undef IStorageItem
    DEFINE_VECTOR_IIDS( IStorageItem )
    #define IStorageItem __x_ABI_CWindows_CStorage_CIStorageItem

    TRACE( "iface %p, items %p\n", iface, items );

    status = vector_create( &IStorageItem_iids, (void **)items );
    if ( FAILED( status ) ) return status;

    IQueryOptions_get_FolderDepth( options, &depth );
    IQueryOptions_get_ApplicationSearchFilter( options, &ApplicationFilter );
    IQueryOptions_get_UserSearchFilter( options, &UserFilter );

    swprintf( searchPath, sizeof(searchPath), L"%s\\*.*", WindowsGetStringRawBuffer( impl_from_IStorageFolder( folder )->Path, NULL ) );
    hFind = FindFirstFileW( searchPath, &findFileData );

    if (hFind == INVALID_HANDLE_VALUE) 
    {
        return E_ABORT;
    }

    while ( FindNextFileW( hFind, &findFileData ) != 0 )
    {
        if ( wcscmp( findFileData.cFileName, L"." ) != 0 && wcscmp( findFileData.cFileName, L".." ) != 0 ) 
        {
            swprintf( fullPath, sizeof(fullPath), L"%s\\%s", WindowsGetStringRawBuffer( impl_from_IStorageFolder( folder )->Path, NULL), findFileData.cFileName );
            if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) 
            {                    
                WindowsCreateString( fullPath, wcslen( fullPath ), &folderPath );
                status = storage_folder_AssignFolder( folderPath, &queryFolder );
                if ( FAILED( status ) ) return status;

                if ( impl->searchType == StorageItemTypes_Folder || impl->searchType == StorageItemTypes_None )
                {
                    //AQS is not supported yet.
                    IStorageFolder_QueryInterface( queryFolder, &IID_IStorageItem, (void **)&queryItem );
                    IStorageItem_get_Name( queryItem, &folderName );
                    if ( wcsstr( WindowsGetStringRawBuffer( folderName, NULL), WindowsGetStringRawBuffer( ApplicationFilter, NULL ) ) 
                         && wcsstr( WindowsGetStringRawBuffer( folderName, NULL), WindowsGetStringRawBuffer( UserFilter, NULL ) ) )
                    {
                        status = IStorageFolder_QueryInterface( queryFolder, &IID_IStorageItem, (void **)&queryItem );
                        if ( FAILED( status ) ) return status;
                        status = IVector_IStorageItem_Append( *items, queryItem );
                        if ( FAILED( status ) ) return status;
                    }
                }
                if ( depth == FolderDepth_Deep )
                    query_result_base_ConductSearch( iface, items );
            } 
            else 
            {
                WindowsCreateString( fullPath, wcslen( fullPath ), &filePath );
                status = storage_file_AssignFile( filePath, &file );
                if ( FAILED( status ) ) return status;

                if ( impl->searchType == StorageItemTypes_File || impl->searchType == StorageItemTypes_None )
                {                
                    //AQS is not supported yet.
                    IStorageFile_QueryInterface( file, &IID_IStorageItem, (void **)&queryItem );
                    IStorageItem_get_Name( queryItem, &fileName );
                    if ( wcsstr( WindowsGetStringRawBuffer( fileName, NULL), WindowsGetStringRawBuffer( ApplicationFilter, NULL ) ) 
                         && wcsstr( WindowsGetStringRawBuffer( fileName, NULL), WindowsGetStringRawBuffer( UserFilter, NULL ) ) )
                    {
                        status = IVector_IStorageItem_Append( *items, queryItem );
                        if ( FAILED( status ) ) return status;
                    }
                }
            }
        }
    }
    FindClose( hFind );

    return status;
}

HRESULT WINAPI query_result_base_SearchCountAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{    
    IVector_IStorageItem *items;

    HRESULT status = S_OK;
    UINT32 size;

    TRACE( "invoker %p, result %p\n", invoker, result );

    status = query_result_base_ConductSearch( (IStorageQueryResultBase *)invoker, &items );
    if ( FAILED( status ) ) return status;

    status = IVector_IStorageItem_get_Size( items, &size );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UI4;
        result->ulVal = (ULONG)size;
    } else
        IVector_IStorageItem_Release( items );

    return status;
}

HRESULT WINAPI query_result_base_FindFirstAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    IAsyncOperation_IVectorView_IStorageItem *folderItemsOperation = NULL;
    IVectorView_IStorageItem *folderItems = NULL;
    IVector_IStorageItem *items = NULL;
    IStorageFolder *folder = impl_from_IStorageQueryResultBase( (IStorageQueryResultBase *)invoker )->Folder;
    IStorageItem *firstItem = NULL;
    IStorageItem *currentItem = NULL;

    HRESULT status = S_OK;
    HSTRING currentItemName;
    HSTRING firstItemName;
    UINT32 size;
    UINT32 iterator;
    INT32 comparisonResult;
    DWORD asyncRes;

    TRACE( "invoker %p, result %p\n", invoker, result );

    status = query_result_base_ConductSearch( (IStorageQueryResultBase *)invoker, &items );
    if ( FAILED( status ) ) return status;

    status = IVector_IStorageItem_GetAt( items, 0, &firstItem );
    if ( FAILED( status ) ) return status;

    IStorageItem_get_Name( firstItem, &firstItemName );

    if ( firstItem )
    {
        status = IStorageFolder_GetItemsAsyncOverloadDefaultStartAndCount( folder, &folderItemsOperation );
        if ( FAILED( status ) ) return status;

        asyncRes = await_IAsyncOperation_IVectorView_IStorageItem( folderItemsOperation, INFINITE );
        if ( asyncRes ) return E_ABORT;

        status = IAsyncOperation_IVectorView_IStorageItem_GetResults( folderItemsOperation, &folderItems );
        IAsyncOperation_IVectorView_IStorageItem_Release( folderItemsOperation );
        if ( FAILED( status ) ) return status;

        CHECK_LAST_RESTRICTED_ERROR();

        IVectorView_IStorageItem_get_Size( folderItems, &size );
        
        for ( iterator = 0; iterator < size; iterator++ )
        {
            IVectorView_IStorageItem_GetAt( folderItems, iterator, &currentItem );
            IStorageItem_get_Name( currentItem, &currentItemName );
            WindowsCompareStringOrdinal( currentItemName, firstItemName, &comparisonResult );
            if ( !comparisonResult )
            {
                result->vt = VT_UI4;
                result->ulVal = (ULONG)iterator;
                break;
            }
        }
    }

    return status;
}

HRESULT WINAPI query_result_base_FetchFoldersAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    IStorageItem *currentItem = NULL;
    IStorageFolder *currentFolder = NULL;
    IVector_IStorageItem *items = NULL;
    IVector_StorageFolder *folders = NULL;
    IVectorView_StorageFolder *foldersView = NULL;

    struct folder_query_result *query_result = impl_from_IStorageFolderQueryResult( (IStorageFolderQueryResult *)invoker );

    HRESULT status = S_OK;
    UINT32 currentSize = 0;
    UINT32 iterator;
    UINT32 size;

    //Parameters
    struct storage_query_options *options = (struct storage_query_options *)param;
    UINT32 startIndex = options->startIndex;
    UINT32 maxNumberOfItems = options->maxNumberOfItems;

    DEFINE_VECTOR_IIDS( StorageFolder )
    status = vector_create( &StorageFolder_iids, (void **)&folders );

    TRACE( "invoker %p, result %p\n", invoker, result );

    status = query_result_base_ConductSearch( &query_result->IStorageQueryResultBase_iface, &items );
    if ( FAILED( status ) ) return status;

    IVector_IStorageItem_get_Size( items, &size );

    if ( startIndex >= size ) return E_BOUNDS;

    for ( iterator = startIndex; iterator < size; iterator++ )
    {
        if ( currentSize == maxNumberOfItems ) break;
        IVector_IStorageItem_GetAt( items, iterator, &currentItem );
        
        status = IStorageItem_QueryInterface( currentItem, &IID_IStorageFolder, (void **)&currentFolder );
        if ( FAILED( status ) ) return status;
        status = IVector_StorageFolder_Append( folders, currentFolder );
        if ( FAILED( status ) ) return status;

        currentSize++;
    }

    status = IVector_StorageFolder_GetView( folders, &foldersView );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)foldersView;
    }

    return status;
}

HRESULT WINAPI query_result_base_FetchFilesAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    IStorageItem *currentItem = NULL;
    IStorageFile *currentFile = NULL;
    IVector_IStorageItem *items = NULL;
    IVector_StorageFile *files = NULL;
    IVectorView_StorageFile *filesView = NULL;

    struct file_query_result *query_result = impl_from_IStorageFileQueryResult( (IStorageFileQueryResult *)invoker );

    HRESULT status = S_OK;
    UINT32 currentSize = 0;
    UINT32 iterator;
    UINT32 size;

    //Parameters
    struct storage_query_options *options = (struct storage_query_options *)param;
    UINT32 startIndex = options->startIndex;
    UINT32 maxNumberOfItems = options->maxNumberOfItems;

    DEFINE_VECTOR_IIDS( StorageFile )
    status = vector_create( &StorageFile_iids, (void **)&files );

    TRACE( "invoker %p, result %p\n", invoker, result );

    status = query_result_base_ConductSearch( &query_result->IStorageQueryResultBase_iface, &items );
    if ( FAILED( status ) ) return status;

    IVector_IStorageItem_get_Size( items, &size );

    if ( startIndex >= size ) return E_BOUNDS;

    for ( iterator = startIndex; iterator < size; iterator++ )
    {
        if ( currentSize == maxNumberOfItems ) break;
        IVector_IStorageItem_GetAt( items, iterator, &currentItem );
        
        status = IStorageItem_QueryInterface( currentItem, &IID_IStorageFile, (void **)&currentFile );
        if ( FAILED( status ) ) return status;
        status = IVector_StorageFile_Append( files, currentFile );
        if ( FAILED( status ) ) return status;

        currentSize++;
    }

    status = IVector_StorageFile_GetView( files, &filesView );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)filesView;
    }

    return status;
}

HRESULT WINAPI query_result_base_FetchItemsAsync( IUnknown *invoker, IUnknown *param, PROPVARIANT *result )
{
    IStorageItem *currentItem = NULL;
    IVector_IStorageItem *items = NULL;
    IVector_IStorageItem *fetchedItems = NULL;
    IVectorView_IStorageItem *itemsView = NULL;

    struct item_query_result *query_result = impl_from_IStorageItemQueryResult( (IStorageItemQueryResult *)invoker );

    HRESULT status = S_OK;
    UINT32 currentSize = 0;
    UINT32 iterator;
    UINT32 size;

    //Parameters
    struct storage_query_options *options = (struct storage_query_options *)param;
    UINT32 startIndex = options->startIndex;
    UINT32 maxNumberOfItems = options->maxNumberOfItems;

    //non-runtime class require redefinition
    #undef IStorageItem
    DEFINE_VECTOR_IIDS( IStorageItem )
    #define IStorageItem __x_ABI_CWindows_CStorage_CIStorageItem

    TRACE( "invoker %p, result %p\n", invoker, result );

    status = vector_create( &IStorageItem_iids, (void **)fetchedItems );
    if ( FAILED( status ) ) return status;

    status = query_result_base_ConductSearch( &query_result->IStorageQueryResultBase_iface, &items );
    if ( FAILED( status ) ) return status;

    IVector_IStorageItem_get_Size( items, &size );

    if ( startIndex >= size ) return E_BOUNDS;

    for ( iterator = startIndex; iterator < size; iterator++ )
    {
        if ( currentSize == maxNumberOfItems ) break;
        IVector_IStorageItem_GetAt( items, iterator, &currentItem );

        status = IVector_IStorageItem_Append( fetchedItems, currentItem );
        if ( FAILED( status ) ) return status;

        currentSize++;
    }

    status = IVector_IStorageItem_GetView( fetchedItems, &itemsView );

    if ( SUCCEEDED( status ) )
    {
        result->vt = VT_UNKNOWN;
        result->punkVal = (IUnknown *)itemsView;
    }

    return status;
}