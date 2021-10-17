# ---------------------------------------------------------------------------
# Frontend2 Author:      jongough aka. Jon Gough Copyright: License:     wxWidgets License
# ---------------------------------------------------------------------------
#-----------------------------------------------------------------------------
# Frontend2: v1.0.149.0    Frontend2 Author: Jon Gough  
# ---------------------------------------------------------------------------


## ----- When changing this file do NOT change the order in which sections occur        ----- ##
## ----- Changes should only be made between the section blocks that identify where     ----- ##
## ----- these changes should be. The whole configuration process relies on this        ----- ##
## ----- sequence to be successful                                                      ----- ##
##

##
## ----- Modify section below to include all the details for your plugin ----- ##
##

set(CMLOC "CMakeLists: ")

# define minimum cmake version
cmake_minimum_required(VERSION 3.1.1)
if(COMMAND cmake_policy)
    if(POLICY CMP0043)
        cmake_policy(SET CMP0043 NEW)
    endif(POLICY CMP0043)
    cmake_policy(SET CMP0048 NEW)
    if(POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
    endif(POLICY CMP0077)
endif(COMMAND cmake_policy)

# define plugin name, owner and versions
set(VERBOSE_NAME "oplaydo1")
set(COMMON_NAME "oplaydo1")
set(TITLE_NAME "oplaydo1")
set(PACKAGE_CONTACT "Mike Rossiter")
set(PACKAGE "oplaydo1")
set(SHORT_DESCRIPTION "Plugin to demonstrate frontend2 workflow")
set(LONG_DESCRIPTION "oplaydo1 Plugin is used to test the frontend2 managed plugin workflow")

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_PATCH "0")
set(VERSION_TWEAK "1")
set(VERSION_DATE "12/02/2021") # DD/MM/YYYY format
set(OCPN_MIN_VERSION "ov50")
set(OCPN_API_VERSION_MAJOR "1")
set(OCPN_API_VERSION_MINOR "16")
set(TP_COMMENT "  * Release for O5 using CI")

set(PARENT "opencpn")

# The next line allows for setting up your own local webserver with git on it for testing purposes, the default is github.com
#set(GIT_REPOSITORY_SERVER "github.com")

# Specifies cloudsmith repository sufficies
set(PROD "prod") # Standard Repos
set(BETA "beta") # Standard Repos
set(ALPHA "alpha") # Standard Repos

# set this if your cloudsmith repository name does not match your git repository name
#set(CLOUDSMITH_BASE_REPOSITORY "oplaydo1_pi")
set(CLOUDSMITH_BASE_REPOSITORY "oplaydo1")
# Set the following to use OpenCPN cloudsmith repositories instead of own repositories
set(CLOUDSMITH_USER "mike-rossiter")
#set(CLOUDSMITH_USER "mike-rossiter")

set(XML_INFO_URL "https://github.com/Rasbats/oplaydo1_pi/wiki")
set(XML_SUMMARY ${SHORT_DESCRIPTION})
set(XML_DESCRIPTION ${LONG_DESCRIPTION})

##
## ----- Modify section above to include all the details for your plugin ----- ##
##

##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

option(PLUGIN_USE_SVG "Use SVG graphics" ON)

set(CMAKE_CXX_STANDARD 11)

# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
# Don't use local version of GLU library
# uncommented the next line to avoid problems with PluginConfigure.cmake
#set(USE_LOCAL_GLU FALSE)					

option(USE_GL "Enable OpenGL support" ON)
message(STATUS "${CMLOC}USE_GL: ${USE_GL}")

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build" FORCE)
endif("${CMAKE_BUILD_TYPE}" STREQUAL "")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "" "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
message(STATUS "${CMLOC}Build type: ${CMAKE_BUILD_TYPE}")

##
## ----- The statements below are used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
##

project(${PACKAGE})
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;")

##=============================================================
include("PluginSetup")
##=============================================================
## ----- The above is used to setup standard variables that are required by the CMAKE process - do not remove ----- ##
## ----- Modify section below if there are special requirements for the plugin ----- ##
##

set(CMAKE_VERBOSE_MAKEFILE ON)

option(Plugin_CXX11 "Use c++11" OFF)

##
## ----- Modify section above if there are special requirements for the plugin ----- ##
## ----- do not change next section - needed to configure build process ----- ##
##

include("PluginConfigure")

##
## ----- do not change section above - needed to configure build process ----- ##
## ----- Change below to match project requirements for source, headers, etc. ----- ##

add_definitions(-DUSE_S57)

# From master branch - is this working? necessary?
IF(PLUGIN_USE_SVG)
    MESSAGE(STATUS "${CMLOC}SVG support enabled...")
    IF(NOT QT_ANDROID)
        ADD_DEFINITIONS(-DPLUGIN_USE_SVG)
    ENDIF(NOT QT_ANDROID)
ENDIF(PLUGIN_USE_SVG)

#======== Below from cmakelists.txt  climatology_pi
# ----Remember to Add your project Set(SRCS and set(HDRS  etc..

set(SRCS
		src/oplaydo1_pi.h
        src/oplaydo1_pi.cpp
        src/icons.h
        src/icons.cpp
	    src/oplaydo1gui.h
	    src/oplaydo1gui.cpp
	    src/oplaydo1gui_impl.cpp
	    src/oplaydo1gui_impl.h
		src/tinyxml.cpp
        src/tinyxmlerror.cpp
        src/tinyxmlparser.cpp
        src/tinyxml.h		
)
SET(LIBHDRS						
			include/GL/gl.h
			include/GL/gl_private.h
			include/GL/glext.h
			include/GL/glu.h
            api-16/ocpn_plugin.h			
)
##=============================================================
# Modify/configure 'Include' Directories below for the Plugin
##=============================================================

#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/plugingl)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/tinyxml)
#include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src/jsoncpp)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/include/GL)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/api-16)



##=============================================================
add_library(${PACKAGE_NAME} SHARED ${SRCS} ${LIBHDRS})
##=============================================================

add_definitions(-DTIXML_USE_STL)

##
## ----- Change above to match project requirements ----- ##
## ----- do not change next section - needed to configure build process ----- ##
##
##=============================================================
include("PluginInstall")
include("PluginLocalization")
include("PluginPackage")
##=============================================================
##
## ----- do not change section above - needed to configure build process ----- ##
##
