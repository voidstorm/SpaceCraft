cmake_minimum_required(VERSION 3.7)
set (CURRENT_MODULE_NAME  spacecraft)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../vortexcore/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")


include(AddDeps)

find_package(GLM)


include_directories (
  .
  ./include
  ../vortexcore/include
  ${GLM_INCLUDE_DIR}
)


set (SOURCE_FILES
  ./src/SpaceCraft.cpp
)

set (PRIVATE_HEADER_FILES

)

set (PUBLIC_HEADER_FILES

)

COFFEEAddExecutableWithOptions(${CURRENT_MODULE_NAME} "" "WIN32" "${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${SOURCE_FILES}")
COFFEESetDefaultSourceGroups("${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${SOURCE_FILES}")


target_link_libraries (${CURRENT_MODULE_NAME}
  vortexcore
)
