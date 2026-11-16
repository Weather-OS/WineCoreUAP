/*
 * Copyright 2018 Nikolay Sivov for CodeWeavers
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

#include "winerror.h"
#define COBJMACROS

#include <stdarg.h>
#include <stdint.h>
#include <zlib.h>

#include "windef.h"
#include "winternl.h"
#include "msopc.h"

#include "opc_private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(msopc);

#pragma pack(push,2)
struct local_file_header
{
    uint32_t signature;
    uint16_t version;
    uint16_t flags;
    uint16_t method;
    uint32_t mtime;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t name_length;
    uint16_t extra_length;
};

struct zip32_data_descriptor
{
    uint32_t signature;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
};

struct zip32_central_directory_header
{
    uint32_t signature;
    uint16_t version;
    uint16_t min_version;
    uint16_t flags;
    uint16_t method;
    uint32_t mtime;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t name_length;
    uint16_t extra_length;
    uint16_t comment_length;
    uint16_t diskid;
    uint16_t internal_attributes;
    uint32_t external_attributes;
    uint32_t local_file_offset;
};

struct zip32_end_of_central_directory
{
    uint32_t signature;
    uint16_t diskid;
    uint16_t firstdisk;
    uint16_t records_num;
    uint16_t records_total;
    uint32_t directory_size;
    uint32_t directory_offset;
    uint16_t comment_length;
};

struct zip64_data_descriptor
{
    uint32_t signature;
    uint32_t crc32;
    uint64_t compressed_size;
    uint64_t uncompressed_size;
};

struct zip64_extra_field
{
    uint16_t id;
    uint16_t size;
    uint64_t uncompressed_size;
    uint64_t compressed_size;
    uint64_t offset;
    uint32_t diskid;
};

struct zip64_end_of_central_directory
{
    uint32_t signature;
    uint64_t size;
    uint16_t version;
    uint16_t min_version;
    uint32_t diskid;
    uint32_t directory_diskid;
    uint64_t records_num;
    uint64_t records_total;
    uint64_t directory_size;
    uint64_t directory_offset;
};

struct zip64_end_of_central_directory_locator
{
    uint32_t signature;
    uint32_t eocd64_disk;
    uint64_t eocd64_offset;
    uint32_t disk_num;
};
#pragma pack(pop)

enum zip_signatures
{
    ZIP32_CDFH = 0x02014b50,
    LOCAL_HEADER_SIGNATURE  = 0x04034b50,
    ZIP32_EOCD = 0x06054b50,
    ZIP64_EOCD64 = 0x06064b50,
    ZIP64_EOCD64_LOCATOR = 0x07064b50,
    DATA_DESCRIPTOR_SIGNATURE = 0x08074b50,
};

enum zip_versions
{
    ZIP32_VERSION = 20,
    ZIP64_VERSION = 45,
};

enum entry_flags
{
    DEFLATE_NORMAL = 0x0,
    DEFLATE_MAX = 0x2,
    DEFLATE_FAST = 0x4,
    DEFLATE_SUPERFAST = 0x6,
    USE_DATA_DESCRIPTOR = 0x8,
};

struct zip_file
{
    uint64_t compressed_size;
    uint64_t uncompressed_size;
    uint64_t offset;
    uint32_t crc32;
    uint16_t name_length;
    uint16_t method;
    uint16_t flags;
    char name[1];
};

struct zip_archive
{
    struct zip_file **files;
    size_t file_count;
    size_t file_size;

    DWORD mtime;
    IStream *stream;
    uint64_t position;
    HRESULT status;
    LONG ref; //<-- part_set and package are going to be holding zip_archives. not used for write.

    bool zip64;
    bool reading;

    unsigned char input_buffer[0x8000];
    unsigned char output_buffer[0x8000];
};

static uint32_t read_u32_le(const BYTE *p)
{
    return ((uint32_t)p[0])       |
           ((uint32_t)p[1] << 8)  |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static uint16_t read_u16_le(const BYTE *p)
{
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint64_t read_u64_le(const BYTE *p)
{
    return ((uint64_t)read_u32_le(p)) |
           ((uint64_t)read_u32_le(p + 4) << 32);
}

HRESULT compress_create_archive(IStream *stream, bool zip64, struct zip_archive **out)
{
    struct zip_archive *archive;
    WORD date, time;
    FILETIME ft;

    if (!(archive = malloc(sizeof(*archive))))
        return E_OUTOFMEMORY;

    archive->files = NULL;
    archive->file_size = 0;
    archive->file_count = 0;
    archive->status = S_OK;

    archive->stream = stream;
    IStream_AddRef(archive->stream);
    archive->position = 0;

    GetSystemTimeAsFileTime(&ft);
    FileTimeToDosDateTime(&ft, &date, &time);
    archive->mtime = date << 16 | time;

    archive->zip64 = zip64;

    *out = archive;

    return S_OK;
}

static void compress_write(struct zip_archive *archive, const void *data, ULONG size)
{
    ULONG written;

    if (FAILED(archive->status))
        return;

    archive->status = IStream_Write(archive->stream, data, size, &written);
    if (written != size)
        archive->status = E_FAIL;
    else
        archive->position += written;

    if (FAILED(archive->status))
        WARN("Failed to write output %p, size %lu, written %lu, hr %#lx.\n", data, size, written, archive->status);
}

HRESULT compress_finalize_archive(struct zip_archive *archive)
{
    struct zip32_end_of_central_directory dir_end;
    struct zip32_central_directory_header cdh;
    uint64_t cd_offset, cd_size = 0;
    size_t i;

    /* Directory entries */
    cd_offset = archive->position;

    if (archive->zip64)
    {
        struct zip64_end_of_central_directory_locator locator;
        struct zip64_end_of_central_directory eocd64;
        uint64_t eocd64_offset = archive->position;
        struct zip64_extra_field extra_field;

        for (i = 0; i < archive->file_count; ++i)
        {
            const struct zip_file *file = archive->files[i];

            cdh.signature = ZIP32_CDFH;
            cdh.version = ZIP64_VERSION;
            cdh.min_version = ZIP64_VERSION;
            cdh.flags = USE_DATA_DESCRIPTOR;
            cdh.method = file->method;
            cdh.mtime = archive->mtime;
            cdh.crc32 = file->crc32;
            cdh.compressed_size = ~0u;
            cdh.uncompressed_size = ~0u;
            cdh.name_length = file->name_length;
            cdh.extra_length = sizeof(extra_field);
            cdh.comment_length = 0;
            cdh.diskid = 0;
            cdh.internal_attributes = 0;
            cdh.external_attributes = 0;
            cdh.local_file_offset = ~0u;
            compress_write(archive, &cdh, sizeof(cdh));

            /* File name */
            compress_write(archive, file->name, file->name_length);

            /* Extra field */
            extra_field.id = 1;
            extra_field.size = sizeof(extra_field);
            extra_field.uncompressed_size = file->uncompressed_size;
            extra_field.compressed_size = file->compressed_size;
            extra_field.offset = file->offset;
            extra_field.diskid = 0;
            compress_write(archive, &extra_field, sizeof(extra_field));

            cd_size += cdh.name_length + cdh.extra_length + sizeof(cdh);
        }

        /* ZIP64 end of central directory */
        eocd64.signature = ZIP64_EOCD64;
        eocd64.size = sizeof(eocd64) - 12;
        eocd64.version = ZIP64_VERSION;
        eocd64.min_version = ZIP64_VERSION;
        eocd64.diskid = 0;
        eocd64.directory_diskid = 0;
        eocd64.records_num = archive->file_count;
        eocd64.records_total = archive->file_count;
        eocd64.directory_size = cd_size;
        eocd64.directory_offset = cd_offset;
        compress_write(archive, &eocd64, sizeof(eocd64));

        /* ZIP64 end of central directory locator */
        locator.signature = ZIP64_EOCD64_LOCATOR;
        locator.eocd64_disk = 0;
        locator.eocd64_offset = eocd64_offset;
        locator.disk_num = 0;
        compress_write(archive, &locator, sizeof(locator));

        /* End of central directory */
        memset(&dir_end, 0xff, sizeof(dir_end));
        dir_end.signature = ZIP32_EOCD;
        dir_end.comment_length = 0;
        compress_write(archive, &dir_end, sizeof(dir_end));
    }
    else
    {
        for (i = 0; i < archive->file_count; ++i)
        {
            const struct zip_file *file = archive->files[i];

            cdh.signature = ZIP32_CDFH;
            cdh.version = ZIP32_VERSION;
            cdh.min_version = ZIP32_VERSION;
            cdh.flags = USE_DATA_DESCRIPTOR;
            cdh.method = file->method;
            cdh.mtime = archive->mtime;
            cdh.crc32 = file->crc32;
            cdh.compressed_size = file->compressed_size;
            cdh.uncompressed_size = file->uncompressed_size;
            cdh.name_length = file->name_length;
            cdh.extra_length = 0;
            cdh.comment_length = 0;
            cdh.diskid = 0;
            cdh.internal_attributes = 0;
            cdh.external_attributes = 0;
            cdh.local_file_offset = file->offset;
            compress_write(archive, &cdh, sizeof(cdh));

            /* File name */
            compress_write(archive, file->name, file->name_length);

            cd_size += cdh.name_length + sizeof(cdh);
        }

        /* End of central directory */
        dir_end.signature = ZIP32_EOCD;
        dir_end.diskid = 0;
        dir_end.firstdisk = 0;
        dir_end.records_num = archive->file_count;
        dir_end.records_total = archive->file_count;
        dir_end.directory_size = cd_size;
        dir_end.directory_offset = cd_offset;
        dir_end.comment_length = 0;
        compress_write(archive, &dir_end, sizeof(dir_end));
    }

    return archive->status;
}

