cmake_minimum_required(VERSION 2.8.3)

if (CMAKE_MAJOR_VERSION GREATER 2)
  cmake_policy(SET CMP0022 OLD)
endif()

get_filename_component(COFFEE_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/../..  ABSOLUTE)
get_filename_component(COFFEE_BASE_DIR ${CMAKE_CURRENT_LIST_DIR}/..     ABSOLUTE)

get_filename_component(COFFEE_BASE_PRES_DIR   ${COFFEE_BASE_DIR}/presentation ABSOLUTE)
get_filename_component(COFFEE_BASE_LOGIC_DIR  ${COFFEE_BASE_DIR}/logic ABSOLUTE)
get_filename_component(COFFEE_BASE_SERVER_DIR ${COFFEE_BASE_DIR}/server ABSOLUTE)

get_filename_component(COFFEE_SHARED_COMPONENTS_DIR               ${COFFEE_BASE_DIR}/shared_components          ABSOLUTE)
get_filename_component(COFFEE_SHARED_COMPONENTS_CMAKE_DIR         ${COFFEE_SHARED_COMPONENTS_DIR}/cmake         ABSOLUTE)
get_filename_component(COFFEE_SHARED_COMPONENTS_BASIC_DIR         ${COFFEE_SHARED_COMPONENTS_DIR}/basic         ABSOLUTE)
get_filename_component(COFFEE_SHARED_COMPONENTS_ADVANCED_DIR      ${COFFEE_SHARED_COMPONENTS_DIR}/advanced      ABSOLUTE)
get_filename_component(COFFEE_SHARED_COMPONENTS_COMMUNICATION_DIR ${COFFEE_SHARED_COMPONENTS_DIR}/communication ABSOLUTE)
get_filename_component(COFFEE_SHARED_COMPONENTS_MPT_DIR           ${COFFEE_SHARED_COMPONENTS_DIR}/mpt           ABSOLUTE)
get_filename_component(COFFEE_SHARED_COMPONENTS_LINKGAME_DIR      ${COFFEE_SHARED_COMPONENTS_DIR}/linkgame      ABSOLUTE)
get_filename_component(COFFEE_SHARED_COMPONENTS_EMULATION_DIR     ${COFFEE_SHARED_COMPONENTS_DIR}/emulation     ABSOLUTE)

# add search path to our own cmake files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/std-modules)

include (${COFFEE_SHARED_COMPONENTS_CMAKE_DIR}/COFFEEGlobalSettings.cmake)
include(COFFEE3rdPartySettings)
include(COFFEEGetBuildVersion)
include(COFFEEUtilities)
include(COFFEEICC)
include(COFFEEAscentSettings)
include(COFFEESgfSettings)

# setup coffee version
COFFEE_GenerateCoffeeVersionH("${COFFEE_BASE_DIR}/build.ini")

# enable unicode
set(COFFEE_DEFINES "${COFFEE_DEFINES} -DUNICODE -D_UNICODE")
# set flag for reading a 32 bit paytable in 64 bit environment
if (COFFEE_IS_64BIT)
    set(COFFEE_DEFINES "${COFFEE_DEFINES} -DLOAD_32_BIT_PAYTABLES_IN_64_BIT_INTERPRETERS")
endif()

set(COFFEE_BIN_DIRECTORY ${COFFEE_OS_DIRECTORY}/${COFFEE_CONFIG_DIR})

if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    get_filename_component(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin ABSOLUTE)
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    get_filename_component(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin ABSOLUTE)
endif()
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    get_filename_component(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin ABSOLUTE)
endif()
if(NOT CMAKE_PDB_OUTPUT_DIRECTORY)
    get_filename_component(CMAKE_PDB_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin ABSOLUTE)
endif()


