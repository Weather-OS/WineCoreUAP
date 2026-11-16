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

#include <stdbool.h>

#include "msopc.h"

static inline BOOL opc_array_reserve(void **elements, size_t *capacity, size_t count, size_t size)
{
    size_t new_capacity, max_capacity;
    void *new_elements;

    if (count <= *capacity)
        return TRUE;

    max_capacity = ~(SIZE_T)0 / size;
    if (count > max_capacity)
        return FALSE;

    new_capacity = max(4, *capacity);
    while (new_capacity < count && new_capacity <= max_capacity / 2)
        new_capacity *= 2;
    if (new_capacity < count)
        new_capacity = max_capacity;

    if (!(new_elements = realloc(*elements, new_capacity * size)))
        return FALSE;

    *elements = new_elements;
    *capacity = new_capacity;
    return TRUE;
}

struct opc_uri
{
    IOpcPartUri IOpcPartUri_iface;
    LONG refcount;
    BOOL is_part_uri;

    IUri *uri;
    IUri *rels_part_uri;
    struct opc_uri *source_uri;
};

extern HRESULT opc_part_uri_create(IUri *uri, struct opc_uri *source_uri, IOpcPartUri **part_uri);
extern HRESULT opc_root_uri_create(IOpcUri **opc_uri);

extern HRESULT opc_package_write(IOpcPackage *package, OPC_WRITE_FLAGS flags, IStream *stream);
extern HRESULT opc_package_read(IOpcFactory *factory, IStream *stream, OPC_READ_FLAGS flags, IOpcPackage **package);

struct zip_archive;
extern HRESULT opc_package_create(IOpcFactory *factory, struct zip_archive *archive, IOpcPackage **package);
extern HRESULT compress_create_archive(IStream *stream, bool zip64, struct zip_archive **archive);
extern HRESULT compress_add_file(struct zip_archive *archive, const WCHAR *path, IStream *content,
        OPC_COMPRESSION_OPTIONS options);
extern HRESULT compress_finalize_archive(struct zip_archive *archive);
extern void compress_release_archive(struct zip_archive *archive);
extern void compress_addref_archive(struct zip_archive *archive);
extern HRESULT decompress_open_archive(IStream *stream, struct zip_archive **out);
extern HRESULT decompress_get_file_count(struct zip_archive *archive, size_t *count);
extern HRESULT decompress_get_file_name(struct zip_archive *archive, size_t index,
        LPSTR name, size_t *size);
extern HRESULT decompress_get_uncompressed_file_size(struct zip_archive *archive, size_t index, UINT64 *size);
extern HRESULT decompress_get_file_compression(struct zip_archive *archive, size_t index, OPC_COMPRESSION_OPTIONS *compression);
extern HRESULT decompress_extract_file(struct zip_archive *archive, size_t index,
        IStream *output);