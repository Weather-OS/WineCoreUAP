/* WinRT Windows.Management.Deployment Implementation
 *
 * Copyright (C) 2023 Mohamad Al-Jaf
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

#ifndef __WINE_APPX_DEPLOYMENT_SERVER_PRIVATE_H
#define __WINE_APPX_DEPLOYMENT_SERVER_PRIVATE_H

#include <stdarg.h>

#define COBJMACROS
#include "windef.h"
#include "winbase.h"
#include "winstring.h"
#include "windows.h"
#include "winsvc.h"

#include "activation.h"
#include "appx.h"
#include "msopc.h"
#include "appxpackaging.h"

#ifdef __cplusplus
// Bug: WinRT in C++ within Wine lacks proper C++ type handling
// Redefine boolean as bool, and DOUBLE as double to prevent compile issues.
// Casting is purely handled by libstdc++, so it's not an issue here.
#undef boolean
#define boolean bool
#undef DOUBLE
#define DOUBLE double

// Bug: __WINESRC__ is not defined in C++ contexts.
#define __WINESRC__ 1
#endif

#include "wine/debug.h"

extern IClassFactory* appx_factory;

#endif
