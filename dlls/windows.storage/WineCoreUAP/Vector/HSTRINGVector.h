/* WinRT IVector<HSTRING> Implementation
 *
 * Written by Weather
 *
 * This is a reverse engineered implementation of Microsoft's OneCoreUAP binaries.
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


#ifndef HSTRING_VECTOR_H
#define HSTRING_VECTOR_H

#include "../../private.h"
#include "wine/debug.h"

extern struct IVectorView_HSTRINGVtbl hstring_vector_view_vtbl;
extern struct IIterable_HSTRINGVtbl hstring_iterable_vtbl;

struct hstring_vector
{
    IVector_HSTRING IVector_HSTRING_iface;
    struct vector_iids iids;
    LONG ref;

    UINT32 size;
    HSTRING *elements;
};

struct hstring_vector *impl_from_IVector_HSTRING( IVector_HSTRING *iface );

#endif