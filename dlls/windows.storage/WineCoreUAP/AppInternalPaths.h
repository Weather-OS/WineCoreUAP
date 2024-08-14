#include <windows.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>
#include <combaseapi.h>

#include "../private.h"
#include "wine/debug.h"

HRESULT WINAPI app_data_paths_GetKnownFolder( IAppDataPaths *iface, const char *FOLDERID, HSTRING *value );