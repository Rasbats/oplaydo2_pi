#
# Find and link general libraries to use: gettext, wxWidgets and OpenGL
#

find_package(Gettext REQUIRED)

set(wxWidgets_USE_DEBUG OFF)
set(wxWidgets_USE_UNICODE ON)
set(wxWidgets_USE_UNIVERSAL OFF)
set(wxWidgets_USE_STATIC OFF)

set(wxWidgets_USE_LIBS base core net xml html adv stc)
set(BUILD_SHARED_LIBS TRUE)

set(_bad_win_env_msg [=[
%WXWIN% is not present in environment, win_deps.bat has not been run.
Build might work, but most likely fail when not finding wxWidgets.
Run buildwin\win_deps.bat or set %WXWIN% to mute this message.
]=])

if (WIN32 AND NOT DEFINED ENV{WXWIN})
  message(WARNING ${_bad_win_env_msg})
endif ()

find_package(wxWidgets REQUIRED base core net xml html adv stc aui)
if (MSYS)
  # This is just a hack. I think the bug is in FindwxWidgets.cmake
  string(
    REGEX REPLACE "/usr/local" "\\\\;C:/MinGW/msys/1.0/usr/local"
    wxWidgets_INCLUDE_DIRS ${wxWidgets_INCLUDE_DIRS}
  )
endif ()
include(${wxWidgets_USE_FILE})
target_link_libraries(${PACKAGE_NAME} ${wxWidgets_LIBRARIES})
