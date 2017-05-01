#
#
# Try to find the GLFW libraries
# Once done this will define
#
# GLFW_FOUND          - system has GLFW
# GLFW_INCLUDE_DIR    - path to GLFW/GLFW.h
# GLFW_LIBRARY      - the library that must be included
#
#

include(FindPackageHandleStandardArgs)
include(FindLibraryDebugRelease)

find_path(VORTEXCORE_INCLUDE_DIR vortexcore/VortexCore.h)

find_library_debug_release(VORTEXCORE_LIBRARY "vortexcore")

if(VORTEXCORE_INCLUDE_DIR)
    set(VORTEXCORE_FOUND TRUE)
endif()

find_package_handle_standard_args(VORTEXCORE DEFAULT_MSG VORTEXCORE_INCLUDE_DIR)

mark_as_advanced(VORTEXCORE_INCLUDE_DIR VORTEXCORE_LIBRARY)