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

find_path(PROTOBUF_INCLUDE_DIR glm/glm.hpp)

find_library_debug_release(PROTOBUF_LIBRARY "assimp-vc140-mt")

if(PROTOBUF_INCLUDE_DIR)
    set(PROTOBUF_FOUND TRUE)
endif()

find_package_handle_standard_args(PROTOBUF DEFAULT_MSG PROTOBUF_INCLUDE_DIR)

mark_as_advanced(PROTOBUF_INCLUDE_DIR PROTOBUF_LIBRARY)