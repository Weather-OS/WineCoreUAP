/* 
 * AppX Packaging Service
 *
 * Written by Weather.
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

#define COBJMACROS
#include <initguid.h>
#include "private.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appxp);

/***********************************************************************
 *              DllGetClassObject (appxpackaging.@)
 */
HRESULT WINAPI DllGetClassObject( REFCLSID clsid, REFIID riid, void **out )
{
    if ( IsEqualGUID(clsid, &CLSID_AppxFactory) )
        return IClassFactory_QueryInterface(appx_factory, riid, out);

    *out = NULL;
    FIXME( "Unknown clsid %s.\n", debugstr_guid(clsid) );
    return CLASS_E_CLASSNOTAVAILABLE;
}
