/* WinRT Windows.Storage Internal Utilities.
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

#include "../private.h"

#ifndef _UTIL_INTERNAL_H_
#define _UTIL_INTERNAL_H_

#include <stdio.h>

#define WINDOWS_TICK 10000000
#define SEC_TO_UNIX_EPOCH 11644473600LL

INT64 FileTimeToUnixTime( const FILETIME *ft );
VOID GenerateUniqueFileName( LPSTR buffer, SIZE_T bufferSize );
LPCWSTR CharToLPCWSTR( LPSTR charString );
LPCSTR HStringToLPCSTR( HSTRING hString );
VOID DeleteDirectoryRecursively(LPCSTR directoryPath);

#endif