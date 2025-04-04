/* WinRT Windows.Storage.Search.IQueryOptions Implementation
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

#ifndef QUERY_OPTIONS_INTERNAL_H
#define QUERY_OPTIONS_INTERNAL_H

#include "../../private.h"
#include "wine/debug.h"

extern const struct IQueryOptionsVtbl query_options_vtbl;
extern const struct IQueryOptionsFactoryVtbl query_options_factory_vtbl;

struct query_options_factory
{
    IActivationFactory IActivationFactory_iface;
    IQueryOptionsFactory IQueryOptionsFactory_iface;
    LONG ref;
};

struct query_options
{
    //Derivatives
    IQueryOptions IQueryOptions_iface;
    IVector_HSTRING *FileTypeFilters;
    IVector_SortEntry *SortOrders;
    FolderDepth Depth;    
    IndexerOption Indexer;
    DateStackOption DateStack;
    HSTRING ApplicationSearchFilter;
    HSTRING UserSearchFilter;
    HSTRING Language;
    HSTRING GroupPropertyName;

    LONG ref;
};

#endif