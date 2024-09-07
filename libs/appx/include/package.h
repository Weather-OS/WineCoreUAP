/**
 * WineCoreUAP Appx Package.
 * 
 * Written by Weather
 * 
 * This module is for appx packaging.
 */

#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "util.h"
#include "read.h"
#include "version.h"

typedef xmlNode* appxResourceList;

typedef xmlNode* appxCapabilityList;

struct appx_properties {
    const xmlChar * DisplayName;
    const xmlChar * PublisherDisplayName;
    const xmlChar * Logo;
};

struct appx_identity {
    const xmlChar * Name;
    const xmlChar * Publisher;
    struct app_version Version;
    enum Architecture Architecture;
};

struct appx {
    struct appx_identity Identity;
    struct appx_properties Properties;
    appxResourceList Resources;
    appxCapabilityList Capabilities;
};

struct appx_package {
    const char* manifestPath;
    xmlNode* manifest;

    struct appx Package;
};

appxstatus registerAppxPackage( const char * manifestPath, struct appx_package *package );

#endif
