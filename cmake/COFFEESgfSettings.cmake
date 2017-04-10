
# setup GameSystem5 root directory
set(SGF_PLATFORM_FOUND FALSE)
if (COFFEE_IS_32BIT)
  set(PLATFORM_PROJECT_ROOT $ENV{PLATFORM_PROJECT_ROOT})
  if(PLATFORM_PROJECT_ROOT)
      get_filename_component(GS5_PLATFORM_ROOT_DIR "${PLATFORM_PROJECT_ROOT}" ABSOLUTE)
      set(GS5_INCLUDES ${GS5_PLATFORM_ROOT_DIR}/GameSystem_V5/GameSystemIncludes)
      set(SGF_PLATFORM_FOUND TRUE)
  endif()

  set(GAME_PROJECT_ROOT $ENV{GAME_PROJECT_ROOT})
  if(GAME_PROJECT_ROOT)
      get_filename_component(GS5_GAME_ROOT_DIR "${GAME_PROJECT_ROOT}" ABSOLUTE)
  else()
      set(SGF_PLATFORM_FOUND FALSE)
  endif()
endif()

if(NOT SGF_PLATFORM_FOUND)
    message(STATUS "SGF Platform not found")
endif()

function(COFFEELogicAddGs5CopyTarget CURRENT_MODULE_NAME GAME_NAME FOLDER_NAME)

  if (GS5_GAME_ROOT_DIR)
    get_filename_component(GS5_CONFIG_SRC_DIR ../gs5_config ABSOLUTE)
    get_filename_component(GS5_CONFIG_DST_DIR ${GS5_GAME_ROOT_DIR}/GTECH/${GAME_NAME} ABSOLUTE)

    COFFEEAddCopyDirectoryTarget(${CURRENT_MODULE_NAME}_gs5_copy_config "${FOLDER_NAME}" ${GS5_CONFIG_SRC_DIR} ${GS5_CONFIG_DST_DIR})

    add_dependencies(${CURRENT_MODULE_NAME} ${CURRENT_MODULE_NAME}_gs5_copy_config)
  endif()

endfunction(COFFEELogicAddGs5CopyTarget)
