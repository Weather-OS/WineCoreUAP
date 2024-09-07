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
registerAppxPackage( const char * manifestPath, struct appx_package *package )
{
    appxstatus status;

    package->manifestPath = manifestPath;
    status = readManifest( manifestPath, &package->manifest );
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

    Identity->Name = Name;
    Identity->Publisher = Publisher;

    if ( xmlStrcmp( ProcArchitecture, (const xmlChar*)"x86" ) )
        Identity->Architecture = PROCESSOR_X86;
    else if ( xmlStrcmp( ProcArchitecture, (const xmlChar*)"x64" ) )
        Identity->Architecture = PROCESSOR_X64;
    else if ( xmlStrcmp( ProcArchitecture, (const xmlChar*)"arm64" ) )
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
                                    xmlNode *content = node->children;
                                    if (content && content->type == XML_TEXT_NODE) {
                                        DisplayName = content->content;
                                    }
                                }
                                if ( !xmlStrcmp( propertiesNode->name, (const xmlChar*)"PublisherDisplayName" ) )
                                {
                                    xmlNode *content = node->children;
                                    if (content && content->type == XML_TEXT_NODE) {
                                        PublisherDisplayName = content->content;
                                    }
                                }
                                if ( !xmlStrcmp( propertiesNode->name, (const xmlChar*)"Logo" ) )
                                {
                                    xmlNode *content = node->children;
                                    if (content && content->type == XML_TEXT_NODE) {
                                        Logo = content->content;
                                    }
                                }
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    Properties->DisplayName = DisplayName;
    Properties->PublisherDisplayName = PublisherDisplayName;
    Properties->Logo = Logo;

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
                        *List = (appxCapabilityList)packageNode;
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