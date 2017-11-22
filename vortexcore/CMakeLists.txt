cmake_minimum_required(VERSION 3.7)
set (CURRENT_MODULE_NAME  vortexcore)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
project(vortexcore)

#directory settings

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
#  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")

include(AddDeps)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GF /Zc:wchar_t /W3 /Wp64")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

find_package(Vulkan)
find_package(GLM)
find_package(GLI)
find_package(ImGui)
find_package(assimp)
find_package(BulletPhysics)
find_package(SteamAudio)
find_package(ProtoBuf)

if (ENABLE_DIRECTX)
  find_package(DirectX)
endif()

if(DirectX_D3D11_FOUND)
  add_definitions("-DHAVE_DX11")
  include_directories(${DirectX_D3D11_INCLUDE_DIR})
endif()


include_directories (
  .
  ./include
  ${Vulkan_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIR}
  ${GLI_INCLUDE_DIR}
  ${IMGUI_INCLUDE_DIR}
  ${ASSIMP_INCLUDE_DIR}
  ${BULLET_PHYSICS_COMMON_INCLUDE_PATH}
  ${PROTOBUF_INCLUDE_DIR}
)



set(PUBLIC_HEADER_FILES
  include/VortexCore/Api.h
  include/VortexCore/AlignedStorage.h
  include/VortexCore/AppWindow.h
  include/VortexCore/CameraComponent.h
  include/VortexCore/CommandQueue.h
  include/VortexCore/Component.h
  include/VortexCore/Delegate.h
  include/VortexCore/Game.h
  include/VortexCore/ForwardDecls.h
  include/VortexCore/Logger.h
  include/VortexCore/Material.h
  include/VortexCore/MeshComponent.h
  include/VortexCore/RenderContext.h
  include/VortexCore/Scene.h
  include/VortexCore/SceneGraph.h
  include/VortexCore/SceneManager.h  
  include/VortexCore/SceneObject.h
  include/VortexCore/ScopedTimer.h
  include/VortexCore/ScopedWaitTimer.h
  include/VortexCore/SystemLogger.h
  include/VortexCore/ThreadContext.h
  include/VortexCore/ThreadMap.h  
  include/VortexCore/Transform.h
  include/VortexCore/Transform.h
  include/VortexCore/TransformCache.h
  include/VortexCore/VortexCore.h
)

set(PRIVATE_HEADER_FILES
  include/VortexCore/private/Camera.h
  include/VortexCore/private/Frustum.h
  include/VortexCore/private/Mesh.h
  include/VortexCore/private/RenderContextVulkan.h
  include/VortexCore/private/ShaderVulkan.h
  include/VortexCore/private/SwapchainVulkan.h
  include/vortexcore/private/VkErrorHelper.h
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
  src/RenderContextVulkan.cpp
  src/ShaderVulkan.cpp
  src/Scene.cpp
  src/SceneGraph.cpp
  src/SceneManager.cpp
  src/SceneObject.cpp
  src/SwapchainVulkan.cpp
  src/TransformCache.cpp
  src/VortexCore.cpp
)

set(SOURCE_FILES_WIN32
  src/win32/OSAppWindow.cpp
)


if (WIN32)
   add_definitions("-DVK_USE_PLATFORM_WIN32_KHR")
endif()


set(TEST_FILES

)


set(SHADER_FILES_DEFAULT
 shader/default.comp
 shader/default.frag
 shader/default.geom
 shader/default.vert
)

 source_group("shader"           FILES ${SHADER_FILES_DEFAULT})


set(ALL_SOURCE_FILES
  ${SOURCE_FILES}
  ${TEST_FILES}
  ${SOURCE_FILES_WIN32}
  ${IMGUI_LIBRARY}
  ${SHADER_FILES_DEFAULT}
)

#SHADER



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

add_definitions(-D_UNICODE -DUNICODE )


target_link_libraries (${CURRENT_MODULE_NAME}
  ${Vulkan_LIBRARY}
  ${ASSIMP_LIBRARY}
  ${BULLET_PHYSICS_DYNAMICS_LIBRARY}
  ${PROTOBUF_LIBRARY}
  ${STEAMAUDIO_LIBRARY}  
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

