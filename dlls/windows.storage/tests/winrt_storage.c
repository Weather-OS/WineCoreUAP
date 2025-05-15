/*
 * Written by Weather
 *
 * This is an implementation of Microsoft's OneCoreUAP binaries.
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

DEFINE_ASYNC_COMPLETED_HANDLER( async_uint_handler, IAsyncOperationCompletedHandler_UINT32, IAsyncOperation_UINT32 )
DEFINE_ASYNC_COMPLETED_HANDLER( async_boolean_handler, IAsyncOperationCompletedHandler_boolean, IAsyncOperation_boolean )
DEFINE_ASYNC_COMPLETED_HANDLER( async_known_folders_access_status, IAsyncOperationCompletedHandler_KnownFoldersAccessStatus, IAsyncOperation_KnownFoldersAccessStatus )
DEFINE_ASYNC_COMPLETED_HANDLER( async_uint_with_progress_handler, IAsyncOperationWithProgressCompletedHandler_UINT32_UINT32, IAsyncOperationWithProgress_UINT32_UINT32 )
DEFINE_ASYNC_COMPLETED_HANDLER( async_long_with_progress_handler, IAsyncOperationWithProgressCompletedHandler_UINT64_UINT64, IAsyncOperationWithProgress_UINT64_UINT64 )
DEFINE_ASYNC_COMPLETED_HANDLER( async_buffer_with_progress_handler, IAsyncOperationWithProgressCompletedHandler_IBuffer_UINT32, IAsyncOperationWithProgress_IBuffer_UINT32 )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_file_handler, IAsyncOperationCompletedHandler_StorageFile, IAsyncOperation_StorageFile )
DEFINE_ASYNC_COMPLETED_HANDLER( async_random_access_stream_handler, IAsyncOperationCompletedHandler_IRandomAccessStream, IAsyncOperation_IRandomAccessStream )
DEFINE_ASYNC_COMPLETED_HANDLER( async_random_access_stream_with_content_type_handler, IAsyncOperationCompletedHandler_IRandomAccessStreamWithContentType, IAsyncOperation_IRandomAccessStreamWithContentType )
DEFINE_ASYNC_COMPLETED_HANDLER( basic_properties_handler, IAsyncOperationCompletedHandler_BasicProperties, IAsyncOperation_BasicProperties )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_folder_handler, IAsyncOperationCompletedHandler_StorageFolder, IAsyncOperation_StorageFolder )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_folder_vector_view_handler, IAsyncOperationCompletedHandler_IVectorView_StorageFolder, IAsyncOperation_IVectorView_StorageFolder )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_file_vector_view_handler, IAsyncOperationCompletedHandler_IVectorView_StorageFile, IAsyncOperation_IVectorView_StorageFile )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_item_handler, IAsyncOperationCompletedHandler_IStorageItem, IAsyncOperation_IStorageItem )
DEFINE_ASYNC_COMPLETED_HANDLER( async_storage_item_vector_view_handler, IAsyncOperationCompletedHandler_IVectorView_IStorageItem, IAsyncOperation_IVectorView_IStorageItem )
DEFINE_ASYNC_COMPLETED_HANDLER( async_action_handler, IAsyncActionCompletedHandler, IAsyncAction )

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
void test_AppDataPathsStatics( wchar_t** pathStr )
{    
    static const WCHAR *app_data_paths_statics_name = L"Windows.Storage.AppDataPaths";
   
    IAppDataPathsStatics *app_data_paths_statics = NULL;    
    IAppDataPaths *app_data_paths = NULL;

    HRESULT hr;
    HSTRING localAppDataPath;
    UINT32 stringLen;
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
{
    IAppDataPaths_get_LocalAppData( app_data_paths, &localAppDataPath );
    trace( "Windows::Storage::IAppDataPaths::LocalAppData for %p is %s\n", &app_data_paths, debugstr_hstring( localAppDataPath ) );

    WindowsGetStringRawBuffer( localAppDataPath, &stringLen );
    *pathStr = (wchar_t *)malloc( (stringLen + 1) * sizeof( WCHAR ) );
    memcpy( *pathStr, WindowsGetStringRawBuffer( localAppDataPath, NULL ), stringLen * sizeof( WCHAR ) );
    (*pathStr)[stringLen] = L'\0';

    if (app_data_paths) IAppDataPaths_Release( app_data_paths );
    ref = IAppDataPathsStatics_Release( app_data_paths_statics );
    ok( ref == 1, "got ref %ld.\n", ref );
}

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
    IStorageItemPropertiesWithProvider *item_properties_with_provider = NULL;

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

    BOOLEAN isEqual = FALSE;
    HRESULT hr = S_OK;
    UINT64 itemSize;
    INT32 compResult;
    DWORD asyncRes;

    /**
     * ABI::Windows::Storage::IStorageItem
     */
{
    hr = IStorageItem_QueryInterface( item, &IID_IStorageItem, (void **)&item );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItem::Name
     */
    IStorageItem_get_Name( item, &origName );
    trace( "Windows::Storage::IStorageItem::Name for %p is %s\n", &item, debugstr_hstring( origName ) );

    /**
     * ABI::Windows::Storage::IStorageItem::Path
     */
    IStorageItem_get_Path( item, &path );
    trace( "Windows::Storage::IStorageItem::Path for %p is %s\n", &item, debugstr_hstring( path ) );

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

    hr = IAsyncAction_GetResults( action );
    CHECK_LAST_RESTRICTED_ERROR();

    // Known folders are not allowed to be renamed or deleted.
    if ( hr != E_ACCESSDENIED )
    {
        hr = IStorageItem_get_Name( item, &secondName );
        CHECK_HR( hr );
        WindowsCompareStringOrdinal( secondName, origName, &compResult );
        ok( compResult, "the string rename is the same as origName! compResult %d\n", compResult );

        hr = IStorageItem_RenameAsync( item, origName, NameCollisionOption_ReplaceExisting, &action );
        CHECK_HR( hr );

        asyncRes = await_IAsyncAction( action, INFINITE );
        ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );
    }

    /**
     * ABI::Windows::Storage::IStorageItem::GetBasicPropertiesAsync
     */
    hr = IStorageItem_GetBasicPropertiesAsync( item, &properties_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_BasicProperties( properties_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_BasicProperties_GetResults( properties_operation, &properties );
    CHECK_HR( hr );
}

    /**
     * ABI::Windows:Storage:IBasicProperties
     */
{
    hr = IBasicProperties_QueryInterface( properties, &IID_IBasicProperties, (void **)&properties );
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
}

    /**
     * ABI::Windows::Storage::IStorageItem2
     */
{
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
}

    /**
     * ABI::Windows::Storage::IStorageItemProperties
     */
{
    hr = IStorageItem_QueryInterface( item, &IID_IStorageItemProperties, (void **)&item_properties );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItemProperties::DisplayName
     */
    IStorageItemProperties_get_DisplayName( item_properties, &origName );
    trace( "Windows::Storage::IStorageItemProperties::DisplayName for %p is %s\n", &item_properties, debugstr_hstring( origName ) );

    /**
     * ABI::Windows::Storage::IStorageItemProperties::DisplayType
     */
    IStorageItemProperties_get_DisplayType( item_properties, &displaytype );
    trace( "Windows::Storage::IStorageItemProperties::DisplayType for %p is %s\n", &item_properties, debugstr_hstring( displaytype ) );

    /**
     * ABI::Windows::Storage::IStorageItemProperties::FolderRelativeId
     */
    IStorageItemProperties_get_FolderRelativeId( item_properties, &folderRelativeId );
    trace( "Windows::Storage::IStorageItemProperties::FolderRelativeId for %p is %s\n", &item_properties, debugstr_hstring( folderRelativeId ) );
}

    /**
     * ABI::Windows::Storage::IStorageItemPropertiesWithProvider
     */
{
    hr = IStorageItem_QueryInterface( item, &IID_IStorageItemPropertiesWithProvider, (void **)&item_properties_with_provider );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::IStorageItemPropertiesWithProvider::Provider
     */
    hr = IStorageItemPropertiesWithProvider_get_Provider( item_properties_with_provider, &provider );
    CHECK_HR( hr );
}

    /**
     * ABI::Windows::Storage::IStorageProvider
     */
{
    /**
     * ABI::Windows::Storage::IStorageProvider::Id
     */
    IStorageProvider_get_Id( provider, &itemId );
    trace( "Windows::Storage::IStorageProvider::Id for %p is %s\n", &provider, debugstr_hstring( itemId ) );

    /**
     * ABI::Windows::Storage::IStorageProvider::DisplayName
     */
    IStorageProvider_get_DisplayName( provider, &origName );
    trace( "Windows::Storage::IStorageProvider::DisplayName for %p is %s\n", &provider, debugstr_hstring( origName ) );
}

}

