#include "AppInternalPaths.h"

#define BUFFER_SIZE 1024

void extractNameAttribute(char* line) {
    char *nameStart = strstr(line, "Name=\"");
    char *nameEnd;
    if (nameStart) {
        nameStart += 6; // Move past 'Name="'
        nameEnd = strchr(nameStart, '"');
        if (nameEnd) {
            *nameEnd = '\0'; // Null-terminate the string at the end quote
            printf("Package->Identity->Name: %s\n", nameStart);
        }
    }
}

HRESULT WINAPI app_data_paths_GetKnownFolder( IAppDataPaths *iface, const char *FOLDERID, HSTRING *value ) 
{
    PWSTR path;
    FILE *file;
    WCHAR username[256];
    WCHAR manifestPath[MAX_PATH];
    char buffer[BUFFER_SIZE];
    GetModuleFileNameW(NULL, manifestPath, MAX_PATH);
    PathRemoveFileSpecW(manifestPath);
    PathAppendW(manifestPath, L"AppxManifest.xml");

    file = fopen((char *)manifestPath, "r");
    if (!file) {
        MessageBoxW(NULL, L"Failed to read AppxManifest.xml", L"WineCoreUAP", MB_ICONERROR);
        return 1;
    }
    while (fgets(buffer, BUFFER_SIZE, file)) {
        if (strstr(buffer, "<Identity")) {
            extractNameAttribute(buffer);
            break;
        }
    }
    fclose(file);
    
    if (!strcmp(FOLDERID, "cookies"))
    {
        //Need to fix FOLDERID_LocalAppData
        path = L"C:\\users\\";
        GetUserNameW(username, (LPDWORD)strlen((char *)username));
        PathAppendW(path, username);
        PathAppendW(path, L"AppData\\Local");
        PathAppendW(path, L"Packages");
        PathAppendW(path, (WCHAR *)buffer);
        PathAppendW(path, L"AC\\INetCookies");
        WindowsCreateString(path, strlen((char *)path), value);
    }
    return S_OK;
}
