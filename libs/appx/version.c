/**
 * WineCoreUAP Appx Versioning.
 * 
 * Written by Weather
 * 
 * This module is for appx versioning.
 */

#include "include/version.h"

/**
 * ParseAppVersion
 */
appxstatus
ParseAppVersion( const xmlChar* versionStr, struct app_version *parsedVersion )
{
    int result = sscanf(versionStr, "%hu.%hu.%hu.%hu", 
                        &parsedVersion->releaseVersion, 
                        &parsedVersion->majorVersion, 
                        &parsedVersion->minorVersion, 
                        &parsedVersion->patchVersion);

    if ( result != 4 )
    {
        errorMessage( "Element \"Package->Identity.Version\" has an incorrect format.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}