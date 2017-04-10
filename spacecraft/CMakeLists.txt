set (CURRENT_MODULE_NAME  coffee_viewer)

find_package(Boost)
find_package(Glew)
find_package(Freetype)
find_package(FTGL)

include_directories (
  .
  ../../bfl/inc
  ../../mml/asset/inc
  ../../scene_graph/bmc/inc
  ../../scene_graph/mco/inc
  ../../scene_graph/mob/inc
  ../../scene_graph/mmi/inc
  ../../scene_graph/mgl/inc
  ../../scene_graph/inc
  ../../mml/gfx/inc
  ../../mml/res/inc
  ../../gfx/inc
  ../../opengl/ogl/inc
  ../../gfx_device/inc
  ../../depthsense_device/inc
  ../../registration_3d/inc
  ${COFFEE_SHARED_COMPONENTS_DIR}/mmi_interfaces/inc
  ${COFFEE_SHARED_COMPONENTS_COMMUNICATION_DIR}/mmi/inc
  ${COFFEE_SHARED_COMPONENTS_ADVANCED_DIR}/env/inc
  ${COFFEE_SHARED_COMPONENTS_ADVANCED_DIR}/dynobj/inc
  ${COFFEE_SHARED_COMPONENTS_ADVANCED_DIR}/xml/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/dbg/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/math/inc
  ${COFFEE_SHARED_COMPONENTS_BASIC_DIR}/utilities/inc
  ${Boost_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIR}
  ${GLEW_INCLUDE_DIR}
  ${FTGL_INCLUDE_DIR}
  ${FREETYPE_INCLUDE_DIR_ft2build}
  ${FREETYPE_INCLUDE_DIR_freetype2}
)

set (RC_SOURCE_FILES
    egd_viewer.rc
#    res/ico00001.ico
#    res/icon1.ico
#    res/idr_font.ico
#    res/imag.ico
#    res/mpng.ico
#    res/toolbar.bmp
#    res/viewer.ico
#    res/dizzy.ico
#    res/egd_viewer.rc2
)

set_source_files_properties(${RC_SOURCE_FILES} PROPERTIES LANGUAGE RC)


set (SOURCE_FILES
${RC_SOURCE_FILES}
egd_animation_doc.cpp
egd_animation_view.cpp
egd_dizzy_doc.cpp
egd_font_doc.cpp
egd_font_view.cpp
egd_image_doc.cpp
egd_image_view.cpp
egd_mpng_doc.cpp
egd_object_view.cpp
egd_png_doc.cpp
egd_viewer.cpp
egd_viewer_childframe.cpp
egd_viewer_mainframe.cpp
)

set (PRIVATE_HEADER_FILES
egd_object_view.h
egd_png_doc.h
egd_viewer.h
egd_viewer_childframe.h
egd_viewer_mainframe.h
resource.h
egd_viewer_afx.h
egd_animation_doc.h
egd_animation_view.h
egd_dizzy_doc.h
egd_font_doc.h
egd_font_view.h
egd_image_doc.h
egd_image_view.h
egd_mpng_doc.h
)

set (PUBLIC_HEADER_FILES
)

# -------------------------------------------------------------------------------
if (MSVC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0")
  add_definitions(-D_AFXDLL)
  set (CMAKE_MFC_FLAG 2)
endif()


COFFEEAddExecutableWithOptions(${CURRENT_MODULE_NAME} "presentation/tools" "WIN32" "${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${SOURCE_FILES}")
COFFEESetDefaultSourceGroups("${PUBLIC_HEADER_FILES}" "${PRIVATE_HEADER_FILES}" "${SOURCE_FILES}")

set_target_properties (${CURRENT_MODULE_NAME} PROPERTIES LINK_FLAGS "/ENTRY:\"wWinMainCRTStartup\"")

target_link_libraries (${CURRENT_MODULE_NAME}
  coffee_bfl
  coffee_mml_asset
  coffee_mml_gfx
  coffee_gfx
  coffee_mml_res
  coffee_opengl_ogl
  coffee_scene_graph_base
  coffee_scene_graph
  coffee_dbg
  coffee_env
  coffee_math
  coffee_mmi
  coffee_utilities
  coffee_dynobj
  coffee_xml
  ${FTGL_LIBRARY}
  ${FREETYPE_LIBRARIES}
  ${GLEW_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${OPENGL_glu_LIBRARY}
)
