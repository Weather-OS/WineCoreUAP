/**
 * WineCoreUAP Appx Package.
 * 
 * Written by Weather
 * 
 * This module is for appx packaging.
 */

#include "include/package.h"

static appxstatus assignAppxIdentity( xmlNode * manifest, struct appx_identity *Identity );
static appxstatus assignAppxProperties( xmlNode * manifest, struct appx_properties *Properties );
static appxstatus assignAppxCapabilities( xmlNode * manifest, appxCapabilityList *List );
static appxstatus assignAppxResources( xmlNode * manifest, appxResourceList *List );

/**
 * registerAppxPackage
 */
appxstatus
registerAppxPackage( const wchar_t * manifestPath, struct appx_package *package )
{
    appxstatus status;

    size_t len;
    char *manifestPathStr;

    len = wcstombs(NULL, manifestPath, 0) + 1;
    if (len == (size_t)-1) {
        return STATUS_FAIL;
    }

    manifestPathStr = malloc(len);
    if (manifestPathStr == NULL) {
        return STATUS_FAIL;
    }

    wcstombs(manifestPathStr, manifestPath, len);

    package->manifestPath = manifestPath;
    status = readManifest( manifestPathStr, &package->manifest );
    if ( !OK( status ) ) return STATUS_FAIL;

    status = verifyManifestRoot( package->manifest );
    if ( !OK( status ) ) return STATUS_FAIL;

    status = assignAppxIdentity( package->manifest, &package->Package.Identity );
    if ( !OK( status ) ) return STATUS_FAIL;

    status = assignAppxProperties( package->manifest, &package->Package.Properties );
    if ( !OK( status ) ) return STATUS_FAIL;

    status = assignAppxCapabilities( package->manifest, &package->Package.Capabilities );
    if ( !OK( status ) ) return STATUS_FAIL;

    status = assignAppxResources( package->manifest, &package->Package.Resources );

    return status;
}