/**
 * ABI::Windows::Storage::DownloadsFolder
 */
void test_DownloadsFolder( void )
{
    static const WCHAR *downloads_folder_statics_name = L"Windows.Storage.DownloadsFolder";

    IDownloadsFolderStatics *downloads_folder_statics = NULL;

    IStorageItem *storage_item = NULL;

    IStorageFile *storage_file = NULL;
    IAsyncOperation_StorageFile *storage_file_operation = NULL;

    IAsyncAction *action = NULL;

    HRESULT hr;
    HSTRING tmpString;

    DWORD asyncRes;

    ACTIVATE_INSTANCE( downloads_folder_statics_name, downloads_folder_statics, IID_IDownloadsFolderStatics );

    /**
     * ABI::Windows::Storage::IDownloadsFolderStatics::CreateFileAsync
     */
    WindowsCreateString( L"TempDownloadFile.tmp", wcslen( L"TempDownloadFile.tmp" ), &tmpString );
    hr = IDownloadsFolderStatics_CreateFileWithCollisionOptionAsync( downloads_folder_statics, tmpString, CreationCollisionOption_FailIfExists, &storage_file_operation );
    CHECK_HR( hr )

    asyncRes = await_IAsyncOperation_StorageFile( storage_file_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFile_GetResults( storage_file_operation, &storage_file );
    CHECK_HR( hr );

    hr = IStorageFile_QueryInterface( storage_file, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    test_StorageItem( storage_item );

    hr = IStorageItem_DeleteAsync( storage_item, StorageDeleteOption_Default, &action );
    CHECK_HR( hr );

    asyncRes = await_IAsyncAction( action, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );
}

/**
 * ABI::Windows::Storage::KnownFolders
 */
void test_KnownFolders( void )
{
    static const WCHAR *known_folders_statics_name = L"Windows.Storage.KnownFolders";

    IKnownFoldersStatics *known_folders_statics = NULL;
    IKnownFoldersStatics4 *known_folders_statics4 = NULL;    
    KnownFoldersAccessStatus access_status;
    IAsyncOperation_KnownFoldersAccessStatus *access_status_operation = NULL;

    IStorageItem *storage_item = NULL;

    IStorageFolder *storage_folder = NULL;

    HRESULT hr;

    DWORD asyncRes;

    ACTIVATE_INSTANCE( known_folders_statics_name, known_folders_statics, IID_IKnownFoldersStatics );
    ACTIVATE_INSTANCE( known_folders_statics_name, known_folders_statics4, IID_IKnownFoldersStatics4 );

    /**
     * ABI::Windows::Storage::IKnownFoldersStatics::DocumentsLibrary
     */
    hr = IKnownFoldersStatics_get_DocumentsLibrary( known_folders_statics, &storage_folder );
    CHECK_HR( hr );

    hr = IStorageFolder_QueryInterface( storage_folder, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );
    test_StorageItem( storage_item );

    /**
     * ABI::Windows::Storage::IKnownFoldersStatics4::RequestAccessAsync
     */
    hr = IKnownFoldersStatics4_RequestAccessAsync( known_folders_statics4, KnownFolderId_DownloadsFolder, &access_status_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_KnownFoldersAccessStatus( access_status_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_KnownFoldersAccessStatus_GetResults( access_status_operation, &access_status );
    CHECK_HR( hr );

    trace( "Returned Windows::Storage::IKnownFoldersStatics4::RequestAccessAsync is %d\n", access_status );
}

/**
 * ABI::Windows::Storage::Streams::RandomAccessStream
 */
void test_Streams_RandomAccessStream( IRandomAccessStream *stream )
{
    //This tests various ways that a random access stream could write to and read from a file.

    IBuffer *buffer = NULL;
    IBuffer *read_buffer = NULL;
    IBufferFactory *buffer_factory = NULL;

    IDataWriter *data_writer = NULL;
    IDataReader *data_reader = NULL;
    IDataWriterFactory *data_writer_factory = NULL;
    IDataReaderFactory *data_reader_factory = NULL;
    IDataReaderStatics *data_reader_statics = NULL;

    IClosable *closable = NULL;
    IOutputStream *output_stream = NULL;
    IInputStream *input_stream = NULL;
    IRandomAccessStream *cloned_stream = NULL;
    IRandomAccessStreamStatics *stream_statics = NULL;

    IAsyncOperation_UINT32 *unit_operation = NULL;
    IAsyncOperation_boolean *boolean_operation = NULL;
    IAsyncOperationWithProgress_UINT32_UINT32 *uint_operation_with_progress = NULL;
    IAsyncOperationWithProgress_UINT64_UINT64 *long_operation_with_progress = NULL;
    IAsyncOperationWithProgress_IBuffer_UINT32 *buffer_operation_with_progress = NULL;

    HSTRING tmpString;
    HSTRING checkString;

    INT32 comparisonResult;
    DWORD asyncRes;
    UINT64 position;
    UINT64 size;
    UINT32 code_unit_count;    
    HRESULT hr = S_OK;
    BOOLEAN canRead = FALSE;
    BOOLEAN canWrite = FALSE;

    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataWriter, data_writer_factory, IID_IDataWriterFactory );
    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataReader, data_reader_factory, IID_IDataReaderFactory );
    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_DataReader, data_reader_statics, IID_IDataReaderStatics );
    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_Buffer, buffer_factory, IID_IBufferFactory );
    ACTIVATE_INSTANCE( RuntimeClass_Windows_Storage_Streams_RandomAccessStream, stream_statics, IID_IRandomAccessStreamStatics );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream
     */
{
    hr = IRandomAccessStream_QueryInterface( stream, &IID_IRandomAccessStream, (void **)&stream );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream::CanRead
     */
    IRandomAccessStream_get_CanRead( stream, &canRead );
    if ( !canRead )
    {
        trace( "stream %p is unreadable. skipping tests...\n", &stream );
        return;
    }

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream::CanWrite
     */
    IRandomAccessStream_get_CanWrite( stream, &canWrite );
    if ( !canWrite )
        trace( "stream %p is unwritable. write tests will be skipped.\n", &stream );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream::Position
     */
    IRandomAccessStream_get_Position( stream, &position );
    trace( "Windows::Storage::Streams::IRandomAccessStream::Position for %p is %lld\n", &stream, position );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream::Size
     */
    IRandomAccessStream_get_Size( stream, &size );
    trace( "Windows::Storage::Streams::IRandomAccessStream::Size for %p is %lld\n", &stream, size );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream::CloneStream
     */
    hr = IRandomAccessStream_CloneStream( stream, &cloned_stream );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream::GetInputStreamAt
     */
    hr = IRandomAccessStream_GetInputStreamAt( stream, position, &input_stream );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStream::GetOutputStreamAt
     */
    if ( canWrite )
    {
        hr = IRandomAccessStream_GetOutputStreamAt( stream, position, &output_stream );
        CHECK_HR( hr );
    }
}

    /**
     * ABI::Windows::Storage::Streams::IOutputStream
     */
{
    hr = IRandomAccessStream_QueryInterface( stream, &IID_IOutputStream, (void **)&output_stream );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::Streams::IOutputStream::WriteAsync
     */
    if ( canWrite )
    {    
        // First pass using IDataWriter
        hr = IDataWriterFactory_CreateDataWriter( data_writer_factory, output_stream, &data_writer );
        CHECK_HR( hr );

        WindowsCreateString( L"Hello, World!\n", wcslen(L"Hello, World!\n"), &tmpString );
        hr = IDataWriter_WriteString( data_writer, tmpString, &code_unit_count );
        CHECK_HR( hr );

        hr = IDataWriter_StoreAsync( data_writer, &unit_operation );
        CHECK_HR( hr );

        asyncRes = await_IAsyncOperation_UINT32( unit_operation, INFINITE );
        ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

        hr = IDataWriter_FlushAsync( data_writer, &boolean_operation );
        CHECK_HR( hr );

        asyncRes = await_IAsyncOperation_boolean( boolean_operation, INFINITE );
        ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

        // Second pass using IBuffer
        WindowsCreateString( L"Goodbye, World!\n", wcslen(L"Goodbye, World!\n"), &tmpString );
        hr = IDataWriter_WriteString( data_writer, tmpString, &code_unit_count );
        CHECK_HR( hr );

        hr = IDataWriter_DetachBuffer( data_writer, &buffer );
        CHECK_HR( hr );

        hr = IOutputStream_WriteAsync( output_stream, buffer, &uint_operation_with_progress );
        CHECK_HR( hr );

        asyncRes = await_IAsyncOperationWithProgress_UINT32_UINT32( uint_operation_with_progress, INFINITE );
        ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );
    }

    /**
     * ABI::Windows::Storage::Streams::IOutputStream::FlushAsync
     */
    if ( canWrite )
    {    
        hr = IOutputStream_FlushAsync( output_stream, &boolean_operation );
        CHECK_HR( hr );

        asyncRes = await_IAsyncOperation_boolean( boolean_operation, INFINITE );
        ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );
    }
}

    /**
     * ABI::Windows::Storage::Streams::IInputStream
     */
{
    hr = IRandomAccessStream_GetInputStreamAt( stream, 0, &input_stream );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::Streams::IInputStream::ReadAsync
     */
    //First pass using IDataReader
    hr = IDataReaderFactory_CreateDataReader( data_reader_factory, input_stream, &data_reader );
    CHECK_HR( hr );

    IRandomAccessStream_get_Size( stream, &size );
    
    hr = IDataReader_LoadAsync( data_reader, size, &unit_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_UINT32( unit_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IDataReader_ReadString( data_reader, size, &tmpString );
    CHECK_HR( hr );

    WindowsCreateString( L"Hello, World!\nGoodbye, World!\n", wcslen( L"Hello, World!\nGoodbye, World!\n" ), &checkString );
    WindowsCompareStringOrdinal( tmpString, checkString, &comparisonResult );
    ok ( !comparisonResult, "the 2 compared strings do not match!\n" ); 

    //Second pass using IBuffer
    hr = IBufferFactory_Create( buffer_factory, size, &read_buffer );
    CHECK_HR( hr );

    hr = IRandomAccessStream_GetInputStreamAt( stream, 0, &input_stream );
    CHECK_HR( hr );

    hr = IInputStream_ReadAsync( input_stream, read_buffer, size, InputStreamOptions_None, &buffer_operation_with_progress );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperationWithProgress_IBuffer_UINT32( buffer_operation_with_progress, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperationWithProgress_IBuffer_UINT32_GetResults( buffer_operation_with_progress, &buffer );
    CHECK_HR( hr );

    hr = IDataReaderStatics_FromBuffer( data_reader_statics, buffer, &data_reader );
    CHECK_HR( hr );

    hr = IDataReader_ReadString( data_reader, size, &tmpString );
    CHECK_HR( hr );

    WindowsCreateString( L"Hello, World!\nGoodbye, World!\n", wcslen( L"Hello, World!\nGoodbye, World!\n" ), &checkString );
    WindowsCompareStringOrdinal( tmpString, checkString, &comparisonResult );
    ok ( !comparisonResult, "the 2 compared strings do not match!\n" ); 
}
    
    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStreamStatics::CopyAsync
     */

    //A fun test to see if the file can write to itself.
    if ( canWrite )
    {
        hr = IRandomAccessStream_GetInputStreamAt( stream, 0, &input_stream );
        CHECK_HR( hr );

        hr = IRandomAccessStream_GetOutputStreamAt( stream, 0, &output_stream );
        CHECK_HR( hr );

        hr = IRandomAccessStreamStatics_CopyAsync( stream_statics, input_stream, output_stream, &long_operation_with_progress );
        CHECK_HR( hr );

        asyncRes = await_IAsyncOperationWithProgress_UINT64_UINT64( long_operation_with_progress, INFINITE );
        ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

        hr = IRandomAccessStream_GetInputStreamAt( stream, 0, &input_stream );
        CHECK_HR( hr );

        hr = IDataReaderFactory_CreateDataReader( data_reader_factory, input_stream, &data_reader );
        CHECK_HR( hr );
    
        IRandomAccessStream_get_Size( stream, &size );
        
        hr = IDataReader_LoadAsync( data_reader, size, &unit_operation );
        CHECK_HR( hr );
    
        asyncRes = await_IAsyncOperation_UINT32( unit_operation, INFINITE );
        ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );
    
        hr = IDataReader_ReadString( data_reader, size, &tmpString );
        CHECK_HR( hr );
    
        WindowsCreateString( L"Hello, World!\nGoodbye, World!\n", wcslen( L"Hello, World!\nGoodbye, World!\n" ), &checkString );
        WindowsCompareStringOrdinal( tmpString, checkString, &comparisonResult );
        ok ( !comparisonResult, "the 2 compared strings do not match!\nFirst string was %s, second string was %s\n", debugstr_hstring( tmpString ), debugstr_hstring( checkString ) ); 
    }

    /**
     * ABI::Windows::Foundation::IClosable
     */
{
    hr = IRandomAccessStream_QueryInterface( stream, &IID_IClosable, (void **)&closable );
    CHECK_HR( hr );

    hr = IClosable_Close( closable );
    CHECK_HR( hr );
}

}

/**
 * ABI::Windows::Storage::StorageFolder
 */
void test_StorageFolder( wchar_t* path, IStorageItem **item, IStorageFile **file, IStorageFolder **testFolder )
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

    UINT32 size;
    INT32 compResult;

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
{
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

    WindowsCreateString( L"TestFile2.tmp", wcslen( L"TestFile2.tmp" ), &tmpString );
    hr = IStorageFolder_CreateFileAsync( storage_folder, tmpString, CreationCollisionOption_ReplaceExisting, &storage_file_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFile( storage_file_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );
    
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
    ok( size == 2u, "unexpected size of %u\n", size );

    hr = IVectorView_StorageFile_GetAt( storage_file_vector_view, 0, &storage_file );
    CHECK_HR( hr );

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

    hr = IVectorView_IStorageItem_GetAt( storage_item_vector_view, 1, &storage_item );
    CHECK_HR( hr );

    check_interface( storage_item, &IID_IStorageFile );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d, tmpString %s, itemName %s.\n", compResult, debugstr_hstring( tmpString ), debugstr_hstring( itemName ) );

    hr = IVectorView_IStorageItem_GetAt( storage_item_vector_view, 0, &storage_item );
    CHECK_HR( hr );

    check_interface( storage_item, &IID_IStorageFolder );

    WindowsCreateString( L"TestFolder", wcslen( L"TestFolder" ), &tmpString );

    hr = IStorageItem_get_Name( storage_item, &itemName );
    CHECK_HR( hr );
    WindowsCompareStringOrdinal( tmpString, itemName, &compResult );
    ok( !compResult, "the string tmpString is not the same as itemName! compResult %d\n", compResult );
}

    /**
     * ABI::Windows::Storage::IStorageFolder2
     */
{
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
}

    /**
     * ABI::Windows::Storage::IStorageFolderQueryOperations
     */
{
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
    ok( size == 2u, "unexpected size of %u\n", size );

    hr = IVectorView_StorageFile_GetAt( storage_file_vector_view, 0, &storage_file );
    CHECK_HR( hr );

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
}

    test_StorageItem( storage_item );

    *file = storage_file;
    *item = storage_item;
    *testFolder = sub_storage_folder;
        
    CHECK_HR( WindowsDeleteString( pathString ) );
    CHECK_HR( WindowsDeleteString( tmpString ) );
    CHECK_HR( WindowsDeleteString( itemName ) );

    IStorageFolderStatics_Release( storage_folder_statics );
}

/**
 * ABI::Windows::Storage::StorageFile
 */
void test_StorageFile( wchar_t* path, IStorageFolder *folder )
{
    static const WCHAR *storage_file_statics_name = L"Windows.Storage.StorageFile";

    IStorageFileStatics *storage_file_statics = NULL;

    IStorageItem *storage_item = NULL;

    IStorageFile *storage_file = NULL;
    IStorageFile *dummy_file = NULL;
    IStorageFile *copied_file = NULL;

    IContentTypeProvider *content_type_provider = NULL;

    IAsyncOperation_StorageFile *storage_file_operation = NULL;
    IAsyncOperation_IRandomAccessStream *random_access_stream_operation = NULL;
    IAsyncOperation_IRandomAccessStreamWithContentType *random_access_stream_with_content_type_operation = NULL;

    IRandomAccessStream *random_access_stream = NULL;
    IRandomAccessStreamReference *random_access_stream_reference = NULL;
    IRandomAccessStreamWithContentType *random_access_stream_with_content_type = NULL;

    IAsyncAction *action = NULL;

    HSTRING pathString;
    HSTRING tmpString;
    HSTRING fileType;
    HSTRING contentType;

    HRESULT hr;
    LPWSTR pathStr;
    DWORD asyncRes;
    INT32 comparisonResult;

    ACTIVATE_INSTANCE( storage_file_statics_name, storage_file_statics, IID_IStorageFileStatics );

    pathStr = (LPWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );
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
    check_interface( storage_file, &IID_IRandomAccessStreamReference );
    //notimpl: check_interface( storage_file, &IID_IInputStreamReference );
    check_interface( storage_file, &IID_IStorageItem );
    check_interface( storage_file, &IID_IStorageItemProperties );
    //notimpl: check_interface( storage_folder, &IID_IStorageItemProperties2 );
    check_interface( storage_file, &IID_IStorageItem2 );
    check_interface( storage_file, &IID_IStorageItemPropertiesWithProvider );    
    check_interface( storage_file, &IID_IStorageFilePropertiesWithAvailability );
    //notimpl:  check_interface( storage_file, &IID_IStorageFile2 );

    //Dummy file for copy and move operations
    free( pathStr );
    pathStr = (LPWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );
    wcscpy( pathStr, path );

    PathAppendW( pathStr, L"TestFile2.tmp" );
    WindowsCreateString( pathStr, wcslen( pathStr ), &pathString );
    hr = IStorageFileStatics_GetFileFromPathAsync( storage_file_statics, pathString, &storage_file_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFile( storage_file_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFile_GetResults( storage_file_operation, &dummy_file );
    CHECK_HR( hr );


    /**
     * ABI::Windows::Storage::IStorageFile
     */
{
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
    IStorageFile_get_ContentType( storage_file, &contentType );
    trace( "Windows::Storage::IStorageFile::ContentType for %p is %s\n", &storage_file, debugstr_hstring( contentType ) );

    /**
     * ABI::Windows::Storage::IStorageFile::CopyAndReplaceAsync
     */
    hr = IStorageFile_CopyAndReplaceAsync( storage_file, dummy_file, &action );
    CHECK_HR( hr );

    asyncRes = await_IAsyncAction( action, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    /**
     * ABI::Windows::Storage::IStorageFile::CopyAsync
     */
    hr = IStorageFile_CopyOverloadDefaultNameAndOptions( storage_file, folder, &storage_file_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_StorageFile( storage_file_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_StorageFile_GetResults( storage_file_operation, &copied_file );
    CHECK_HR( hr );
    
    free( pathStr );
    pathStr = (LPWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );
    wcscpy( pathStr, path );
    PathAppendW( pathStr, L"TestFolder" );
    PathAppendW( pathStr, L"TestFile.tmp" );
    WindowsCreateString( pathStr, wcslen( pathStr ), &pathString );
    
    hr = IStorageFile_QueryInterface( copied_file, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    IStorageItem_get_Path( storage_item, &tmpString );

    WindowsCompareStringOrdinal( tmpString, pathString, &comparisonResult );
    ok( !comparisonResult, "unexpected path. expected %s to be %s\n", debugstr_hstring( tmpString ), debugstr_hstring( pathString ) );

    /**
     * ABI::Windows::Storage::IStorageFile::MoveAndReplaceAsync
     */
    hr = IStorageFile_MoveAndReplaceAsync( dummy_file, storage_file, &action );
    CHECK_HR( hr );

    asyncRes = await_IAsyncAction( action, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    free( pathStr );
    pathStr = (LPWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );
    wcscpy( pathStr, path );
    PathAppendW( pathStr, L"TestFile.tmp" );
    WindowsCreateString( pathStr, wcslen( pathStr ), &pathString );

    hr = IStorageFile_QueryInterface( dummy_file, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    IStorageItem_get_Path( storage_item, &tmpString );

    WindowsCompareStringOrdinal( tmpString, pathString, &comparisonResult );
    ok( !comparisonResult, "unexpected path. expected %s to be %s\n", debugstr_hstring( tmpString ), debugstr_hstring( pathString ) );

    /**
     * ABI::Windows::Storage::IStorageFile::MoveAsync
     */
    WindowsCreateString( L"TestMove.tmp", wcslen( L"TestMove.tmp" ), &tmpString );
    hr = IStorageFile_MoveOverloadDefaultOptions( storage_file, folder, tmpString, &action );
    CHECK_HR( hr );

    asyncRes = await_IAsyncAction( action, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    free( pathStr );
    pathStr = (LPWSTR)malloc( MAX_PATH * sizeof( WCHAR ) );
    wcscpy( pathStr, path );
    PathAppendW( pathStr, L"TestFolder" );
    PathAppendW( pathStr, L"TestMove.tmp" );
    WindowsCreateString( pathStr, wcslen( pathStr ), &pathString );

    hr = IStorageFile_QueryInterface( storage_file, &IID_IStorageItem, (void **)&storage_item );
    CHECK_HR( hr );

    IStorageItem_get_Path( storage_item, &tmpString );

    WindowsCompareStringOrdinal( tmpString, pathString, &comparisonResult );
    ok( !comparisonResult, "unexpected path. expected %s to be %s\n", debugstr_hstring( tmpString ), debugstr_hstring( pathString ) );

    /**
     * ABI::Windows::Storage::IStorageFile::OpenAsync
     */
    // OpenAsync opens with write access.
    hr = IStorageFile_OpenAsync( storage_file, FileAccessMode_ReadWrite, &random_access_stream_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_IRandomAccessStream( random_access_stream_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IRandomAccessStream_GetResults( random_access_stream_operation, &random_access_stream );
    CHECK_HR( hr );

    test_Streams_RandomAccessStream( random_access_stream );
}

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStreamReference
     */
{
    hr = IStorageFile_QueryInterface( storage_file, &IID_IRandomAccessStreamReference, (void **)&random_access_stream_reference );
    CHECK_HR( hr );

    /**
     * ABI::Windows::Storage::Streams::IRandomAccessStreamReference::OpenReadAsync
     */
    hr = IRandomAccessStreamReference_OpenReadAsync( random_access_stream_reference, &random_access_stream_with_content_type_operation );
    CHECK_HR( hr );

    asyncRes = await_IAsyncOperation_IRandomAccessStreamWithContentType( random_access_stream_with_content_type_operation, INFINITE );
    ok( !asyncRes, "got asyncRes %#lx\n", asyncRes );

    hr = IAsyncOperation_IRandomAccessStreamWithContentType_GetResults( random_access_stream_with_content_type_operation, &random_access_stream_with_content_type );
    CHECK_HR( hr );

    hr = IRandomAccessStreamWithContentType_QueryInterface( random_access_stream_with_content_type, &IID_IRandomAccessStream, (void **)&random_access_stream );
    CHECK_HR( hr );

    test_Streams_RandomAccessStream( random_access_stream );
}

    /**
     * ABI::Windows::Storage::Streams:IContentTypeProvider
     */
{
    hr = IRandomAccessStreamWithContentType_QueryInterface( random_access_stream_with_content_type, &IID_IContentTypeProvider, (void **)&content_type_provider );
    CHECK_HR( hr );


    /**
     * ABI::Windows::Storage::Streams:IContentTypeProvider::ContentType
     */
    IContentTypeProvider_get_ContentType( content_type_provider, &tmpString );
    trace( "Windows::Storage::Streams:IContentTypeProvider::ContentType for %p is %s\n", &content_type_provider, debugstr_hstring( tmpString ) );
}

}

START_TEST(winrt_storage)
{
    HRESULT hr;
    wchar_t *apppath;
    wchar_t execPath[MAX_PATH];
    IStorageItem *returnedItem = NULL;
    IStorageFile *returnedFile = NULL;
    IStorageFolder *returnedFolder = NULL;

    hr = RoInitialize( RO_INIT_MULTITHREADED );

    CoInitialize(NULL);

    ok( hr == S_OK, "RoInitialize failed, hr %#lx\n", hr );

    GetModuleFileNameW( NULL, execPath, MAX_PATH );
    PathRemoveFileSpecW(execPath);

    PathAppendW(execPath, L"AppxManifest.xml");

    if ( !PathFileExistsW( execPath ) )
    {
        // For now, On Wine, we rely on AppxManifest.xml to provide us with Package details.
        // On Windows, this is handled by an Internal package management tool.
        hr = SHGetKnownFolderPath( &FOLDERID_Documents, 0, NULL, &apppath );
        CHECK_HR( hr );
        PathAppendW( apppath, L"storage_tests" );
        CreateDirectoryW( apppath, NULL );
    }
    else
    {
        test_AppDataPathsStatics( &apppath );
        test_KnownFolders();
        test_DownloadsFolder();
    }
    
    test_StorageFolder( apppath, &returnedItem, &returnedFile, &returnedFolder );
    test_StorageFile( apppath, returnedFolder );

    RoUninitialize();
}
