set (CURRENT_MODULE_NAME  coffee_gfx_device)

find_package(OpenGL)
find_package(GLFW)
find_package(Glew)
find_package(Boost)
find_package(json_spirit)
find_package(SeeFront)
find_package(SeeFront2)
find_package(ImGui)

if (COFFEE_ENABLE_DIRECTX)
  find_package(DirectX)
endif()

if (SEEFRONT_FOUND)
  add_definitions("-DHAVE_SEEFRONT")
  include_directories(${SEEFRONT_INCLUDE_PATH})
endif()

if (SEEFRONT2_FOUND)
  add_definitions("-DHAVE_SEEFRONT2")
  include_directories(${SEEFRONT2_INCLUDE_PATH})
endif()

if(DirectX_D3D11_FOUND)
  add_definitions("-DHAVE_DX11")
  include_directories(${DirectX_D3D11_INCLUDE_DIR})
endif()

include_directories (
  inc
  src/common
  ../bfl/inc
  ../mml/gfx/inc
  ../mml/res/inc
  ../mml/asset/inc
  ../gfx/inc
  ../opengl/ogl/inc
  ../opengl/ogl/src
  ../directx/inc
  ../gfx/inc
  ../registration_3d/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/dbg/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/math/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/telemetry/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/thread/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/utilities/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/kunit/inc
  ${OPENGL_INCLUDE_DIR}
  ${GLFW_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${Boost_INCLUDE_DIRS}
  ${ANT_TWEAK_BAR_INCLUDE_PATH}
  ${JSON_SPIRIT_INCLUDE_PATH}
  ${IMGUI_INCLUDE_PATH}
)

if(WIN32)
  include_directories(src/win32)
endif()

if(UNIX)
  find_package(X11)
  include_directories(
    src/linux
    ${X11_INCLUDE_DIR}
  )

  if( NOT X11_FOUND)
    message ( FATAL_ERROR "X11 not found!")
  endif( NOT X11_FOUND)
  if( NOT X11_Xi_FOUND)
    message ( FATAL_ERROR "X11_Xi not found!")
  endif( NOT X11_Xi_FOUND)

  message(STATUS "X11_X11_LIB == ${X11_X11_LIB}")
  message(STATUS "X11_Xtest_LIB == ${X11_Xtest_LIB}")
  message(STATUS "X11_Xi_LIB == ${X11_Xi_LIB}")

  message (STATUS "finding XInput2" )
  find_path( XINPUT2_INCLUDE_DIR X11/extensions/XInput2.h PATHS ${X11_INCLUDE_DIR} /usr/local/include /usr/include )
  if (XINPUT2_INCLUDE_DIR)
    message (STATUS "XInput2 Support found" )
    include_directories(
      ${XINPUT2_INCLUDE_DIR}
    )
    add_definitions(-DXInput2_Support_Enabled)
  else()
    message (STATUS "XInput2 Support not found" )
  endif()
endif()

set(PUBLIC_HEADER_FILES
  inc/gfx_device.h
  inc/gfx_device_api.h
  inc/gfx_device_event_dispatcher.h
  inc/gfx_device_handler.h
  inc/gfx_device_stereo_settings.h
  inc/gfx_device_layers.h
  inc/gfx_device_if_tracking_device_asd.h
  inc/gfx_device_if_tracking_data_asd.h
  inc/gfx_device_types.h
)

set(PRIVATE_HEADER_FILES
  src/common/gfx_device_base.h
  src/common/gfx_device_handler_common.h
  src/common/gfx_device_multitouch_config.h
  src/common/gfx_device_multitouch_config_parser.h
  src/common/gfx_device_maya.h
  src/common/gfx_device_opengl.h
  src/common/gfx_device_opengl_thread.h
  src/common/gfx_device_stereo_settings_imp.h
  src/common/gfx_device_layers_imp.h
  src/common/gfx_device_tracking_data_parser.h
  src/common/gfx_device_tracking_data_asd.h
  src/common/gfx_device_dx11.h
  src/common/gfx_device_app_window.h
  src/linux/gfx_device_opengl_thread_linux.h
  src/win32/gfx_device_opengl_thread_win32.h
  src/win32/gfx_device_win32_dll.h
  src/win32/gfx_device_os_app_window.h
)

set(SOURCE_FILES
  src/common/gfx_device_base.cpp
  src/common/gfx_device_multitouch_config.cpp
  src/common/gfx_device_multitouch_config_parser.cpp
  src/common/gfx_device_maya.cpp
  src/common/gfx_device_opengl.cpp
  src/common/gfx_device_opengl_thread.cpp
  src/common/gfx_device_dllmain.cpp
  src/common/gfx_device_handler_common.cpp
  src/common/gfx_device_stereo_settings_imp.cpp
  src/common/gfx_device_tracking_data_parser.cpp
  src/common/gfx_device_if_tracking_device_asd.cpp
  src/common/gfx_device_tracking_data_asd.cpp
)

set(SEEFRONT_SOURCE_FILES
  src/common/gfx_device_seefront_sgf.h
  src/common/gfx_device_seefront_v1.h
  src/common/gfx_device_seefront_base.h
  src/common/gfx_device_seefront_v2.h
  src/common/gfx_device_seefront_ascent.h
)

if (SEEFRONT_FOUND)
  set(SEEFRONT_SOURCE_FILES
    ${SEEFRONT_SOURCE_FILES}
    src/common/gfx_device_seefront_v1.cpp
  )
endif()

if (SEEFRONT2_FOUND)
  set(SEEFRONT_SOURCE_FILES
    ${SEEFRONT_SOURCE_FILES}
    src/common/gfx_device_seefront_v2.cpp
  )
endif()

if (WIN32)
  set(SOURCE_FILES
    ${SOURCE_FILES}
    src/common/gfx_device_app_window.cpp
    src/win32/gfx_device_os_app_window.cpp)

  if (COFFEE_ENABLE_DIRECTX)
    set(SOURCE_FILES
    ${SOURCE_FILES}
    src/common/gfx_device_dx11.cpp)
  endif()

endif()


set(TEST_FILES
  tests/entry_point.cpp
  tests/gfx_device_test_suite.h
  tests/gfx_device_test_json_parsing.cpp
  tests/gfx_device_test_json_parsing.h
  tests/gfx_device_test_suite.cpp
)

if (SGF_PLATFORM_FOUND)
  set(PRIVATE_HEADER_FILES
    ${PRIVATE_HEADER_FILES}
    ${GS5_INCLUDES}/gsmm/ASD.h
  )
  set(SEEFRONT_SOURCE_FILES
    ${SEEFRONT_SOURCE_FILES}
    src/common/gfx_device_seefront_sgf.cpp
  )
  include_directories(${GS5_INCLUDES})
  add_definitions("-DHAVE_TRACKING_DATA_ASD")
else()
  message(STATUS "Tracking Data ASD disabled")
endif()

if (WIN32)
  set(SOURCE_FILES
    ${SOURCE_FILES}
    src/win32/resources.rc
    src/win32/resources.h
    src/win32/gfx_device_handler_win32.cpp
    src/win32/gfx_device_opengl_thread_win32.cpp
    src/common/gfx_device_seefront_base.cpp
  )
elseif (UNIX)
  set(SOURCE_FILES
    ${SOURCE_FILES}
    src/linux/gfx_device_handler_linux.cpp
    src/linux/gfx_device_opengl_thread_linux.cpp
  )
endif()

if (ASCENT_BUILD_ENABLED)
  include_directories(
    src/win32
    src/win32/ascent
  )

  set(ASCENT_SOURCE_FILES
    src/win32/ascent/FourDNotificationClient.cpp
    src/win32/ascent/FourDNotificationClient.hpp
    src/win32/ascent/FourDNotificationListenerInterface.hpp
    src/win32/ascent/FourDNotificationSender.cpp
    src/win32/ascent/FourDNotificationSender.hpp
    src/win32/ascent/HeadDetectionClient.cpp
    src/win32/ascent/HeadDetectionClient.hpp
    src/win32/ascent/HeadDetectionCommonDefs.hpp
    src/win32/ascent/MultiTouchClient.cpp
    src/win32/ascent/MultiTouchClient.hpp
    src/win32/ascent/MultiTouchClientInterpreterCommonDefs.hpp
    src/win32/ascent/MultiTouchClientWindow.cpp
    src/win32/ascent/MultiTouchClientWindow.hpp
    src/win32/ascent/SetThreadName.cpp
  )

  set (SEEFRONT_SOURCE_FILES
      ${SEEFRONT_SOURCE_FILES}
      src/common/gfx_device_seefront_ascent.cpp
      )

  add_definitions(-DHAVE_IGT_MULTI_TOUCH_CLIENT)
  add_definitions(-DHAVE_IGT_ASD_4D)
  add_definitions(-DFOURD_CLIENT_DLL)
  add_definitions(-DMULTITOUCH_CLIENT_DLL)

  source_group("source\\ascent" FILES ${ASCENT_SOURCE_FILES})

endif()

source_group("source\\seefront" FILES ${SEEFRONT_SOURCE_FILES})

set(ALL_SOURCE_FILES
  ${SOURCE_FILES}
  ${ASCENT_SOURCE_FILES}
  ${SEEFRONT_SOURCE_FILES}
  ${TEST_FILES}
)

get_filename_component(JSON_TEST_FOLDER ${CMAKE_CURRENT_LIST_DIR}/tests ABSOLUTE)
COFFEE_AddCompileFlags(
  FILES tests/gfx_device_test_json_parsing.cpp
  FLAGS "-DJSON_TEST_FOLDER=\\\"${JSON_TEST_FOLDER}\\\"")


COFFEEAddLibrary(${CURRENT_MODULE_NAME} "presentation" GFX_DEVICE "${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${ALL_SOURCE_FILES}")
COFFEESetDefaultSourceGroupsTest("${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${SOURCE_FILES}" "${TEST_FILES}")

target_link_libraries (${CURRENT_MODULE_NAME}
  coffee_gfx
  coffee_opengl_ogl
  coffee_mml_asset
  coffee_mml_gfx
  coffee_bfl
  coffee_telemetry
  coffee_thread
  coffee_dbg
  coffee_utilities
  coffee_registration_3d
  ${GLEW_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${ANT_TWEAK_BAR_LIBRARY}
  ${JSON_SPIRIT_LIBRARY}
  ${GLFW_LIBRARY}
  ${GLEW_LIBRARY}
  ${IMGUI_LIBRARY}
)

if (COFFEE_ENABLE_DIRECTX)
  target_link_libraries (${CURRENT_MODULE_NAME} coffee_directx_dx)

  if(DirectX_D3D11_FOUND)
    target_link_libraries (${CURRENT_MODULE_NAME}
      ${DirectX_D3D11_LIBRARY}
      ${DirectX_DXGI_LIBRARY})
  endif()
endif()

if (SEEFRONT_FOUND)
  target_link_libraries (${CURRENT_MODULE_NAME} ${SEEFRONT_LIBRARY})
endif()

if (SEEFRONT2_FOUND)
  target_link_libraries (${CURRENT_MODULE_NAME} ${SEEFRONT2_LIBRARY})
endif()

if (UNIX)
  target_link_libraries (${CURRENT_MODULE_NAME}
    ${X11_XTest_LIB}
    ${X11_X11_LIB}
    ${X11_Xi_LIB}
  )
endif()