static appxstatus
assignAppxIdentity( xmlNode * manifest, struct appx_identity *Identity )
{
    const xmlChar* Name;
    const xmlChar* Publisher;
    const xmlChar* Version;
    const xmlChar* ProcArchitecture;

    struct app_version Parsed_Version;

    for ( xmlNode *node = manifest; node; node = node->next ) 
    {
        if ( node->type == XML_ELEMENT_NODE ) 
        {
            if ( !xmlStrcmp( node->name, (const xmlChar*)"Package" ) )
            {
                for ( xmlNode *packageNode = node->children; packageNode; packageNode = packageNode->next )
                {
                    if ( packageNode->type == XML_ELEMENT_NODE && 
                        !xmlStrcmp( packageNode->name, (const xmlChar*)"Identity" ) )
                    {
                        Name = xmlGetProp( packageNode, (const xmlChar *)"Name" );
                        Publisher = xmlGetProp( packageNode, (const xmlChar *)"Publisher" );
                        Version = xmlGetProp( packageNode, (const xmlChar *)"Version" );
                        ProcArchitecture = xmlGetProp( packageNode, (const xmlChar *)"ProcessorArchitecture" );
                        break;
                    }
                }
            }
        }
    }

    Identity->Name = charToWChar( Name );
    Identity->Publisher = charToWChar( Publisher );

    if ( !xmlStrcmp( ProcArchitecture, (const xmlChar*)"x86" ) )
        Identity->Architecture = PROCESSOR_X86;
    else if ( !xmlStrcmp( ProcArchitecture, (const xmlChar*)"x64" ) )
        Identity->Architecture = PROCESSOR_X64;
    else if ( !xmlStrcmp( ProcArchitecture, (const xmlChar*)"arm64" ) )
        Identity->Architecture = PROCESSOR_ARM64;
    else 
    {
        errorMessage( "Element \"Package->Identity.ProcessorArchitecture\" has an incorrect architecture.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }

    if ( !OK( ParseAppVersion( Version, &Parsed_Version ) ) )
    {
        return STATUS_FAIL;
    } 

    Identity->Version = Parsed_Version;

    return STATUS_SUCCESS;
}

static appxstatus
assignAppxProperties( xmlNode * manifest, struct appx_properties *Properties )
{
    const xmlChar* DisplayName;
    const xmlChar* PublisherDisplayName;
    const xmlChar* Logo;

    for ( xmlNode *node = manifest; node; node = node->next ) 
    {
        if ( node->type == XML_ELEMENT_NODE ) 
        {
            if ( !xmlStrcmp( node->name, (const xmlChar*)"Package" ) )
            {
                for ( xmlNode *packageNode = node->children; packageNode; packageNode = packageNode->next )
                {
                    if ( packageNode->type == XML_ELEMENT_NODE && 
                        !xmlStrcmp( packageNode->name, (const xmlChar*)"Properties" ) )
                    {
                        for ( xmlNode *propertiesNode = packageNode->children; propertiesNode; propertiesNode = propertiesNode->next )
                        {
                            if ( propertiesNode->type == XML_ELEMENT_NODE )
                            {
                                if ( !xmlStrcmp( propertiesNode->name, (const xmlChar*)"DisplayName" ) )
                                {
                                    DisplayName = xmlNodeGetContent(propertiesNode);
                                }
                                if ( !xmlStrcmp( propertiesNode->name, (const xmlChar*)"PublisherDisplayName" ) )
                                {
                                    PublisherDisplayName = xmlNodeGetContent(propertiesNode);
                                }
                                if ( !xmlStrcmp( propertiesNode->name, (const xmlChar*)"Logo" ) )
                                {
                                    Logo = xmlNodeGetContent(propertiesNode);
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    Properties->DisplayName = charToWChar( DisplayName );
    Properties->PublisherDisplayName = charToWChar( PublisherDisplayName );
    Properties->Logo = charToWChar( Logo );

    return STATUS_SUCCESS;
}

static appxstatus
assignAppxCapabilities( xmlNode * manifest, appxCapabilityList *List )
{
    for ( xmlNode *node = manifest; node; node = node->next ) 
    {
        if ( node->type == XML_ELEMENT_NODE ) 
        {
            if ( !xmlStrcmp( node->name, (const xmlChar*)"Package" ) )
            {
                for ( xmlNode *packageNode = node->children; packageNode; packageNode = packageNode->next )
                {
                    if ( packageNode->type == XML_ELEMENT_NODE && 
                        !xmlStrcmp( packageNode->name, (const xmlChar*)"Capabilities" ) )
                    {
                        *List = (appxCapabilityList)packageNode->children;
                    }
                }
            }
        }
    }

    return STATUS_SUCCESS;
}

static appxstatus
assignAppxResources( xmlNode * manifest, appxResourceList *List )
{
    for ( xmlNode *node = manifest; node; node = node->next ) 
    {
        if ( node->type == XML_ELEMENT_NODE ) 
        {
            if ( !xmlStrcmp( node->name, (const xmlChar*)"Package" ) )
            {
                for ( xmlNode *packageNode = node->children; packageNode; packageNode = packageNode->next )
                {
                    if ( packageNode->type == XML_ELEMENT_NODE && 
                        !xmlStrcmp( packageNode->name, (const xmlChar*)"Resources" ) )
                    {
                        *List = (appxCapabilityList)packageNode;
                    }
                }
            }
        }
    }

    return STATUS_SUCCESS;
}

static appxstatus
assignAppxApplication( xmlNode * manifest, appxResourceList *List )
{
    for ( xmlNode *node = manifest; node; node = node->next ) 
    {
        if ( node->type == XML_ELEMENT_NODE ) 
        {
            if ( !xmlStrcmp( node->name, (const xmlChar*)"Package" ) )
            {
                for ( xmlNode *packageNode = node->children; packageNode; packageNode = packageNode->next )
                {
                    if ( packageNode->type == XML_ELEMENT_NODE && 
                        !xmlStrcmp( packageNode->name, (const xmlChar*)"Resources" ) )
                    {
                        *List = (appxCapabilityList)packageNode;
                    }
                }
            }
        }
    }

    return STATUS_SUCCESS;
}