void compress_release_archive(struct zip_archive *archive)
{
    size_t i;
    ULONG refcount;

    if ( archive->reading )
    {
        refcount = InterlockedDecrement( &archive->ref );
        if ( refcount )
            return;
    }
    IStream_Release(archive->stream);

    for (i = 0; i < archive->file_count; i++)
        free(archive->files[i]);
    free(archive->files);
    free(archive);
}

void compress_addref_archive(struct zip_archive *archive)
{
    if ( archive->reading )
        InterlockedIncrement( &archive->ref );
}

static void *zalloc(void *opaque, unsigned int items, unsigned int size)
{
    return malloc(items * size);
}

static void zfree(void *opaque, void *ptr)
{
    free(ptr);
}

static void compress_write_content(struct zip_archive *archive, IStream *content,
        OPC_COMPRESSION_OPTIONS options, struct zip_file *file)
{
    int level, flush;
    z_stream z_str;
    LARGE_INTEGER move;
    ULONG num_read;
    HRESULT hr;
    int init_ret;

    if (FAILED(archive->status))
        return;

    file->crc32 = RtlComputeCrc32(0, NULL, 0);
    file->compressed_size = file->uncompressed_size = 0;

    move.QuadPart = 0;
    IStream_Seek(content, move, STREAM_SEEK_SET, NULL);

    switch (options)
    {
    case OPC_COMPRESSION_NONE:
        level = Z_NO_COMPRESSION;
        break;
    case OPC_COMPRESSION_NORMAL:
        level = Z_DEFAULT_COMPRESSION;
        break;
    case OPC_COMPRESSION_MAXIMUM:
        level = Z_BEST_COMPRESSION;
        break;
    case OPC_COMPRESSION_FAST:
        level = 2;
        break;
    case OPC_COMPRESSION_SUPERFAST:
        level = Z_BEST_SPEED;
        break;
    default:
        WARN("Unsupported compression options %d.\n", options);
        level = Z_DEFAULT_COMPRESSION;
    }

    memset(&z_str, 0, sizeof(z_str));
    z_str.zalloc = zalloc;
    z_str.zfree = zfree;
    if ((init_ret = deflateInit2(&z_str, level, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY)) != Z_OK)
    {
        WARN("deflateInit2() failed, ret %d.\n", init_ret);
        archive->status = OPC_E_ZIP_COMPRESSION_FAILED;
        return;
    }

    do
    {
        int ret;

        if (FAILED(hr = IStream_Read(content, archive->input_buffer, sizeof(archive->input_buffer), &num_read)))
        {
            archive->status = hr;
            break;
        }

        z_str.avail_in = num_read;
        z_str.next_in = archive->input_buffer;
        file->crc32 = RtlComputeCrc32(file->crc32, archive->input_buffer, num_read);
        file->uncompressed_size += num_read;

        flush = sizeof(archive->input_buffer) > num_read ? Z_FINISH : Z_NO_FLUSH;

        do
        {
            ULONG have;

            z_str.avail_out = sizeof(archive->output_buffer);
            z_str.next_out = archive->output_buffer;

            if ((ret = deflate(&z_str, flush)) < 0)
            {
                WARN("Failed to deflate(), ret %d.\n", ret);
                archive->status = OPC_E_ZIP_COMPRESSION_FAILED;
                break;
            }
            have = sizeof(archive->output_buffer) - z_str.avail_out;
            compress_write(archive, archive->output_buffer, have);

            file->compressed_size += have;
        } while (z_str.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&z_str);
}

HRESULT compress_add_file(struct zip_archive *archive, const WCHAR *path,
        IStream *content, OPC_COMPRESSION_OPTIONS options)
{
    struct local_file_header local_header;
    struct zip_file *file;
    DWORD len;

    len = WideCharToMultiByte(CP_ACP, 0, path, -1, NULL, 0, NULL, NULL);

    if (!(file = calloc(1, offsetof(struct zip_file, name[len]))))
        return E_OUTOFMEMORY;

    WideCharToMultiByte(CP_ACP, 0, path, -1, file->name, len, NULL, NULL);
    file->offset = archive->position;
    file->name_length = len - 1;
    if (options != OPC_COMPRESSION_NONE)
    {
        file->method = Z_DEFLATED;
        if (options == OPC_COMPRESSION_MAXIMUM)
            file->flags = DEFLATE_MAX;
        else if (options == OPC_COMPRESSION_FAST)
            file->flags = DEFLATE_FAST;
        else if (options == OPC_COMPRESSION_SUPERFAST)
            file->flags = DEFLATE_SUPERFAST;
        else
            file->flags = DEFLATE_NORMAL;
    }
    file->flags |= USE_DATA_DESCRIPTOR;

    local_header.signature = LOCAL_HEADER_SIGNATURE;
    local_header.flags = USE_DATA_DESCRIPTOR;
    local_header.method = file->method;
    local_header.mtime = archive->mtime;
    local_header.crc32 = 0;
    local_header.name_length = len - 1;
    local_header.extra_length = 0;
    if (archive->zip64)
    {
        local_header.version = ZIP64_VERSION;
        local_header.compressed_size = ~0u;
        local_header.uncompressed_size = ~0u;
    }
    else
    {
        local_header.version = ZIP32_VERSION;
        local_header.compressed_size = 0;
        local_header.uncompressed_size = 0;
    }

    compress_write(archive, &local_header, sizeof(local_header));
    compress_write(archive, file->name, file->name_length);

    /* Content */
    compress_write_content(archive, content, options, file);

    /* Data descriptor */
    if (archive->zip64)
    {
        struct zip64_data_descriptor zip64_data_desc;

        zip64_data_desc.signature = DATA_DESCRIPTOR_SIGNATURE;
        zip64_data_desc.crc32 = file->crc32;
        zip64_data_desc.compressed_size = file->compressed_size;
        zip64_data_desc.uncompressed_size = file->uncompressed_size;
        compress_write(archive, &zip64_data_desc, sizeof(zip64_data_desc));
    }
    else
    {
        struct zip32_data_descriptor zip32_data_desc;

        zip32_data_desc.signature = DATA_DESCRIPTOR_SIGNATURE;
        zip32_data_desc.crc32 = file->crc32;
        zip32_data_desc.compressed_size = file->compressed_size;
        zip32_data_desc.uncompressed_size = file->uncompressed_size;
        compress_write(archive, &zip32_data_desc, sizeof(zip32_data_desc));
    }

    if (FAILED(archive->status))
    {
        free(file);
        return archive->status;
    }

    if (!opc_array_reserve((void **)&archive->files, &archive->file_size, archive->file_count + 1,
            sizeof(*archive->files)))
    {
        free(file);
        return E_OUTOFMEMORY;
    }

    archive->files[archive->file_count++] = file;

    return S_OK;
}

static HRESULT decompress_read_at(struct zip_archive *archive, uint64_t offset, BYTE *buffer, size_t length)
{
    LARGE_INTEGER seek;
    ULARGE_INTEGER new_position;
    size_t done = 0;
    HRESULT hr;

    if ( offset > (uint64_t)INT64_MAX )
        return OPC_E_ENUM_INVALID_POSITION;

    seek.QuadPart = (LONGLONG)offset;
    if ( FAILED( hr = IStream_Seek( archive->stream, seek, STREAM_SEEK_SET, &new_position ) ) )
        return hr;

    archive->position = new_position.QuadPart;

    while ( done < length )
    {
        ULONG request = (ULONG)((length - done) > __LONG_MAX__ ? __LONG_MAX__ : (length - done));
        ULONG received = 0;

        if ( FAILED( hr = IStream_Read( archive->stream, buffer + done, request, &received ) ) )
            return hr;

        if ( received != request )
            return OPC_E_ENUM_INVALID_POSITION;

        done += received;
        archive->position += received;
    }

    return S_OK;
}

static HRESULT decompress_parse_extra(BYTE *extra_data, size_t extra_length, uint32_t compressed32, uint32_t uncompressed32, uint32_t offset32, uint16_t disk16, uint64_t *compressed_out, uint64_t *uncompressed_out, uint64_t *offset_out)
{
    bool need_uncompressed = (uncompressed32 == 0xFFFFFFFFu);
    bool need_compressed   = (compressed32 == 0xFFFFFFFFu);
    bool need_offset       = (offset32 == 0xFFFFFFFFu);
    bool need_disk         = (disk16 == 0xFFFFu);
    bool found_zip64 = false;
    BYTE *field_data;
    size_t pos = 0;
    size_t remaining;
    uint32_t disk_number = disk16;
    uint16_t id, data_size;
    
    *compressed_out = compressed32;
    *uncompressed_out = uncompressed32;
    *offset_out = offset32;

    while ( pos < extra_length )
    {
        remaining = extra_length - pos;

        if ( remaining < 4 )
            return OPC_E_ENUM_INVALID_POSITION;

        id = read_u16_le( extra_data + pos );
        data_size = read_u16_le( extra_data + pos + 2 );
        pos += 4;

        if ( (size_t)data_size > extra_length - pos )
            return OPC_E_ENUM_INVALID_POSITION;

        field_data = extra_data + pos;

        if ( id == 0x0001 )
        {
            size_t value_pos = 0;

            if ( found_zip64 )
                return OPC_E_ENUM_INVALID_POSITION;
            found_zip64 = true;

            if ( need_uncompressed )
            {
                if ( (size_t)data_size - value_pos < 8 )
                    return OPC_E_ENUM_INVALID_POSITION;
                *uncompressed_out = read_u64_le( field_data + value_pos );
                value_pos += 8;
            }

            if ( need_compressed )
            {
                if ( (size_t)data_size - value_pos < 8 )
                    return OPC_E_ENUM_INVALID_POSITION;
                *compressed_out = read_u64_le( field_data + value_pos );
                value_pos += 8;
            }

            if ( need_offset )
            {
                if ( (size_t)data_size - value_pos < 8 )
                    return OPC_E_ENUM_INVALID_POSITION;
                *offset_out = read_u64_le( field_data + value_pos );
                value_pos += 8;
            }

            if ( need_disk )
            {
                if ( (size_t)data_size - value_pos < 4 )
                    return OPC_E_ENUM_INVALID_POSITION;
                disk_number = read_u32_le( field_data + value_pos );
                value_pos += 4;
            }
        }

        pos += data_size;
    }

    if ( (need_uncompressed || need_compressed || need_offset || need_disk) && !found_zip64 )
        return OPC_E_ENUM_INVALID_POSITION;

    // OPC does not support multi disk
    if ( disk_number != 0 )
        return E_NOTIMPL;

    return S_OK;
}

static HRESULT decompress_read_cd( struct zip_archive *archive, uint64_t directory_offset, uint64_t directory_size, uint64_t record_count )
{
    HRESULT hr = S_OK;
    BYTE *buffer = NULL;
    BYTE *entry;
    BYTE *name;
    BYTE *extra;
    size_t dir_size;
    size_t offset = 0;
    size_t current_file;
    size_t count;
    size_t variable_size;
    size_t allocation_size;
    size_t iterator;
    uint64_t compressed_size;
    uint64_t uncompressed_size;
    uint64_t local_offset;

    struct zip32_central_directory_header header;
     struct zip_file *file = NULL;

    if ( directory_size > SIZE_MAX ||
         directory_size > (uint64_t)INT64_MAX ||
         record_count > SIZE_MAX ||
         record_count > SIZE_MAX / sizeof(*archive->files) )
        return OPC_E_ENUM_INVALID_POSITION;

    dir_size = (size_t)directory_size;
    count = (size_t)record_count;

    if ( count > dir_size / sizeof(struct zip32_central_directory_header) )
        return OPC_E_ENUM_INVALID_POSITION;

    if ( directory_offset > (uint64_t)INT64_MAX ||
         directory_size > UINT64_MAX - directory_offset )
        return OPC_E_ENUM_INVALID_POSITION;

    if ( dir_size != 0 )
    {
        buffer = (BYTE *)malloc( dir_size * sizeof(BYTE) );
        if ( !buffer )
            return E_OUTOFMEMORY;

        if ( FAILED( hr = decompress_read_at( archive, directory_offset, buffer, dir_size ) ) )
            goto _CLEANUP;
    }

    archive->files = NULL;
    archive->file_count = count;

    if ( count != 0 )
    {
        if ( !(archive->files = (struct zip_file **)calloc( count, sizeof(*archive->files) )) )
        {
            hr = E_OUTOFMEMORY;
            goto _CLEANUP;
        }
    }

    for ( current_file = 0; current_file < count; current_file++ )
    {
        if ( offset > dir_size || dir_size - offset < sizeof(header) )
        {
            hr = OPC_E_ENUM_INVALID_POSITION;
            goto _CLEANUP;
        }

        entry = buffer + offset;

        if ( read_u32_le( entry ) != ZIP32_CDFH )
        {
            hr = OPC_E_ENUM_INVALID_POSITION;
            goto _CLEANUP;
        }

        memcpy( &header, entry, sizeof(header) );

        variable_size = (size_t)header.name_length + (size_t)header.extra_length + (size_t)header.comment_length;

        if ( variable_size > dir_size - offset - sizeof(header) )
        {
            hr = OPC_E_ENUM_INVALID_POSITION;
            goto _CLEANUP;
        }

        name = entry + sizeof(header);
        extra = name + header.name_length;

        if ( header.diskid != 0 && header.diskid != 0xFFFFu )
        {
            hr = OPC_E_ENUM_INVALID_POSITION;
            goto _CLEANUP;
        }

        hr = decompress_parse_extra( extra, header.extra_length, header.compressed_size, header.uncompressed_size, header.local_file_offset, header.diskid, &compressed_size, &uncompressed_size, &local_offset );
        if ( FAILED( hr ) ) goto _CLEANUP;

        allocation_size = offsetof(struct zip_file, name) + (size_t)header.name_length + 1;

        if ( !(file = (struct zip_file *)calloc(1, allocation_size)) )
        {
            hr = E_OUTOFMEMORY;
            goto _CLEANUP;
        }

        if ( header.name_length != 0 )
            memcpy(file->name, name, header.name_length);
        
        file->name[header.name_length] = '\0';

        file->name_length = header.name_length;
        file->offset = local_offset;
        file->crc32 = header.crc32;
        file->compressed_size = compressed_size;
        file->uncompressed_size = uncompressed_size;
        file->method = header.method;
        file->flags = header.flags;

        archive->files[current_file] = file;
        offset += sizeof(header) + variable_size;
    }

    if (offset != dir_size)
    {
        hr = OPC_E_ENUM_INVALID_POSITION;
        goto _CLEANUP;
    }

_CLEANUP:
    free( buffer );

    if ( FAILED( hr ) )
    {
        if ( archive->files )
        {
            for ( iterator = 0; iterator < archive->file_count; iterator++ )
                free( archive->files[iterator] );
            free( archive->files );
            archive->files = NULL;
        }
        archive->file_count = 0;
    }

    return hr;
}

static HRESULT decompress_read_zip32cd( struct zip_archive *archive, struct zip32_end_of_central_directory zip_eocd )
{
    if ( zip_eocd.diskid != 0 || zip_eocd.firstdisk != 0 || zip_eocd.records_num != zip_eocd.records_total )
        return OPC_E_ENUM_INVALID_POSITION;

    return decompress_read_cd( archive, zip_eocd.directory_offset, zip_eocd.directory_size, zip_eocd.records_num );
}

static HRESULT decompress_read_zip64cd(
    struct zip_archive *archive,
    struct zip64_end_of_central_directory zip64_eocd)
{
    if ( zip64_eocd.diskid != 0 || zip64_eocd.directory_diskid != 0 || zip64_eocd.records_num != zip64_eocd.records_total )
        return OPC_E_ENUM_INVALID_POSITION;

    if ( zip64_eocd.min_version < ZIP64_VERSION )
        return OPC_E_ENUM_INVALID_POSITION;

    return decompress_read_cd( archive, zip64_eocd.directory_offset, zip64_eocd.directory_size, zip64_eocd.records_num );
}

static HRESULT decompress_read_eocd(struct zip_archive *archive, size_t archive_size)
{
    HRESULT hr = S_OK;
    uint16_t comment_length;
    uint64_t locator_offset;
    uint64_t zip64_end;
    size_t eocdBlockSize = 22l + 65535l;
    size_t absolute_eocd_offset;
    size_t searchSize = min( archive_size, eocdBlockSize );
    size_t iterator = 0;
    size_t file_index;
    PBYTE dataPointer = NULL;
    BYTE *buffer = NULL;
    BYTE locator_bytes[sizeof(struct zip64_end_of_central_directory_locator)];
    BYTE zip64_bytes[sizeof(struct zip64_end_of_central_directory)];
    bool containsLocator = FALSE;
    bool zip64_sentinel;
    bool found = FALSE;

    struct zip32_end_of_central_directory zip_eocd;
    struct zip64_end_of_central_directory_locator locator;
    struct zip64_end_of_central_directory zip64_eocd;

    if ( archive_size < 22 )
        return OPC_E_ENUM_CANNOT_MOVE_NEXT;
    
    buffer = malloc( searchSize * sizeof(BYTE) );
    if ( !buffer )
        return E_OUTOFMEMORY;

    if ( FAILED( hr = decompress_read_at( archive, (uint64_t)(archive_size - searchSize), buffer, searchSize ) ) )
        goto _CLEANUP;

    for ( iterator = searchSize - 22 + 1; iterator-- > 0; )
    {
        containsLocator = FALSE;
        zip64_sentinel = FALSE;

        dataPointer = buffer + iterator;

        if ( read_u32_le( dataPointer ) != ZIP32_EOCD )
            continue;

        comment_length = read_u16_le( dataPointer + 20 );
        if ( (size_t)comment_length != searchSize - iterator - 22 )
            continue;

        absolute_eocd_offset = archive_size - searchSize + iterator;
        if ( (iterator >= sizeof(struct zip64_end_of_central_directory_locator)) && 
             (read_u32_le( dataPointer - sizeof(struct zip64_end_of_central_directory_locator) ) == ZIP64_EOCD64_LOCATOR) )
            containsLocator = TRUE;

        if ( read_u16_le( dataPointer + 8 ) == 0xFFFFu ||
             read_u16_le( dataPointer + 10 ) == 0xFFFFu ||
             read_u32_le( dataPointer + 12 ) == 0xFFFFFFFFu ||
             read_u32_le( dataPointer + 16 ) == 0xFFFFFFFFu )
            zip64_sentinel = TRUE;
            
        if ( zip64_sentinel && !containsLocator )
        {
            hr = OPC_E_ENUM_INVALID_POSITION;
            goto _CLEANUP;
        }

        if ( containsLocator )
        {
            locator_offset = (uint64_t)absolute_eocd_offset - sizeof(locator);

            memcpy( locator_bytes, dataPointer - sizeof(locator), sizeof(locator_bytes) );
            memcpy( &locator, locator_bytes, sizeof(locator) );

            if ( read_u32_le( locator_bytes ) != ZIP64_EOCD64_LOCATOR )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            if ( locator.eocd64_disk != 0 || locator.disk_num != 1 )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            if ( locator.eocd64_offset > (uint64_t)INT64_MAX )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            if ( FAILED( hr = decompress_read_at( archive, locator.eocd64_offset,  zip64_bytes, sizeof(zip64_bytes) ) ) )
                goto _CLEANUP;

            if ( read_u32_le(zip64_bytes) != ZIP64_EOCD64 )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            memcpy( &zip64_eocd, zip64_bytes, sizeof(zip64_eocd) );

            /* ZIP64 EOCD size excludes the 12-byte signature+size prefix. */
            if ( zip64_eocd.size < 44 || zip64_eocd.size > UINT64_MAX - 12 )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            zip64_end = locator.eocd64_offset + 12 + zip64_eocd.size;
            if ( zip64_end < locator.eocd64_offset || zip64_end > locator_offset )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            if ( zip64_eocd.diskid != 0 ||
                 zip64_eocd.directory_diskid != 0 ||
                 zip64_eocd.records_num != zip64_eocd.records_total ||
                 zip64_eocd.min_version < ZIP64_VERSION ||
                 zip64_eocd.directory_offset > UINT64_MAX - zip64_eocd.directory_size ||
                 zip64_eocd.directory_offset + zip64_eocd.directory_size >
                     locator.eocd64_offset )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            archive->zip64 = TRUE;
            
            if ( FAILED(hr = decompress_read_zip64cd( archive, zip64_eocd ) ) )
                goto _CLEANUP;

            {
                for ( file_index = 0; file_index < archive->file_count; file_index++ )
                {
                    if ( archive->files[file_index]->offset >= zip64_eocd.directory_offset ||
                         archive->files[file_index]->offset > archive_size ||
                         archive_size - (size_t)archive->files[file_index]->offset <
                             sizeof(struct local_file_header) )
                    {
                        hr = OPC_E_ENUM_INVALID_POSITION;
                        goto _CLEANUP;
                    }
                }
            }

            found = true;
            break;
        } else
        {
            memcpy( &zip_eocd, dataPointer, sizeof(zip_eocd) );

            if ( zip_eocd.diskid != 0 ||
                 zip_eocd.firstdisk != 0 ||
                 zip_eocd.records_num != zip_eocd.records_total )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            if ( (uint64_t)zip_eocd.directory_offset + (uint64_t)zip_eocd.directory_size > (uint64_t)absolute_eocd_offset )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto _CLEANUP;
            }

            archive->zip64 = FALSE;

            if ( FAILED( hr = decompress_read_zip32cd( archive, zip_eocd ) ) )
                goto _CLEANUP;

            {
                for ( file_index = 0; file_index < archive->file_count; file_index++ )
                {
                    if ( archive->files[file_index]->offset >= zip_eocd.directory_offset ||
                         archive->files[file_index]->offset > archive_size ||
                         archive_size - (size_t)archive->files[file_index]->offset <
                             sizeof(struct local_file_header) )
                    {
                        hr = OPC_E_ENUM_INVALID_POSITION;
                        goto _CLEANUP;
                    }
                }
            }

            found = true;
            break;
        }
    }

    if ( !found )
        hr = HRESULT_FROM_WIN32( ERROR_EMPTY );

_CLEANUP:
    free( buffer );
    return hr;
}

