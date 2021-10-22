##---------------------------------------------------------------------------
## Author:      Rasbats aka. Mike Rossiter
## Copyright:
## License:     wxWidgets License
##---------------------------------------------------------------------------


# -------- Cmake setup ---------
#
cmake_minimum_required(VERSION 3.12.0)

cmake_policy(SET CMP0042 NEW)

if (POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif ()

if (POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif ()

# Locations where cmake looks for cmake modules.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/build ${CMAKE_SOURCE_DIR}/cmake)

if (WIN32)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/buildwin)
endif ()
message(STATUS "Cmake version: ${CMAKE_VERSION}.")


# -------- Build setup (mostly in PluginCompiler) --------
#
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
message(STATUS "Building: ${BUILD_TYPE}")

# Prefer libGL.so to libOpenGL.so, see CMP0072
set(OpenGL_GL_PREFERENCE "LEGACY")
option(USE_GL "Enable OpenGL support" ON)
message(STATUS "USE_GL: ${USE_GL}")

# -------- Options ----------


set(OCPN_TEST_REPO
    "mike-rossiter/oplaydo2-alpha"
    CACHE STRING "Default repository for untagged builds"
)
set(OCPN_BETA_REPO
    "mike-rossiter/oplaydo2-beta"
    CACHE STRING
    "Default repository for tagged builds matching 'beta'"
)
set(OCPN_RELEASE_REPO
    "mike-rossiter/oplaydo2-prod"
    CACHE STRING
    "Default repository for tagged builds not matching 'beta'"
)
option(oplaydo2_USE_SVG "Use SVG graphics" ON)

include(PluginOptions)
#
#
# -------  Plugin setup --------
#
project(oplaydo2_pi VERSION 0.0.0.1)
set(PKG_PRERELEASE "")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME oplaydo2)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME oplaydo2) # As of GetCommonName() in plugin API
set(CPACK_PACKAGE_CONTACT "Mike Rossiter")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Simple plugin")
set(CPACK_PACKAGE_DESCRIPTION [=[
Simple plugin to test Android builds
]=])

set(PKG_AUTHOR "Mike Rossiter")
set(PKG_IS_OPEN_SOURCE "yes")
set(CPACK_PACKAGE_HOMEPAGE_URL https://github.com/Rasbats/oplaydo2_pi)
set(PKG_INFO_URL https://opencpn.org/OpenCPN/plugins/oplaydo2.html)

set (SRC
  src/oplaydo2_pi.h
  src/oplaydo2_pi.cpp
  src/icons.h
  src/icons.cpp
  src/oplaydo2gui.h
  src/oplaydo2gui.cpp
  src/oplaydo2gui_impl.cpp
  src/oplaydo2gui_impl.h
  src/NavFunc.h
  src/NavFunc.cpp
  src/pi_TexFont.cpp
  src/pi_TexFont.h
  src/pi_ocpndc.cpp
  src/pi_ocpndc.h
)

if (QT_ANDROID)
  list(APPEND SRC src/pi_shaders.h src/pi_shaders.cpp)
endif ()

# -------- Setup completed, build the plugin --------
#
add_library(${CMAKE_PROJECT_NAME} SHARED ${SRC})
include_directories(BEFORE ${CMAKE_BINARY_DIR}/include)
include_directories(BEFORE ${PROJECT_SOURCE_DIR}/libs)

add_subdirectory("libs/api-16")
target_link_libraries(${CMAKE_PROJECT_NAME} ocpn::api)

set(VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(VERSION_PATCH ${PROJECT_VERSION_PATCH})

set(PACKAGE_NAME ${CMAKE_PROJECT_NAME})

# Create the minimim ocpn version id string like ov50
string(REGEX REPLACE "\([0-9]\)\.\([0-9]\).*" "ov\\1\\2"
       OCPN_MIN_VERSION ${API_OCPN_MIN_VERSION})

if (oplaydo2_USE_SVG)
  add_definitions(-Doplaydo2_USE_SVG)
endif ()

add_definitions(-DocpnUSE_GL)

# Set up targets. Targets sets up a recursive call with BUILD_TYPE set to
# 'flatpak', 'pkg' or 'tarball'. The initial call without BUILD_TYPE ends
# here.
#
include(Targets)
create_targets(
  ${PROJECT_SOURCE_DIR}/flatpak/org.opencpn.OpenCPN.Plugin.oplaydo2.yaml
)
if ("${BUILD_TYPE}" STREQUAL "")
  return ()
endif ()

if (NOT ${BUILD_TYPE} STREQUAL "flatpak")
  # Build package as required (flatpak already dealt with).
  #
  include(PluginCompiler)
  include(PluginInstall)
  include(PluginPackage)
  if (QT_ANDROID)
    include(AndroidLibs)
  else ()
    include(PluginLibs)
  endif ()
  include(PluginLocalization)

  include_directories(BEFORE ${CMAKE_BINARY_DIR}/include)

  if (WIN32)
    add_subdirectory("libs/WindowsHeaders")
    target_link_libraries(${PACKAGE_NAME} windows::headers)
  endif ()

  add_subdirectory("libs/jsoncpp")
  target_link_libraries(${PACKAGE_NAME} ocpn::jsoncpp)

  add_subdirectory("libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)

  add_subdirectory("libs/wxJSON")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)

endif ()


configure_file(
  ${CMAKE_SOURCE_DIR}/config.h.in ${CMAKE_BINARY_DIR}/include//config.h
  @ONLY
)

configure_file(
  # The cloudsmith upload script
  ${CMAKE_SOURCE_DIR}/ci/upload.sh.in ${CMAKE_BINARY_DIR}/upload.sh
  @ONLY
)
configure_file(
  # The cloudsmith upload script, windows bat file.
  ${CMAKE_SOURCE_DIR}/ci/upload.bat.in ${CMAKE_BINARY_DIR}/upload.bat
  @ONLY
)


set(checksum "@checksum@")
configure_file(
  # The XML metadata file
  ${CMAKE_SOURCE_DIR}/plugin.xml.in
  ${CMAKE_BINARY_DIR}/${pkg_displayname}.xml.in
  @ONLY
)
