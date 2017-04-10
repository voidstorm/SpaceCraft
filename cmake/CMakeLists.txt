add_custom_target(cmake_infrastructure SOURCES
  CMakeLists.txt
  COFFEE3rdPartySettings.cmake
  COFFEEAscentSettings.cmake
  COFFEEGetBuildVersion.cmake
  COFFEEICC.cmake
  COFFEEInfrastructureSettings.cmake
  COFFEESettings.cmake
  COFFEESgfSettings.cmake
  COFFEEUtilities.cmake
  scripts/build.bat
  scripts/create.bat
  shared_libs_component_init.h.in
  version_coffee.h.in
  version_game.h.in
  version_parser.py
)

set_target_properties(cmake_infrastructure PROPERTIES FOLDER "cmake")