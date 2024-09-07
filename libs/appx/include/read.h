/**
 * WineCoreUAP Appx Parser.
 * 
 * Written by Weather
 * 
 * This module is for parsing appx manifests.
 */

#ifndef _READ_H_
#define _READ_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "util.h"

appxstatus readManifest( const char *manifestpath, xmlNode** node );
appxstatus verifyManifestRoot( xmlNode *manifestRoot );

#endif