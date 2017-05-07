list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/glm")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/gli")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/imgui")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/assimp")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/assimp/include")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/bullet3/src")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/steamaudio/include")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/protobuf/include/google")


list(APPEND CMAKE_LIBRARY_PATH "../3rdparty/assimp/build_x64/code")
list(APPEND CMAKE_LIBRARY_PATH "../3rdparty/bullet3/build_cmake/lib")
list(APPEND CMAKE_LIBRARY_PATH "../3rdparty/steamaudio/lib/Windows/x64")
list(APPEND CMAKE_LIBRARY_PATH "../3rdparty/protobuf/lib")


function(COFFEEAddSharedLibrary TARGET_NAME MODULE_FOLDER MODULE_DEFINE PUBLIC_HEADER_FILES PRIVATE_HEADER_FILES SOURCE_FILES)

    set (FILES
      ${PUBLIC_HEADER_FILES}
      ${SOURCE_FILES}
      ${PRIVATE_HEADER_FILES}
    )

    add_library (${TARGET_NAME} SHARED ${FILES})

    if (MODULE_FOLDER)
      set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "${MODULE_FOLDER}")
    endif()
    set_target_properties(${TARGET_NAME} PROPERTIES COMPILE_DEFINITIONS "${MODULE_DEFINE}_EXPORTS")
    set_target_properties(${TARGET_NAME} PROPERTIES LINK_INTERFACE_LIBRARIES "")

endfunction(COFFEEAddSharedLibrary)


function(COFFEESetDefaultSourceGroups PUBLIC_HEADER_FILES PRIVATE_HEADER_FILES SOURCE_FILES SOURCE_FILES_WIN32)

  source_group("header\\public"          FILES ${PUBLIC_HEADER_FILES})
  source_group("header\\private"         FILES ${PRIVATE_HEADER_FILES})
  source_group("source"                  FILES ${SOURCE_FILES})
  source_group("source\\win32"           FILES ${SOURCE_FILES_WIN32})
  

endfunction(COFFEESetDefaultSourceGroups)


function(COFFEEAddExecutableWithOptions TARGET_NAME MODULE_FOLDER EXECUTABLE_OPTION PUBLIC_HEADER_FILES PRIVATE_HEADER_FILES SOURCE_FILES)

    add_executable (${TARGET_NAME}
        ${EXECUTABLE_OPTION}
        ${PUBLIC_HEADER_FILES}
        ${SOURCE_FILES}
        ${PRIVATE_HEADER_FILES}
    )

    if (MODULE_FOLDER)
      set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "${MODULE_FOLDER}")
    endif()

endfunction(COFFEEAddExecutableWithOptions)