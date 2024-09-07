/**
 * WineCoreUAP Appx Parser.
 * 
 * Written by Weather
 * 
 * This module is for parsing appx manifests.
 */

#include "include/read.h"

static appxstatus verifyPackage( xmlNode* manifestRoot );
static appxstatus verifyPackageContent( xmlNode* manifestRoot );
static appxstatus verifyPackageIdentity( xmlNode* manifestRoot );
static appxstatus verifyPackageProperties( xmlNode* manifestRoot );

/**
 * readManifest
 */
appxstatus
readManifest( const char *manifestpath, xmlNode** node )
{
    initErrorSeed();

    xmlDoc *manifest = xmlReadFile( manifestpath, NULL, 0 );

    if ( !manifest )
    {
        errorMessage( "Failed to read manifest file %s\n", manifestpath );
        setLastErrorCode( 2 );
        return STATUS_FAIL;
    }

    *node = xmlDocGetRootElement( manifest );

    if ( !*node )
    {
        errorMessage( "Failed to find the root element in the manifest file %s\n", manifestpath );
        setLastErrorCode( 3 );
        xmlFreeDoc(manifest);
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}

/**
 * verifyManifestRoot
 */
appxstatus
verifyManifestRoot( xmlNode *manifestRoot )
{
    if ( !OK( verifyPackage( manifestRoot ) ) )
        return STATUS_FAIL;
    if ( !OK( verifyPackageContent( manifestRoot ) ) )
        return STATUS_FAIL;
    if ( !OK( verifyPackageIdentity( manifestRoot ) ) )
        return STATUS_FAIL;
    if ( !OK( verifyPackageProperties( manifestRoot ) ) )
        return STATUS_FAIL;

    return STATUS_SUCCESS;
}

static appxstatus
verifyPackage( xmlNode* manifestRoot )
{
    bool packageExists = false;

    for ( xmlNode *node = manifestRoot; node; node = node->next ) 
    {
        if ( node->type == XML_ELEMENT_NODE ) 
        {
            if ( !xmlStrcmp( node->name, (const xmlChar*)"Package" ) )
                packageExists = true;
            else
            {
                errorMessage( "Wild element %s\n", (char *)node->name );
                setLastErrorCode( 1 );
                return STATUS_FAIL;
            }
        }
    }

    if ( !packageExists )
    {
        errorMessage( "Element \"Package\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}

static appxstatus
verifyPackageContent( xmlNode* manifestRoot )
{
    bool propertiesExists = false;
    bool dependenciesExists = false;
    bool resourcesExists = false;
    bool applicationsExists = false;
    bool capabilitiesExists = false;
    bool identityExists = false;

    for ( xmlNode *node = manifestRoot; node; node = node->next ) 
    {
        if ( node->type == XML_ELEMENT_NODE ) 
        {
            if ( !xmlStrcmp( node->name, (const xmlChar*)"Package" ) )
            {
                for ( xmlNode *packageNode = node->children; packageNode; packageNode = packageNode->next )
                {
                    if ( packageNode->type == XML_ELEMENT_NODE )
                    {
                        if (!propertiesExists) propertiesExists = !xmlStrcmp( packageNode->name, (const xmlChar*)"Properties" );
                        if (!dependenciesExists) dependenciesExists = !xmlStrcmp( packageNode->name, (const xmlChar*)"Dependencies" );
                        if (!resourcesExists) resourcesExists = !xmlStrcmp( packageNode->name, (const xmlChar*)"Resources" );
                        if (!applicationsExists) applicationsExists = !xmlStrcmp( packageNode->name, (const xmlChar*)"Applications" );
                        if (!capabilitiesExists) capabilitiesExists = !xmlStrcmp( packageNode->name, (const xmlChar*)"Capabilities" );
                        if (!identityExists) identityExists = !xmlStrcmp( packageNode->name, (const xmlChar*)"Identity" );
                    }
                }
            }
        }
    }

    if ( !propertiesExists )
    {
        errorMessage( "Element \"Package->Properties\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !dependenciesExists )
    {
        errorMessage( "Element \"Package->Dependencies\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !resourcesExists )
    {
        errorMessage( "Element \"Package->Resources\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !applicationsExists )
    {
        errorMessage( "Element \"Package->Applications\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !capabilitiesExists )
    {
        errorMessage( "Element \"Package->Capabilities\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !identityExists )
    {
        errorMessage( "Element \"Package->Identity\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}

static appxstatus
verifyPackageIdentity( xmlNode* manifestRoot )
{
    bool nameExists = false;
    bool publisherExists = false;
    bool versionExists = false;
    bool processorArchitectureExists = false;

    for ( xmlNode *node = manifestRoot; node; node = node->next ) 
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
                        if ( xmlGetProp( packageNode, (const xmlChar *)"Name" ) )
                            nameExists = true;
                        if ( xmlGetProp( packageNode, (const xmlChar *)"Publisher" ) )
                            publisherExists = true;
                        if ( xmlGetProp( packageNode, (const xmlChar *)"Version" ) )
                            versionExists = true;
                        if ( xmlGetProp( packageNode, (const xmlChar *)"ProcessorArchitecture" ) )
                            processorArchitectureExists = true;
                        break;
                    }
                }
            }
        }
    }

    if ( !nameExists )
    {
        errorMessage( "Element \"Package->Identity.Name\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !publisherExists )
    {
        errorMessage( "Element \"Package->Identity.Publisher\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !versionExists )
    {
        errorMessage( "Element \"Package->Identity.Version\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !processorArchitectureExists )
    {
        errorMessage( "Element \"Package->Identity.ProcessorArchitecture\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    return STATUS_SUCCESS;
}

static appxstatus
verifyPackageProperties( xmlNode* manifestRoot )
{
    bool displayNameExists = false;
    bool publisherDisplayNameExists = false;
    bool logoExists = false;

    for ( xmlNode *node = manifestRoot; node; node = node->next ) 
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
                                if (!displayNameExists) displayNameExists = !xmlStrcmp( propertiesNode->name, (const xmlChar*)"DisplayName" );
                                if (!publisherDisplayNameExists) publisherDisplayNameExists = !xmlStrcmp( propertiesNode->name, (const xmlChar*)"PublisherDisplayName" );
                                if (!logoExists) logoExists = !xmlStrcmp( propertiesNode->name, (const xmlChar*)"Logo" );
                            }
                        }
                        break;
                    }
                }
            }
        }
    }

    if ( !displayNameExists )
    {
        errorMessage( "Element \"Package->Properties.DisplayName\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !publisherDisplayNameExists )
    {
        errorMessage( "Element \"Package->Properties.PublisherDisplayName\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }
    if ( !logoExists )
    {
        errorMessage( "Element \"Package->Properties.Logo\" doesn't exist.\n" );
        setLastErrorCode( 1 );
        return STATUS_FAIL;
    }

    return STATUS_SUCCESS;
}