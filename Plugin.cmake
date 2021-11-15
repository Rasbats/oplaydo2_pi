# ~~~
# Summary:      Local, non-generic plugin setup
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.


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

#
#
# -------  Plugin setup --------
#
set(PKG_VERSION  0.2.0)
set(PKG_PRERELEASE "")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME oplaydo2)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME oplaydo2) # As of GetCommonName() in plugin API
set(PKG_SUMMARY "Simple Plugin")
set(PKG_DESCRIPTION [=[
Simple plugin to test Android builds
]=])

set(PKG_AUTHOR "Mike Rossiter")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE https://github.com/Rasbats/oplaydo2_pi)
set(PKG_INFO_URL https://mike-rossiter.org/OpenCPN/plugins/oplaydo2.html)

set(SRC
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
  src/piOverlayFactory.cpp
  src/piOverlayFactory.h
  src/gl_private.h
)

set(PKG_API_LIB api-16)  #  A directory in libs/ e. g., api-17 or api-16

macro(late_init)
  # Perform initialization after the PACKAGE_NAME library, compilers
  # and ocpn::api is available.
  if (oplaydo2_USE_SVG)
    target_compile_definitions(${PACKAGE_NAME} PUBLIC oplaydo2_USE_SVG)
  endif ()
  
  add_definitions(-DocpnUSE_GL)
endmacro ()

macro(add_plugin_libraries)
  
  # Add libraries required by this plugin
  add_subdirectory("libs/tinyxml")
  target_link_libraries(${PACKAGE_NAME} ocpn::tinyxml)

  add_subdirectory("libs/wxJSON")
  target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)

  add_subdirectory("libs/plugingl")
  target_link_libraries(${PACKAGE_NAME} ocpn::plugingl)
endmacro ()
