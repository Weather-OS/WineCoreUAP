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
typedef xmlNode* extensionsList;
typedef xmlNode* uapObject;

/*
<Applications>
    <Application Id="App" Executable="$targetnametoken$.exe" EntryPoint="UWPApp.App" uap18:TrustLevel="appContainer" uap18:RuntimeBehavior="windowsApp">
      <uap:VisualElements DisplayName="UWPApp" Description="Project for a single page C++/WinRT Universal Windows Platform (UWP) app with no predefined layout"
        Square150x150Logo="Assets\Square150x150Logo.png" Square44x44Logo="Assets\Square44x44Logo.png" BackgroundColor="transparent">
        <uap:DefaultTile Wide310x150Logo="Assets\Wide310x150Logo.png">
        </uap:DefaultTile>
        <uap:SplashScreen Image="Assets\SplashScreen.png" />
        <uap:InitialRotationPreference>
          <uap:Rotation Preference="landscape"/></uap:InitialRotationPreference>
      </uap:VisualElements>
      <Extensions>
        <uap:Extension Category="windows.fileTypeAssociation">
          <uap:FileTypeAssociation Name="text">
            <uap:SupportedFileTypes>
              <uap:FileType ContentType="text/plain">.txt</uap:FileType>
            </uap:SupportedFileTypes>
            <uap:DisplayName>Text</uap:DisplayName>
            <uap:EditFlags AlwaysUnsafe="true" OpenIsSafe="true"/>
          </uap:FileTypeAssociation>
        </uap:Extension>
      </Extensions>
    </Application>
  </Applications>
*/
struct appx_properties {
    const wchar_t * DisplayName;
    const wchar_t * PublisherDisplayName;
    const wchar_t * Logo;
};

struct appx_application {
    const wchar_t * Id;
    const wchar_t * Executable;
    const wchar_t * EntryPoint;
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
