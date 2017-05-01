cmake_minimum_required(VERSION 3.7)
set (CURRENT_MODULE_NAME  vortexcore)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})


#directory settings



list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/glm")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/gli")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/imgui")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/assimp")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/assimp/include")
list(APPEND CMAKE_INCLUDE_PATH "../3rdparty/bullet3/src")


list(APPEND CMAKE_LIBRARY_PATH "../3rdparty/assimp/build_x64/code")
list(APPEND CMAKE_LIBRARY_PATH "../3rdparty/bullet3/build_cmake/lib")


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


find_package(Vulkan)
find_package(GLM)
find_package(GLI)
find_package(ImGui)
find_package(assimp)
find_package(BulletPhysics)

if (ENABLE_DIRECTX)
  find_package(DirectX)
endif()

if(DirectX_D3D11_FOUND)
  add_definitions("-DHAVE_DX11")
  include_directories(${DirectX_D3D11_INCLUDE_DIR})
endif()


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

include_directories (
  .
  ./include
  ${Vulkan_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIR}
  ${GLI_INCLUDE_DIR}
  ${IMGUI_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
  ${BULLET_PHYSICS_COMMON_INCLUDE_PATH}
)



set(PUBLIC_HEADER_FILES
  include/VortexCore/Api.h
  include/VortexCore/AppWindow.h
  include/VortexCore/CameraComponent.h
  include/VortexCore/CommandQueue.h
  include/VortexCore/Component.h
  include/VortexCore/Delegate.h
  include/VortexCore/Game.h
  include/VortexCore/ForwardDecls.h
  include/VortexCore/Material.h
  include/VortexCore/MeshComponent.h
  include/VortexCore/RenderContext.h
  include/VortexCore/Scene.h
  include/VortexCore/SceneGraph.h
  include/VortexCore/SceneManager.h  
  include/VortexCore/SceneObject.h
  include/VortexCore/ScopedTimer.h
  include/VortexCore/ScopedWaitTimer.h
  include/VortexCore/ThreadContext.h
  include/VortexCore/Transform.h
  include/VortexCore/Transform.h
  include/VortexCore/TransformCache.h
  include/VortexCore/VortexCore.h
)

set(PRIVATE_HEADER_FILES
  include/VortexCore/private/Camera.h
  include/VortexCore/private/Frustum.h
  include/VortexCore/private/Mesh.h
)

set(SOURCE_FILES
  src/AppWindow.cpp
  src/Camera.cpp
  src/CameraComponent.cpp
  src/Component.cpp
  src/dllmain.cpp
  src/Frustum.cpp
  src/Game.cpp
  src/Material.cpp
  src/Mesh.cpp
  src/MeshComponent.cpp
  src/RenderContext.cpp
  src/Scene.cpp
  src/SceneGraph.cpp
  src/SceneManager.cpp
  src/SceneObject.cpp
  src/TransformCache.cpp
  src/VortexCore.cpp
)

set(SOURCE_FILES_WIN32
  src/win32/OSAppWindow.cpp
)


if (WIN32)
  if (ENABLE_DIRECTX)
    set(SOURCE_FILES
    ${SOURCE_FILES}
    src/common/gfx_device_dx11.cpp)
  endif()
endif()


set(TEST_FILES

)


add_definitions(-D_UNICODE -DUNICODE )


set(ALL_SOURCE_FILES
  ${SOURCE_FILES}
  ${TEST_FILES}
  ${SOURCE_FILES_WIN32}
  ${IMGUI_LIBRARY}  
)


#add_library(${CURRENT_MODULE_NAME} SHARED ${SOURCE_FILES})

source_group("source\\imgui"           FILES ${IMGUI_LIBRARY})

COFFEESetDefaultSourceGroups("${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${SOURCE_FILES}" "${SOURCE_FILES_WIN32}")
COFFEEAddSharedLibrary(${CURRENT_MODULE_NAME} "presentation" VORTEXCORE "${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${ALL_SOURCE_FILES}")

set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/debug" )
set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/release" )

set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/bin/debug" )
set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/bin/release" )

set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/debug" )
set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/release" )

set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_LIST_DIR}/lib/debug" )
set_target_properties( ${CURRENT_MODULE_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/release" )

set(CMAKE_CFG_INTDIR "$<$<CONFIG:Debug>:${CMAKE_CURRENT_LIST_DIR}/obj/debug>"
                     "$<$<CONFIG:Release>:${CMAKE_CURRENT_LIST_DIR}/obj/release>"
)

target_link_libraries (${CURRENT_MODULE_NAME}
  ${Vulkan_LIBRARY}
  ${ASSIMP_LIBRARY}
  ${BULLET_PHYSICS_DYNAMICS_LIBRARY}
)

if (ENABLE_DIRECTX)
  target_link_libraries (${CURRENT_MODULE_NAME} coffee_directx_dx)

  if(DirectX_D3D11_FOUND)
    target_link_libraries (${CURRENT_MODULE_NAME}
      ${DirectX_D3D11_LIBRARY}
      ${DirectX_DXGI_LIBRARY})
  endif()
endif()

add_custom_command(TARGET ${CURRENT_MODULE_NAME} POST_BUILD COMMAND "${CMAKE_CURRENT_LIST_DIR}/update_3rdparty.bat" WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")

