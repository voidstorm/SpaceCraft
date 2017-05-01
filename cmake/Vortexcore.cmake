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

find_path(ASSIMP_INCLUDE_DIR glm/glm.hpp)

find_library_debug_release(ASSIMP_LIBRARY "assimp-vc140-mt")

if(ASSIMP_INCLUDE_DIR)
    set(ASSIMP_FOUND TRUE)
endif()

find_package_handle_standard_args(ASSIMP DEFAULT_MSG ASSIMP_INCLUDE_DIR)

mark_as_advanced(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)