get_filename_component(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/${COFFEE_OS_DIRECTORY} ABSOLUTE)
get_filename_component(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${COFFEE_OS_DIRECTORY} ABSOLUTE)
get_filename_component(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${COFFEE_OS_DIRECTORY} ABSOLUTE)
if(WIN32)
    if(CMAKE_GENERATOR MATCHES "Visual Studio[.]*")
        get_filename_component(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_PDB_OUTPUT_DIRECTORY}/${COFFEE_OS_DIRECTORY} ABSOLUTE)
    elseif(CMAKE_GENERATOR MATCHES "NMake[.]*")
        get_filename_component(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_PDB_OUTPUT_DIRECTORY}/${COFFEE_OS_DIRECTORY}/${COFFEE_CONFIG_DIR} ABSOLUTE)
    endif()
endif()

get_filename_component(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug ABSOLUTE)
get_filename_component(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug ABSOLUTE)
get_filename_component(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug ABSOLUTE)

get_filename_component(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release ABSOLUTE)
get_filename_component(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release ABSOLUTE)
get_filename_component(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release ABSOLUTE)

include(${COFFEE_SHARED_COMPONENTS_CMAKE_DIR}/COFFEESharedComponentSettings.cmake)

COFFEEFindBoostLib(filesystem)
COFFEEFindBoostLib(date_time)
COFFEEFindBoostLib(log)
COFFEEFindBoostLib(regex)
COFFEEFindBoostLib(serialization)
COFFEEFindBoostLib(system)
COFFEEFindBoostLib(thread)
COFFEEFindBoostLib(program_options)

function(COFFEECreateSharedLibComponentInit GROUP NAME LONG_NAME MAJOR MINOR BUILD)

    set(VERSION_MAJOR   ${MAJOR})
    set(VERSION_MINOR   ${MINOR})
    set(VERSION_BUILD   ${BUILD})
    set(COMPONENT_NAME  ${NAME})
    set(COMPONENT_GROUP ${GROUP})
    set(COMPONENT_NAME_LONG ${LONG_NAME})

    configure_file(
        ${COFFEE_BASE_DIR}/cmake/shared_libs_component_init.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/shared_lib_component_init.h)
    message(STATUS "Component ${COMPONENT_GROUP}.${COMPONENT_NAME}_${COFFEE_VERSION}")

    include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
    )

endfunction()

function(COFFEEAddPythonLibrary TARGET_NAME)
    if(NOT PYTHONLIBS_FOUND)
    find_package(PythonLibs)
    endif()

    #message("PYTHON_LIBRARIES = ${PYTHON_LIBRARIES}")
    #message("PYTHON_DEBUG_LIBRARIES = ${PYTHON_DEBUG_LIBRARIES}")
    set(NEXT_ONE_IS_DEBUG 0)
    set(NEXT_ONE_IS_OPTIMIZED 0)
    set(DEBUG_OR_OPTIMIZED_FOUND 0)
    foreach(_TEMP ${PYTHON_LIBRARIES})
    if(NEXT_ONE_IS_DEBUG EQUAL 1)
        target_link_libraries(${TARGET_NAME} debug ${_TEMP})
        #message("found python debug = ${_TEMP}")
        set(NEXT_ONE_IS_DEBUG 0)
        set(DEBUG_OR_OPTIMIZED_FOUND 1)
    endif()

    if(NEXT_ONE_IS_OPTIMIZED EQUAL 1)
        target_link_libraries(${TARGET_NAME} optimized ${_TEMP})
        #message("found python optimized = ${_TEMP}")
        set(NEXT_ONE_IS_OPTIMIZED 0)
        set(DEBUG_OR_OPTIMIZED_FOUND 1)
    endif()

    if(${_TEMP} MATCHES "(optimized)")
        SET(NEXT_ONE_IS_OPTIMIZED 1)
    elseif(${_TEMP} MATCHES "(debug)")
        SET(NEXT_ONE_IS_DEBUG 1)
    endif()
    endforeach(_TEMP ${PYTHON_LIBRARIES})
    #no explicit debug or optimized version found
    if(DEBUG_OR_OPTIMIZED_FOUND EQUAL 0)
        target_link_libraries(${TARGET_NAME} debug ${PYTHON_LIBRARIES})
        target_link_libraries(${TARGET_NAME} optimized ${PYTHON_LIBRARIES})
    endif()
endfunction()

COFFEESetupAscent()
