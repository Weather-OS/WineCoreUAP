/**
 * WineCoreUAP Appx Parser (Util module).
 * 
 * Written by Weather
 * 
 * This is a general utility module.
 */

#ifndef ___UTIL__H_
#define ___UTIL__H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#define STATUS_SUCCESS 0
#define STATUS_FAIL 1

#define OK( status ) ( ( status ) == STATUS_SUCCESS )

typedef bool appxstatus;

enum Architecture {
    PROCESSOR_X86,
    PROCESSOR_X64,
    PROCESSOR_ARM64
};

struct error_info {
    short code;
    char * msg;
};

void setLastErrorMessage( const char * str );
void setLastErrorCode( int errorCode );
void initErrorSeed( void );
void errorMessage( const char * str, ... );
wchar_t* charToWChar(const char *charStr);
#endif