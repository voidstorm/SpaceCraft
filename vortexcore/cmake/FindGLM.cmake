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

find_path(GLM_INCLUDE_DIR glm/glm.hpp)
#find_library_debug_release(GLFW_LIBRARY "glfw3;glfw")
SET(GLM_LIBRARY "")

find_package_handle_standard_args(GLFW DEFAULT_MSG GLM_INCLUDE_DIR GLM_LIBRARY)
mark_as_advanced(GLM_INCLUDE_DIR GLM_LIBRARY)
