/**
 * APPX Version File
 */

#ifndef _VERSION_H_
#define _VERSION_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "util.h"

struct app_version {
    unsigned short releaseVersion;
    unsigned short majorVersion;
    unsigned short minorVersion;
    unsigned short patchVersion;
};

appxstatus ParseAppVersion( const xmlChar* versionStr, struct app_version *parsedVersion );

#endif