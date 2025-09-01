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

#ifndef PATH_IO_INTERNAL_H
#define PATH_IO_INTERNAL_H

#include "StorageFileInternal.h"

#include "../private.h"
#include "wine/debug.h"

#define MAX_BUFFER 4096

struct path_io_statics
{
    IActivationFactory IActivationFactory_iface;
    IPathIOStatics IPathIOStatics_iface;
    LONG ref;
};

#endif