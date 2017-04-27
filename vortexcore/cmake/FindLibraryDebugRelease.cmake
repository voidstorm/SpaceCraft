
macro(find_library_debug_release VAR_NAME LIBNAMES)

    unset(_LIBNAMES_DEBUG)
    unset(_LIBNAMES_RELEASE)
    foreach(L ${LIBNAMES})
      list(APPEND _LIBNAMES_DEBUG Debug/${L}.lib)
      list(APPEND _LIBNAMES_RELEASE Release/${L}.lib)
    endforeach()

    # on windows search for Debug and Release
    message(STATUS "Searching for ${_LIBNAMES_DEBUG}")
    find_library(${VAR_NAME}_DEBUG NAMES ${_LIBNAMES_DEBUG})
    message(STATUS "Searching for ${_LIBNAMES_RELEASE}")
    find_library(${VAR_NAME}_RELEASE NAMES ${_LIBNAMES_RELEASE})

    if(${VAR_NAME}_DEBUG AND ${VAR_NAME}_RELEASE)
      set(${VAR_NAME}
        debug ${${VAR_NAME}_DEBUG}
        optimized ${${VAR_NAME}_RELEASE})
    elseif (MLD_OVN_LIBRARY_RELEASE)
      set(${VAR_NAME} ${${VAR_NAME}_RELEASE})
    elseif(${VAR_NAME}_DEBUG)
      set(${VAR_NAME} ${${VAR_NAME}_DEBUG})
    else()
      message(STATUS "Searching for ${LIBNAMES}")
      find_library(${VAR_NAME} NAMES ${LIBNAMES})
    endif()

    unset(_LIBNAMES_DEBUG)
    unset(_LIBNAMES_RELEASE)

    if (NOT "${${VAR_NAME}}" STREQUAL "")
      message(STATUS "Found ${VAR_NAME}: ${${VAR_NAME}}")
    else()
      message(STATUS "Not Found ${VAR_NAME}")
    endif()
endmacro()
