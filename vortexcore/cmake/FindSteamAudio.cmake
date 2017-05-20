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

find_path(STEAMAUDIO_INCLUDE_DIR phonon.h)

find_library_debug_release(STEAMAUDIO_LIBRARY "phonon")

if(STEAMAUDIO_INCLUDE_DIR)
    set(STEAMAUDIO_FOUND TRUE)
endif()

find_package_handle_standard_args(STEAMAUDIO DEFAULT_MSG STEAMAUDIO_INCLUDE_DIR)

mark_as_advanced(STEAMAUDIO_INCLUDE_DIR STEAMAUDIO_LIBRARY)