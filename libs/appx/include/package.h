/**
 * WineCoreUAP Appx Package.
 * 
 * Written by Weather
 * 
 * This module is for appx packaging.
 */

#ifndef _PACKAGE_H_
#define _PACKAGE_H_

#include "util.h"
#include "read.h"
#include "version.h"

typedef xmlNode* appxResourceList;

typedef xmlNode* appxCapabilityList;

struct appx_properties {
    const wchar_t * DisplayName;
    const wchar_t * PublisherDisplayName;
    const wchar_t * Logo;
};

struct appx_identity {
    const wchar_t * Name;
    const wchar_t * Publisher;
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
    const wchar_t* manifestPath;
    xmlNode* manifest;

    struct appx Package;
};

appxstatus registerAppxPackage( const wchar_t * manifestPath, struct appx_package *package );

#endif
