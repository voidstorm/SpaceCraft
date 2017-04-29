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

find_path(GLM_INCLUDE_DIR glm/glm.hpp)
SET(GLM_LIBRARY "")

if(GLM_INCLUDE_DIR)
    set(GLM_FOUND TRUE)
endif()

find_package_handle_standard_args(GLM DEFAULT_MSG GLM_INCLUDE_DIR)

mark_as_advanced(GLM_INCLUDE_DIR GLM_LIBRARY)
