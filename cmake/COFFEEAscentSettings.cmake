function(COFFEEAddCopyAscentTarget)

  if (ASCENT_PLATFORM_FOUND)
  endif()

endfunction(COFFEEAddCopyAscentTarget)

function(COFFEESetupAscent)
    if (WIN32)
      set (ASCENT_BUILD_ENABLED TRUE PARENT_SCOPE)
      message(STATUS "Building for ASCENT enabled")
    endif()

    set(ASCENT_ROOT $ENV{ASCENT_ROOT} )
    set(ASCENT_ROOT $ENV{ASCENT_ROOT} PARENT_SCOPE)
    if(ASCENT_ROOT)
        get_filename_component(ASCENT_SOURCE_ROOT "${ASCENT_ROOT}/projects" ABSOLUTE)

        set(ASCENT_SOURCE_ROOT ${ASCENT_SOURCE_ROOT} PARENT_SCOPE)
        set(ASCENT_BIN ${ASCENT_ROOT}/AVP/Shared PARENT_SCOPE)
        set(ASCENT_GAME_ROOT ${ASCENT_ROOT}/Games PARENT_SCOPE)
        set(ASCENT_PLATFORM_FOUND TRUE PARENT_SCOPE)
        message(STATUS "ASCENT_ROOT=${ASCENT_ROOT}")
    endif()
endfunction(COFFEESetupAscent)

function(COFFEELogicAddAscentCopyTarget CURRENT_MODULE_NAME GAME_NAME FOLDER_NAME)

  if(ASCENT_PLATFORM_FOUND)
    get_filename_component(ASCENT_CONFIG_SRC_DIR ../ascent_config ABSOLUTE)
    get_filename_component(ASCENT_CONFIG_DST_DIR ${ASCENT_GAME_ROOT}/${GAME_NAME} ABSOLUTE)

    COFFEEAddCopyDirectoryTarget(${CURRENT_MODULE_NAME}_ascent_copy_config "${FOLDER_NAME}" ${ASCENT_CONFIG_SRC_DIR} ${ASCENT_CONFIG_DST_DIR})

    add_dependencies(${CURRENT_MODULE_NAME} ${CURRENT_MODULE_NAME}_ascent_copy_config)
  endif()

endfunction(COFFEELogicAddAscentCopyTarget)
