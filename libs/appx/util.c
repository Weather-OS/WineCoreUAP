/**
 * WineCoreUAP Appx Parser (Util module).
 * 
 * Written by Weather
 * 
 * This is a general utility module.
 */

#include <stdio.h>
#include <string.h>

#include "include/util.h"

struct error_info errorSeed;

/**
 * setLastErrorMessage
 */
void
setLastErrorMessage( const char * str )
{
    strcpy( errorSeed.msg, str );
    return;
}

/**
 * setLastErrorCode
 */
void
setLastErrorCode( int errorCode )
{
    errorSeed.code = errorCode;
    return;
}

/**
 * initErrorSeed
 */
void
initErrorSeed( void )
{
    memset(&errorSeed, 0, sizeof(errorSeed));
    return;
}

/**
 * errorMessage
 */
void
errorMessage( const char * str, ... )
{
    char err[4096];
    va_list va;
    const char * fmterr;

    fmterr = str;
    va_start(va, str);
    vsprintf(err, str, va);
    fprintf(stderr, "appx: error %s\n", err);
    setLastErrorMessage( err );
}

