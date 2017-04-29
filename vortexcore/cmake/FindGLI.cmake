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

find_path(GLI_INCLUDE_DIR gli/gli.hpp)
SET(GLI_LIBRARY "")

if(GLI_INCLUDE_DIR)
    set(GLI_FOUND TRUE)
endif()

find_package_handle_standard_args(GLI DEFAULT_MSG GLI_INCLUDE_DIR)

mark_as_advanced(GLI_INCLUDE_DIR GLI_LIBRARY)