static HRESULT decompress_get_size(struct zip_archive *archive, size_t *out)
{
    HRESULT hr;
    ULARGE_INTEGER size = {};
    LARGE_INTEGER offset = {};
    offset.QuadPart = 0;
    
    if ( FAILED( hr = IStream_Seek( archive->stream, offset, STREAM_SEEK_END, &size ) ) ) 
        return hr;

    if ( FAILED( hr = IStream_Seek( archive->stream, offset, STREAM_SEEK_SET, NULL ) ) ) 
        return hr;

    *out = size.QuadPart;

    return hr;
}

HRESULT decompress_get_file_count(struct zip_archive *archive, size_t *count)
{
    if ( !count )
        return E_POINTER;

    *count = archive->file_count;

    return S_OK;
}

HRESULT decompress_get_file_name(struct zip_archive *archive, size_t index, LPSTR name, size_t *size)
{
    if ( index >= archive->file_count )
        return E_BOUNDS;

    if ( !name )
    {
        //get size path
        if ( !size )
            return E_POINTER;

        *size = strlen( archive->files[index]->name ) + 1;
    } else
    {
        strcpy( name, archive->files[index]->name );
    }

    return S_OK;
}

HRESULT decompress_get_uncompressed_file_size(struct zip_archive *archive, size_t index, UINT64 *size)
{
    if ( index >= archive->file_count )
        return E_BOUNDS;

    if ( !size )
        return E_POINTER;

    *size = archive->files[index]->uncompressed_size;

    return S_OK;
}

