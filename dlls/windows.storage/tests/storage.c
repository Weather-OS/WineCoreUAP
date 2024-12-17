/*
 * Written by Weather
 *
 * This is a reverse engineered implementation of Microsoft's OneCoreUAP binaries.
 *
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

#include "initguid.h"
#include "test.h"

DEFINE_ASYNC_COMPLETED_HANDLER( basic_properties_handler, IAsyncOperationCompletedHandler_BasicProperties, IAsyncOperation_BasicProperties )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_folder_handler, IAsyncOperationCompletedHandler_StorageFolder, IAsyncOperation_StorageFolder )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_folder_vector_view_handler, IAsyncOperationCompletedHandler_IVectorView_StorageFolder, IAsyncOperation_IVectorView_StorageFolder )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_file_handler, IAsyncOperationCompletedHandler_StorageFile, IAsyncOperation_StorageFile )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_file_vector_view_handler, IAsyncOperationCompletedHandler_IVectorView_StorageFile, IAsyncOperation_IVectorView_StorageFile )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_item_handler, IAsyncOperationCompletedHandler_IStorageItem, IAsyncOperation_IStorageItem )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_item_vector_view_handler, IAsyncOperationCompletedHandler_IVectorView_IStorageItem, IAsyncOperation_IVectorView_IStorageItem )
DEFINE_ASYNC_COMPLETED_HANDLER( async_action_handler, IAsyncActionCompletedHandler, IAsyncAction );

LPCSTR HSTRINGToLPCSTR( HSTRING hstr ) {
    int requiredSize;
    char* pszString;
    int convertedSize;
    const wchar_t* wszString;
    UINT32 length;
    wszString = WindowsGetStringRawBuffer( hstr, &length );
    requiredSize = WideCharToMultiByte(CP_UTF8, 0, wszString, length, NULL, 0, NULL, NULL);
    if (requiredSize == 0) {
        return NULL;
    }
    pszString = (char*)malloc(requiredSize + 1);
    if (!pszString) {
        return NULL;
    }
    convertedSize = WideCharToMultiByte(CP_UTF8, 0, wszString, length, pszString, requiredSize, NULL, NULL);
    if (convertedSize == 0) {
        free(pszString);
        return NULL;
    }
    pszString[convertedSize] = '\0';
    return pszString;
}

void check_interface_( unsigned int line, void *obj, const IID *iid )
{
    IUnknown *iface = obj;
    IUnknown *unk;
    HRESULT hr;

    hr = IUnknown_QueryInterface( iface, iid, (void **)&unk );
    ok_(__FILE__, line)( hr == S_OK, "got hr %#lx.\n", hr );
    IUnknown_Release( unk );
}

void stubbed_interface_( unsigned int line, void *obj, const IID *iid )
{
    IUnknown *iface = obj;
    IUnknown *unk;
    HRESULT hr;

    hr = IUnknown_QueryInterface( iface, iid, (void **)&unk );
    ok_(__FILE__, line)( hr == E_NOINTERFACE, "%s is already implemented!\n", wine_dbgstr_guid( iid ) );
}

/**
 * ABI::Windows::Storage::AppDataPaths
 */

void test_AppDataPathsStatics( const wchar_t** pathStr )
{    
    static const WCHAR *app_data_paths_statics_name = L"Windows.Storage.AppDataPaths";
   
    IAppDataPathsStatics *app_data_paths_statics = NULL;    
    IAppDataPaths *app_data_paths = NULL;

    HRESULT hr;
    HSTRING localAppDataPath;
    LONG ref;

    ACTIVATE_INSTANCE( app_data_paths_statics_name, app_data_paths_statics, IID_IAppDataPathsStatics );

    /**
     * ABI::Windows::Storage::IAppDataPathsStatics::GetDefault
     */
    hr = IAppDataPathsStatics_GetDefault( app_data_paths_statics, &app_data_paths );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IAppDataPaths::LocalAppData
     */
    IAppDataPaths_get_LocalAppData( app_data_paths, &localAppDataPath );
    trace( "Windows::Storage::IAppDataPaths::LocalAppData for %p is %s\n", &app_data_paths, HSTRINGToLPCSTR( localAppDataPath ) );

    *pathStr = WindowsGetStringRawBuffer(localAppDataPath, NULL);

    if (app_data_paths) IAppDataPaths_Release( app_data_paths );
    ref = IAppDataPathsStatics_Release( app_data_paths_statics );
    ok( ref == 1, "got ref %ld.\n", ref );
}

