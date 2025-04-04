/* WinRT Windows.Storage.SetVersion* Implementation
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

#ifndef SET_VERSION_INTERNAL_H
#define SET_VERSION_INTERNAL_H

#include "../private.h"
#include "wine/debug.h"

struct set_version
{
    ISetVersionRequest ISetVersionRequest_iface;
    ISetVersionDeferral ISetVersionDeferral_iface;
    UINT32 CurrentVersion;
    UINT32 DesiredVersion;
    LONG ref;
};

struct set_version *impl_from_ISetVersionRequest( ISetVersionRequest *iface );

#endif