HRESULT decompress_get_file_compression(struct zip_archive *archive, size_t index, OPC_COMPRESSION_OPTIONS *compression)
{
    if ( index >= archive->file_count )
        return E_BOUNDS;

    if ( !compression )
        return E_POINTER;

    if ( archive->files[index]->method == 0 )
    {
        *compression = OPC_COMPRESSION_NONE;
    } else if ( archive->files[index]->method == 8 )
    {
        *compression = OPC_COMPRESSION_NORMAL;
    }

    return S_OK;
}

HRESULT decompress_extract_file(struct zip_archive *archive, size_t index, IStream *output)
{
    HRESULT hr = S_OK;
    struct zip_file *file;
    BYTE local_header[30];
    BYTE input_buffer[(1024 * 64)-1];
    BYTE output_buffer[(1024 * 64)-1];
    uint64_t data_offset;
    uint64_t input_remaining;
    uint64_t output_total = 0;
    uint64_t input_size;
    uint64_t output_size;
    uint32_t crc;
    uint16_t flags;
    uint16_t method;
    uint16_t name_length;
    uint16_t extra_length;
    ULONG written;
    ULONG request;
    UINT previous_in;
    UINT previous_out;
    size_t write_offset;
    size_t chunk;
    size_t produced;
    z_stream zs;
    int zret;
    bool initialized = false;
    bool stream_end = false;

    if ( !archive || !output )
        return E_POINTER;

    if ( index >= archive->file_count )
        return E_BOUNDS;

    file = archive->files[index];

    if ( !file )
        return E_POINTER;

    input_size = file->compressed_size;
    output_size = file->uncompressed_size;

    // TODO: implement encryption
    if ( file->flags & ( 0x0001 | 0x0040 | 0x2000 ) )
        return E_NOTIMPL;

    if ( file->method != 0 && file->method != 8 )
        return E_NOTIMPL;

    if ( file->offset > (uint64_t)INT64_MAX )
        return OPC_E_ENUM_INVALID_POSITION;

    hr = decompress_read_at( archive, file->offset, local_header, sizeof(local_header) );

    if ( FAILED( hr ) )
        return hr;

    if ( read_u32_le( local_header ) != 0x04034b50 )
        return OPC_E_ENUM_INVALID_POSITION;

    flags = read_u16_le( local_header + 6 );
    method = read_u16_le( local_header + 8 );
    name_length = read_u16_le( local_header + 26 );
    extra_length = read_u16_le( local_header + 28 );

    if ( flags != file->flags || method != file->method )
        return OPC_E_ENUM_INVALID_POSITION;

    data_offset = file->offset + sizeof(local_header);

    if ( data_offset > UINT64_MAX - name_length - extra_length )
        return OPC_E_ENUM_INVALID_POSITION;

    data_offset += (uint64_t)name_length + extra_length;

    if ( data_offset > (uint64_t)INT64_MAX || input_size > (uint64_t)INT64_MAX - data_offset )
        return OPC_E_ENUM_INVALID_POSITION;

    crc = (uint32_t)crc32(0L, Z_NULL, 0);

    if ( file->method == 0 )
    {
        //uncompressed
        if ( input_size != output_size )
            return OPC_E_ENUM_INVALID_POSITION;

        input_remaining = input_size;

        while ( input_remaining )
        {
            chunk = input_remaining > sizeof(input_buffer) ? sizeof(input_buffer) : (size_t)input_remaining;

            hr = decompress_read_at( archive, data_offset, input_buffer, chunk );

            if ( FAILED( hr ) )
                return hr;

            data_offset += chunk;
            input_remaining -= chunk;
            output_total += chunk;

            crc = (uint32_t)crc32( crc, input_buffer, (uInt)chunk );

            write_offset = 0;

            while ( write_offset < chunk )
            {
                request = (ULONG)(chunk - write_offset);

                written = 0;

                hr = IStream_Write( output, input_buffer + write_offset, request, &written );

                if ( FAILED( hr ) )
                    return hr;

                if ( written == 0 || written > request )
                    return STG_E_WRITEFAULT;

                write_offset += written;
            }
        }

        if ( output_total != output_size || crc != file->crc32 )
            return OPC_E_ENUM_INVALID_POSITION;

        return S_OK;
    }

    memset( &zs, 0, sizeof(zs) );

    zret = inflateInit2( &zs, -MAX_WBITS );

    if ( zret != Z_OK )
        return zret == Z_MEM_ERROR ? E_OUTOFMEMORY : OPC_E_ENUM_INVALID_POSITION;

    initialized = true;
    input_remaining = input_size;

    while ( !stream_end )
    {
        if ( zs.avail_in == 0 && input_remaining != 0 )
        {
            chunk = input_remaining > sizeof(input_buffer) ? sizeof(input_buffer) : (size_t)input_remaining;

            hr = decompress_read_at( archive, data_offset, input_buffer, chunk );

            if ( FAILED( hr ) ) goto CLEANUP;

            data_offset += chunk;
            input_remaining -= chunk;

            zs.next_in = input_buffer;
            zs.avail_in = (uInt)chunk;
        }

        zs.next_out = output_buffer;
        zs.avail_out = sizeof(output_buffer);

        do
        {
            previous_in = zs.avail_in;
            previous_out = zs.avail_out;

            zret = inflate( &zs, Z_NO_FLUSH );

            if ( zret != Z_OK && zret != Z_STREAM_END )
            {
                hr = zret == Z_MEM_ERROR ? E_OUTOFMEMORY : OPC_E_ENUM_INVALID_POSITION;
                goto CLEANUP;
            }

            {
                produced = sizeof(output_buffer) - zs.avail_out;

                if ( produced != 0 )
                {
                    if ( output_total > output_size || produced > output_size - output_total )
                    {
                        hr = OPC_E_ENUM_INVALID_POSITION;
                        goto CLEANUP;
                    }

                    output_total += produced;

                    crc = (uint32_t)crc32( crc, output_buffer, (UINT)produced );

                    write_offset = 0;

                    while ( write_offset < produced )
                    {
                        ULONG request = (ULONG)(produced - write_offset);

                        written = 0;

                        hr = IStream_Write( output, output_buffer + write_offset, request, &written );

                        if ( FAILED( hr ) )
                            goto CLEANUP;

                        if ( written == 0 || written > request )
                        {
                            hr = STG_E_WRITEFAULT;
                            goto CLEANUP;
                        }

                        write_offset += written;
                    }
                }
            }

            if ( zret == Z_STREAM_END )
            {
                stream_end = true;
                break;
            }

            if ( previous_in == zs.avail_in && previous_out == zs.avail_out )
            {
                hr = OPC_E_ENUM_INVALID_POSITION;
                goto CLEANUP;
            }

        } while ( zs.avail_in != 0 || zs.avail_out == 0 );

        if ( !stream_end && input_remaining == 0 && zs.avail_in == 0 )
        {
            hr = OPC_E_ENUM_INVALID_POSITION;
            goto CLEANUP;
        }
    }

    if ( input_remaining != 0 || zs.avail_in != 0 ||  output_total != output_size || crc != file->crc32 )
    {
        hr = OPC_E_ENUM_INVALID_POSITION;
        goto CLEANUP;
    }

CLEANUP:
    if ( initialized )
        inflateEnd( &zs );

    return hr;
}


HRESULT decompress_open_archive(IStream *stream, struct zip_archive **out)
{
    struct zip_archive *archive;
    WORD date, time;
    size_t archive_size;
    HRESULT hr = S_OK;
    FILETIME ft;

    if (!(archive = malloc(sizeof(*archive))))
        return E_OUTOFMEMORY;

    archive->files = NULL;
    archive->file_size = 0;
    archive->file_count = 0;
    archive->ref = 1;
    archive->status = S_OK;
    archive->reading = TRUE;
    archive->zip64 = FALSE;

    archive->stream = stream;
    IStream_AddRef(archive->stream);
    archive->position = 0;

    if ( FAILED( hr = decompress_get_size( archive, &archive_size ) ) )
        return hr;

    if ( FAILED( hr = decompress_read_eocd( archive, archive_size ) ) )
        return hr;

    GetSystemTimeAsFileTime(&ft);
    FileTimeToDosDateTime(&ft, &date, &time);
    archive->mtime = date << 16 | time;

    *out = archive;

    return S_OK;
}