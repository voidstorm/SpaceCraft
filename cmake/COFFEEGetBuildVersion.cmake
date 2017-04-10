get_filename_component (COFFEE_GET_PYTHON_MODULE_FILE "${CMAKE_CURRENT_LIST_DIR}/version_parser.py" ABSOLUTE)
get_filename_component (COFFEE_GAME_VERSION_H_FILE_IN "${CMAKE_CURRENT_LIST_DIR}/version_game.h.in" ABSOLUTE)
get_filename_component (COFFEE_VERSION_H_FILE_IN      "${CMAKE_CURRENT_LIST_DIR}/version_coffee.h.in" ABSOLUTE)



function(COFFEE_GetBuildVersion BUILD_INI IS_COFFEE VERSION_OUT)
  find_package(PythonInterp)

  get_filename_component (BUILD_INI "${BUILD_INI}" ABSOLUTE)
  file (TO_NATIVE_PATH "${COFFEE_GET_PYTHON_MODULE_FILE}" MODULE_FILE)
  file (TO_NATIVE_PATH "${BUILD_INI}" BUILD_INI)

  # message(STATUS "executing ${PYTHON_EXECUTABLE} ${MODULE_FILE} ${BUILD_INI}")

  execute_process(
    COMMAND ${PYTHON_EXECUTABLE} ${MODULE_FILE} ${BUILD_INI} ${IS_COFFEE}
    RESULT_VARIABLE RESULT
    OUTPUT_VARIABLE VERSION
  )

  if (NOT RESULT STREQUAL "0")
    message(FATAL_ERROR "Error executing version parser error = ${RESULT} error = ${VERSION}")
  endif()

  set(${VERSION_OUT} ${VERSION} PARENT_SCOPE)
endfunction()



function(COFFEE_GenerateCoffeeVersionH BUILD_INI)

  COFFEE_GetBuildVersion("${BUILD_INI}" 1 COFFEE_VERSION)
  message(STATUS "COFFEE version ${COFFEE_VERSION}")

  configure_file(${COFFEE_VERSION_H_FILE_IN} ${CMAKE_CURRENT_BINARY_DIR}/coffee_version.h)

  set(COFFEE_VERSION ${COFFEE_VERSION} PARENT_SCOPE)

  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  # Check coffee version if environment is set
  set (COFFEE_RELEASE_VERSION $ENV{COFFEE_RELEASE_VERSION} PARENT_SCOPE)
  if (COFFEE_RELEASE_VERSION)
    if (NOT COFFEE_RELEASE_VERSION STREQUAL COFFEE_VERSION)
      message(FATAL_ERROR "COFFEE_RELEASE_VERSION environment '$ENV{COFFEE_RELEASE_VERSION}' does not match COFFEE_VERSION '${COFFEE_VERSION}' from build.ini")
    endif()
  endif()

endfunction()




function(COFFEE_GenerateGameVersionH GAME_SHORT_NAME GAME_LONG_NAME BUILD_INI)

  COFFEE_GetBuildVersion("${BUILD_INI}" 0 GAME_VERSION)
  message(STATUS "GAME: ${GAME_NAME}(${GAME_LONG_NAME})")
  message(STATUS "GAME version ${GAME_VERSION}")

  configure_file(${COFFEE_GAME_VERSION_H_FILE_IN} ${CMAKE_CURRENT_BINARY_DIR}/game_version.h)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})

  set(GAME_SHORT_NAME ${GAME_SHORT_NAME} PARENT_SCOPE)
  set(GAME_LONG_NAME  ${GAME_LONG_NAME} PARENT_SCOPE)
  set(GAME_VERSION    ${GAME_VERSION} PARENT_SCOPE)

endfunction()