/**
 * ABI::Windows::Storage::IStorageItem
 */

void test_StorageItem( IStorageItem *item )
{
    DateTime time;
    FileAttributes attributes;

    IAsyncAction *action = NULL;

    IStorageItem2 *item2 = NULL;

    IStorageProvider *provider = NULL;
    IStorageItemProperties *item_properties = NULL;
    IStorageItemPropertiesWithProvider *item_properites_with_provider = NULL;

    IBasicProperties *properties = NULL;
    IAsyncOperation_BasicProperties *properties_operation = NULL;

    IStorageFolder *parentFolder = NULL;
    IAsyncOperation_StorageFolder *storage_folder_operation = NULL;

    HSTRING folderRelativeId;
    HSTRING displaytype;
    HSTRING secondName;
    HSTRING origName;    
    HSTRING itemId;
    HSTRING rename;
    HSTRING path;

    BOOLEAN isEqual;
    HRESULT hr = S_OK;
    UINT64 itemSize;
    INT32 compResult;
    DWORD asyncRes;

    /**
     * ABI::Windows::Storage::IStorageItem
     */
    hr = IStorageItem_QueryInterface( item, &IID_IStorageItem, (void **)&item );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItem::Name
     */
    IStorageItem_get_Name( item, &origName );
    trace( "Windows::Storage::IStorageItem::Name for %p is %s\n", &item, HSTRINGToLPCSTR( origName ) );

    /**
     * ABI::Windows::Storage::IStorageItem::Path
     */
    IStorageItem_get_Path( item, &path );
    trace( "Windows::Storage::IStorageItem::Path for %p is %s\n", &item, HSTRINGToLPCSTR( path ) );

    /**
     * ABI::Windows::Storage::IStorageItem::DateCreated
     */
    IStorageItem_get_DateCreated( item, &time );
    trace( "Windows::Storage::IStorageItem::DateCreated.UniversalTime for %p is %lld\n", &item, time.UniversalTime );

    /**
     * ABI::Windows::Storage::IStorageItem::Attributes
     */
    IStorageItem_get_Attributes( item, &attributes );
    trace( "Windows::Storage::IStorageItem::Attributes for %p is %d\n", &item, attributes );

    /**
     * ABI::Windows::Storage::IStorageItem::RenameAsync
     */
    WindowsCreateString( L"TestRename", wcslen( L"TestRename" ), &rename );
    hr = IStorageItem_RenameAsync( item, rename, NameCollisionOption_ReplaceExisting, &action );
    CHECK_HR( hr );

    asyncRes = await_IAsyncAction( action, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IStorageItem_get_Name( item, &secondName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( secondName, origName, &compResult );
    ok( compResult, "the string rename is the same as origName! compResult %d\n", compResult );

    hr = IStorageItem_RenameAsync( item, origName, NameCollisionOption_ReplaceExisting, &action );
    CHECK_HR( hr );

    asyncRes = await_IAsyncAction( action, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    /**
     * ABI::Windows::Storage::IStorageItem::GetBasicPropertiesAsync
     */
    hr = IStorageItem_GetBasicPropertiesAsync( item, &properties_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_BasicProperties( properties_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_BasicProperties_GetResults( properties_operation, &properties );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IBasicProperties::Size
     */
    IBasicProperties_get_Size( properties, &itemSize );
    trace( "Windows::Storage::IBasicProperties::Size for %p is %lld\n", &properties, itemSize );

    /**
     * ABI::Windows::Storage::IBasicProperties::DateModified
     */
    IBasicProperties_get_DateModified( properties, &time );
    trace( "Windows::Storage::IBasicProperties::DateModified.UniversalTime for %p is %lld\n", &properties, time.UniversalTime );

    /**
     * ABI::Windows::Storage::IBasicProperties::ItemDate
     */
    IBasicProperties_get_ItemDate( properties, &time );
    trace( "Windows::Storage::IBasicProperties::DateModified.ItemDate for %p is %lld\n", &properties, time.UniversalTime );

    /**
     * ABI::Windows::Storage::IStorageItem2
     */
    hr = IStorageItem_QueryInterface( item, &IID_IStorageItem2, (void **)&item2 );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItem2::GetParentAsync
     */
    hr = IStorageItem2_GetParentAsync( item2, &storage_folder_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFolder( storage_folder_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFolder_GetResults( storage_folder_operation, &parentFolder );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItem2::IsEqual
     */
    hr = IStorageItem2_IsEqual( item2, item, &isEqual );
    CHECK_HR( hr );
    ok( isEqual, "item %p unexpectedly is not equal to item2 %p\n", &item, &item2 );

    /**
     * ABI::Windows::Storage::IStorageItemProperties
     */
    hr = IStorageItem_QueryInterface( item, &IID_IStorageItemProperties, (void **)&item_properties );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItemProperties::DisplayName
     */
    IStorageItemProperties_get_DisplayName( item_properties, &origName );
    trace( "Windows::Storage::IStorageItemProperties::DisplayName for %p is %s\n", &item_properties, HSTRINGToLPCSTR( origName ) );

    /**
     * ABI::Windows::Storage::IStorageItemProperties::DisplayType
     */
    IStorageItemProperties_get_DisplayType( item_properties, &displaytype );
    trace( "Windows::Storage::IStorageItemProperties::DisplayType for %p is %s\n", &item_properties, HSTRINGToLPCSTR( displaytype ) );

    /**
     * ABI::Windows::Storage::IStorageItemProperties::FolderRelativeId
     */
    IStorageItemProperties_get_FolderRelativeId( item_properties, &folderRelativeId );
    trace( "Windows::Storage::IStorageItemProperties::FolderRelativeId for %p is %s\n", &item_properties, HSTRINGToLPCSTR( folderRelativeId ) );

    /**
     * ABI::Windows::Storage::IStorageItemPropertiesWithProvider
     */
    hr = IStorageItem_QueryInterface( item, &IID_IStorageItemPropertiesWithProvider, (void **)&item_properites_with_provider );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItemPropertiesWithProvider::Provider
     */
    hr = IStorageItemPropertiesWithProvider_get_Provider( item_properites_with_provider, &provider );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItemPropertiesWithProvider::Id
     */
    IStorageProvider_get_Id( provider, &itemId );
    trace( "Windows::Storage::IStorageItemPropertiesWithProvider::Id for %p is %s\n", &provider, HSTRINGToLPCSTR( itemId ) );

    /**
     * ABI::Windows::Storage::IStorageItemPropertiesWithProvider::DisplayName
     */
    IStorageProvider_get_DisplayName( provider, &origName );
    trace( "Windows::Storage::IStorageItemPropertiesWithProvider::DisplayName for %p is %s\n", &provider, HSTRINGToLPCSTR( origName ) );
}

/**
 * ABI::Windows::Storage::StorageFolder
 */

void test_StorageFolder( const wchar_t* path, IStorageItem **item, IStorageFile **file )
{
    static const WCHAR *storage_folder_statics_name = L"Windows.Storage.StorageFolder";

    IStorageFolderStatics *storage_folder_statics = NULL;

    IStorageItem *storage_item = NULL;
    IAsyncOperation_IStorageItem *storage_item_operation = NULL;
    IVectorView_IStorageItem *storage_item_vector_view = NULL;
    IAsyncOperation_IVectorView_IStorageItem *storage_item_vector_view_operation = NULL;

    IStorageFile *storage_file = NULL;
    IAsyncOperation_StorageFile *storage_file_operation = NULL;
    IVectorView_StorageFile *storage_file_vector_view = NULL;
    IAsyncOperation_IVectorView_StorageFile *storage_file_vector_view_operation = NULL;

    IStorageFolder *storage_folder = NULL;
    IStorageFolder *sub_storage_folder = NULL;
    IStorageFolder2 *storage_folder_2 = NULL;
    IAsyncOperation_StorageFolder *storage_folder_operation = NULL;
    IVectorView_StorageFolder *storage_folder_vector_view = NULL;
    IAsyncOperation_IVectorView_StorageFolder *storage_folder_vector_view_operation = NULL;

    IStorageFolderQueryOperations *storage_folder_query_operations = NULL;

    HSTRING pathString;
    HSTRING tmpString;
    HSTRING itemName;
    
    HRESULT hr;
    DWORD asyncRes;

    BOOLEAN found;
    UINT32 size;
    UINT32 index;
    INT32 compResult;
    LONG ref;

    ACTIVATE_INSTANCE( storage_folder_statics_name, storage_folder_statics, IID_IStorageFolderStatics );

    /**
     * ABI::Windows::Storage::IStorageFolderStatics::GetFolderFromPathAsync
     */
    WindowsCreateString( path, wcslen( path ), &pathString );
    hr = IStorageFolderStatics_GetFolderFromPathAsync( storage_folder_statics, pathString, &storage_folder_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFolder( storage_folder_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFolder_GetResults( storage_folder_operation, &storage_folder );
    CHECK_HR( hr );

    check_interface( storage_folder, &IID_IUnknown );
    check_interface( storage_folder, &IID_IInspectable );
    check_interface( storage_folder, &IID_IAgileObject );

    /** --- (@windows.storage.idl) ---
     * StorageFolder runtime class expects: 
     * UniversalAPIContract 1.0: 
     *      IStorageFolder 
     *      IStorageItem 
     *      IStorageFolderQueryOperations 
     *      IStorageItemProperties
     *      IStorageItemProperties2
     *      IStorageItem2
     *      IStorageFolder2
     *      IStorageItemPropertiesWithProvider
     * 
     * UniversalAPIContract 6.0:
     *      IStorageFolder3
     */

    check_interface( storage_folder, &IID_IStorageFolder );
    check_interface( storage_folder, &IID_IStorageItem );
    check_interface( storage_folder, &IID_IStorageFolderQueryOperations ); 
    check_interface( storage_folder, &IID_IStorageItemProperties );
    //notimpl: check_interface( storage_folder, &IID_IStorageItemProperties2 );
    check_interface( storage_folder, &IID_IStorageItem2 );
    check_interface( storage_folder, &IID_IStorageFolder2 );
    check_interface( storage_folder, &IID_IStorageItemPropertiesWithProvider );
    //notimpl: check_interface( storage_folder, &IID_IStorageFolder3 );

    /**
     * ABI::Windows::Storage::IStorageFolder
     */
    hr = IStorageFolder_QueryInterface( storage_folder, &IID_IStorageFolder, (void **)&storage_folder );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageFolder::CreateFileAsync
     */
    WindowsCreateString( L"TestFile.tmp", wcslen( L"TestFile.tmp" ), &tmpString );
    hr = IStorageFolder_CreateFileAsync( storage_folder, tmpString, CreationCollisionOption_ReplaceExisting, &storage_file_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFile( storage_file_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFile_GetResults( storage_file_operation, &storage_file );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageFolder::CreateFolderAsync
     */
    WindowsCreateString( L"TestFolder", wcslen( L"TestFolder" ), &tmpString );
    hr = IStorageFolder_CreateFolderAsync( storage_folder, tmpString, CreationCollisionOption_ReplaceExisting, &storage_folder_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFolder( storage_folder_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFolder_GetResults( storage_folder_operation, &sub_storage_folder );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageFolder::GetFileAsync
     */
    WindowsCreateString( L"TestFile.tmp", wcslen( L"TestFile.tmp" ), &tmpString );
    hr = IStorageFolder_GetFileAsync( storage_folder, tmpString, &storage_file_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFile( storage_file_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFile_GetResults( storage_file_operation, &storage_file );
    CHECK_HR( hr );

    hr = IStorageFile_QueryInterface( storage_file, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    /**
     * ABI::Windows::Storage::IStorageFolder::GetFilesAsync
     */
    hr = IStorageFolder_GetFilesAsyncOverloadDefaultOptionsStartAndCount( storage_folder, &storage_file_vector_view_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_IVectorView_StorageFile( storage_file_vector_view_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IVectorView_StorageFile_GetResults( storage_file_vector_view_operation, &storage_file_vector_view );
    CHECK_HR( hr );

    hr = IVectorView_StorageFile_get_Size( storage_file_vector_view, &size );
    CHECK_HR( hr );
    ok( size == 1u, "unexpected size of %u\n", size );

    hr = IVectorView_StorageFile_GetAt( storage_file_vector_view, 0, &storage_file );
    CHECK_HR( hr );

    hr = IVectorView_StorageFile_IndexOf( storage_file_vector_view, storage_file, &index, &found );
    CHECK_HR( hr );
    ok( found, "storage file element (%p) not found in vector!\n", storage_file );
    ok( index == 0u, "index (%u) is not at 0!\n", index );

    hr = IStorageFile_QueryInterface( storage_file, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    /**
     * ABI::Windows::Storage::IStorageFolder::GetFolderAsync
     */
    WindowsCreateString( L"TestFolder", wcslen( L"TestFolder" ), &tmpString );
    hr = IStorageFolder_GetFolderAsync( storage_folder, tmpString, &storage_folder_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFolder( storage_folder_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFolder_GetResults( storage_folder_operation, &sub_storage_folder );
    CHECK_HR( hr );

    hr = IStorageFolder_QueryInterface( sub_storage_folder, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    /**
     * ABI::Windows::Storage::IStorageFolder::GetFoldersAsync
     */
    hr = IStorageFolder_GetFoldersAsyncOverloadDefaultOptionsStartAndCount( storage_folder, &storage_folder_vector_view_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_IVectorView_StorageFolder( storage_folder_vector_view_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IVectorView_StorageFolder_GetResults( storage_folder_vector_view_operation, &storage_folder_vector_view );
    CHECK_HR( hr );

    hr = IVectorView_StorageFolder_GetAt( storage_folder_vector_view, 0, &sub_storage_folder );
    CHECK_HR( hr );

    hr = IStorageFolder_QueryInterface( sub_storage_folder, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    /**
     * ABI::Windows::Storage::IStorageFolder::GetItemAsync
     */
    WindowsCreateString( L"TestFile.tmp", wcslen( L"TestFile.tmp" ), &tmpString );
    hr = IStorageFolder_GetItemAsync( storage_folder, tmpString, &storage_item_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_IStorageItem( storage_item_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IStorageItem_GetResults( storage_item_operation, &storage_item );
    CHECK_HR( hr );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    /**
     * ABI::Windows::Storage::IStorageFolder::GetItemsAsync
     */
    hr = IStorageFolder_GetItemsAsyncOverloadDefaultStartAndCount( storage_folder, &storage_item_vector_view_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_IVectorView_IStorageItem( storage_item_vector_view_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IVectorView_IStorageItem_GetResults( storage_item_vector_view_operation, &storage_item_vector_view );
    CHECK_HR( hr );

    hr = IVectorView_IStorageItem_GetAt( storage_item_vector_view, 0, &storage_item );
    CHECK_HR( hr );

    check_interface( storage_item, &IID_IStorageFile );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    hr = IVectorView_IStorageItem_GetAt( storage_item_vector_view, 1, &storage_item );
    CHECK_HR( hr );

    check_interface( storage_item, &IID_IStorageFolder );

    WindowsCreateString( L"TestFolder", wcslen( L"TestFolder" ), &tmpString );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    /**
     * ABI::Windows::Storage::IStorageFolder2
     */
    hr = IStorageFolder_QueryInterface( storage_folder, &IID_IStorageFolder2, (void **)&storage_folder_2 );
    CHECK_HR ( hr );

    /**
     * ABI::Windows::Storage::IStorageFolder2::TryGetItemAsync
     */
    WindowsCreateString( L"TestFolder", wcslen( L"TestFolder" ), &tmpString );
    hr = IStorageFolder2_TryGetItemAsync( storage_folder_2, tmpString, &storage_item_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_IStorageItem( storage_item_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IStorageItem_GetResults( storage_item_operation, &storage_item );
    CHECK_HR( hr );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );

    /**
     * ABI::Windows::Storage::IStorageFolderQueryOperations
     */
    hr = IStorageFolder_QueryInterface( storage_folder, &IID_IStorageFolderQueryOperations, (void **)&storage_folder_query_operations );
    CHECK_HR ( hr );

    /**
     * ABI::Windows::Storage::IStorageFolderQueryOperations::GetFilesAsync
     */
    //This tests pretty much everything relating to default behaviors of Windows.Storage.Search
    hr = IStorageFolderQueryOperations_GetFilesAsyncOverloadDefaultStartAndCount( storage_folder_query_operations, CommonFileQuery_DefaultQuery, &storage_file_vector_view_operation );
    CHECK_HR ( hr );

    asyncRes = await_IAsyncOperation_IVectorView_StorageFile( storage_file_vector_view_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IVectorView_StorageFile_GetResults( storage_file_vector_view_operation, &storage_file_vector_view );
    CHECK_HR( hr );

    hr = IVectorView_StorageFile_get_Size( storage_file_vector_view, &size );
    CHECK_HR( hr );
    ok( size == 1u, "unexpected size of %u\n", size );

    hr = IVectorView_StorageFile_GetAt( storage_file_vector_view, 0, &storage_file );
    CHECK_HR( hr );

    hr = IVectorView_StorageFile_IndexOf( storage_file_vector_view, storage_file, &index, &found );
    CHECK_HR( hr );
    ok( found, "storage file element (%p) not found in vector!\n", storage_file );
    ok( index == 0u, "index (%u) is not at 0!\n", index );

    hr = IStorageFile_QueryInterface( storage_file, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageFolderQueryOperations::GetFoldersAsync
     */
    hr = IStorageFolderQueryOperations_GetFoldersAsyncOverloadDefaultStartAndCount( storage_folder_query_operations, CommonFolderQuery_DefaultQuery, &storage_folder_vector_view_operation );
    CHECK_HR ( hr );

    asyncRes = await_IAsyncOperation_IVectorView_StorageFolder( storage_folder_vector_view_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IVectorView_StorageFolder_GetResults( storage_folder_vector_view_operation, &storage_folder_vector_view );
    CHECK_HR( hr );

    hr = IVectorView_StorageFolder_GetAt( storage_folder_vector_view, 0, &sub_storage_folder );
    CHECK_HR( hr );

    hr = IStorageFolder_QueryInterface( sub_storage_folder, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    test_StorageItem( storage_item );

    *file = storage_file;
    *item = storage_item;
        
    CHECK_HR( WindowsDeleteString( pathString ) );
    CHECK_HR( WindowsDeleteString( tmpString ) );
    CHECK_HR( WindowsDeleteString( itemName ) );

    ref = IStorageFolderStatics_Release( storage_folder_statics );
    ok( ref == 2, "got ref %ld.\n", ref );
}

/**
 * ABI::Windows::Storage::StorageFile
 */

void test_StorageFile( const wchar_t* path )
{
    static const WCHAR *storage_file_statics_name = L"Windows.Storage.StorageFile";

    IStorageFileStatics *storage_file_statics = NULL;

    IStorageFile *storage_file = NULL;
    IAsyncOperation_StorageFile *storage_file_operation = NULL;

    HSTRING pathString;
    HSTRING tmpString;
    HSTRING fileType;
    HSTRING contentType;

    HRESULT hr;
    LPWSTR pathStr;
    DWORD asyncRes;
    INT32 comparisonResult;

    LONG ref;

    ACTIVATE_INSTANCE( storage_file_statics_name, storage_file_statics, IID_IStorageFileStatics );

    pathStr = (LPWSTR)malloc( wcslen( path ) * sizeof( WCHAR ) );
    wcscpy( pathStr, path );

    /**
     * ABI::Windows::Storage::IStorageFileStatics::GetFileFromPathAsync
     */
    PathAppendW( pathStr, L"TestFile.tmp" );
    WindowsCreateString( pathStr, wcslen( pathStr ), &pathString );
    hr = IStorageFileStatics_GetFileFromPathAsync( storage_file_statics, pathString, &storage_file_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFile( storage_file_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFile_GetResults( storage_file_operation, &storage_file );
    CHECK_HR( hr );

    check_interface( storage_file, &IID_IUnknown );
    check_interface( storage_file, &IID_IInspectable );
    check_interface( storage_file, &IID_IAgileObject );

    /** --- (@windows.storage.idl) ---
     * StorageFile runtime class expects: 
     * UniversalAPIContract 1.0: 
     *      IStorageFile 
     *      IInputStreamReference 
     *      IRandomAccessStreamReference 
     *      IStorageItem
     *      IStorageItemProperties
     *      IStorageItemProperties2
     *      IStorageItem2
     *      IStorageItemPropertiesWithProvider
     *      IStorageFilePropertiesWithAvailability
     *      IStorageFile2
     */

    check_interface( storage_file, &IID_IStorageFile );
    stubbed_interface( storage_file, &IID_IRandomAccessStreamReference );
    //notimpl: check_interface( storage_file, &IID_IInputStreamReference );
    check_interface( storage_file, &IID_IStorageItem );
    check_interface( storage_file, &IID_IStorageItemProperties );
    //notimpl: check_interface( storage_folder, &IID_IStorageItemProperties2 );
    check_interface( storage_file, &IID_IStorageItem2 );
    check_interface( storage_file, &IID_IStorageItemPropertiesWithProvider );    
    check_interface( storage_file, &IID_IStorageFilePropertiesWithAvailability );
    //notimpl:  check_interface( storage_file, &IID_IStorageFile2 );

    /**
     * ABI::Windows::Storage::IStorageFile
     */
    hr = IStorageFile_QueryInterface( storage_file, &IID_IStorageFile, (void **)&storage_file );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageFile::FileType
     */
    WindowsCreateString( L".tmp", wcslen( L".tmp" ), &tmpString );
    IStorageFile_get_FileType( storage_file, &fileType );
    WindowsCompareStringOrdinal( tmpString, fileType, &comparisonResult );
    ok( !comparisonResult, "unexpected filetype. expected .tmp\n" );

    /**
     * ABI::Windows::Storage::IStorageFile::ContentType
     */
    hr = IStorageFile_get_ContentType( storage_file, &contentType );
    CHECK_HR( hr );

    
}

START_TEST(storage)
{
    HRESULT hr;
    const wchar_t* apppath;
    IStorageItem *returnedItem = NULL;
    IStorageFile *returnedFile = NULL;

    hr = RoInitialize( RO_INIT_MULTITHREADED );

    CoInitialize(NULL);

    ok( hr == S_OK, "RoInitialize failed, hr %#lx\n", hr );

    test_AppDataPathsStatics( &apppath );
    test_StorageFolder( apppath, &returnedItem, &returnedFile );
    test_StorageFile( apppath );

    RoUninitialize();
}
