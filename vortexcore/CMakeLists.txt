cmake_minimum_required(VERSION 3.7)
set (CURRENT_MODULE_NAME  vortexcore)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


find_package(Vulkan)
find_package(GLM)

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


function(COFFEESetDefaultSourceGroups PUBLIC_HEADER_FILES PRIVATE_HEADER_FILES SOURCE_FILES)

  source_group("header\\public"          FILES ${PUBLIC_HEADER_FILES})
  source_group("header\\private"         FILES ${PRIVATE_HEADER_FILES})
  source_group("source"                  FILES ${SOURCE_FILES})

endfunction(COFFEESetDefaultSourceGroups)

include_directories (
  .
  ./include
  ${VULKAN_INCLUDE_DIR}
  #${IMGUI_INCLUDE_PATH}
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


#add_definitions(-DHAVE_IGT_MULTI_TOUCH_CLIENT)


set(ALL_SOURCE_FILES
  ${SOURCE_FILES}
  ${TEST_FILES}
)


#add_library(${CURRENT_MODULE_NAME} SHARED ${SOURCE_FILES})

COFFEESetDefaultSourceGroups("${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${SOURCE_FILES}")
COFFEEAddSharedLibrary(${CURRENT_MODULE_NAME} "presentation" GFX_DEVICE "${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${ALL_SOURCE_FILES}")


target_link_libraries (${CURRENT_MODULE_NAME}
  ${Vulkan_LIBRARY}  
)

if (ENABLE_DIRECTX)
  target_link_libraries (${CURRENT_MODULE_NAME} coffee_directx_dx)

  if(DirectX_D3D11_FOUND)
    target_link_libraries (${CURRENT_MODULE_NAME}
      ${DirectX_D3D11_LIBRARY}
      ${DirectX_DXGI_LIBRARY})
  endif()
endif()

