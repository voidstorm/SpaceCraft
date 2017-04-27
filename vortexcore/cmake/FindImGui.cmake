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

find_path(IMGUI_INCLUDE_DIR imgui.h)
SET(IMGUI_LIBRARY "${IMGUI_INCLUDE_DIR}/imgui.cpp")
list(APPEND IMGUI_LIBRARY "${IMGUI_INCLUDE_DIR}/imgui_draw.cpp")


if(IMGUI_INCLUDE_DIR)
    set(IMGUI_FOUND TRUE)
endif()

find_package_handle_standard_args(IMGUI DEFAULT_MSG IMGUI_INCLUDE_DIR IMGUI_LIBRARY)

mark_as_advanced(IMGUI_INCLUDE_DIR IMGUI_LIBRARY)
