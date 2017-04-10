cmake_minimum_required (VERSION 3.0)

project(all)

list(APPEND CMAKE_MODULE_PATH ${3rdparty_SOURCE_DIR}/cmake)

if(CMAKE_GENERATOR MATCHES "NMake[.]*")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE})
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
else()
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

set (COFFEE_DISABLE_VISIBILITY_FLAG TRUE)
include (COFFEESharedComponentSettings)

include_directories(${3rdparty_SOURCE_DIR}/mem_leak_detection/inc)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

list (APPEND CMAKE_INCLUDE_PATH ${3rdparty_SOURCE_DIR}/bzip2)
list (APPEND CMAKE_INCLUDE_PATH ${3rdparty_SOURCE_DIR}/zlib)
list (APPEND CMAKE_INCLUDE_PATH ${3rdparty_SOURCE_DIR}/expat/lib)
list (APPEND CMAKE_INCLUDE_PATH ${3rdparty_SOURCE_DIR}/python/Include)
list (APPEND CMAKE_INCLUDE_PATH ${3rdparty_SOURCE_DIR}/boost)
list (APPEND CMAKE_INCLUDE_PATH ${3rdparty_SOURCE_DIR}/glfw/include)
list (APPEND CMAKE_INCLUDE_PATH ${3rdparty_SOURCE_DIR}/glew/include)

list (APPEND CMAKE_LIBRARY_PATH ${CMAKE_BINARY_DIR}/lib/Release)
list (APPEND CMAKE_LIBRARY_PATH ${CMAKE_BINARY_DIR}/lib/Debug)
list (APPEND CMAKE_LIBRARY_PATH ${CMAKE_BINARY_DIR}/bin/Release)
list (APPEND CMAKE_LIBRARY_PATH ${CMAKE_BINARY_DIR}/bin/Debug)

if (MSVC)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
elseif (UNIX)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

#setup stuff if zlib is needed
set(ZLIB_FOUND TRUE)
set(ZLIB_ROOT ${3rdparty_SOURCE_DIR}/zlib)
set(ZLIB_INCLUDE_DIRS ${3rdparty_SOURCE_DIR}/zlib)
set(ZLIB_INCLUDE_DIR ${ZLIB_INCLUDE_DIRS})
if (WIN32)
  set(ZLIB_LIBRARY zlib)
else()
  set(ZLIB_LIBRARY z)
endif()
set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})


if(CMAKE_GENERATOR MATCHES "NMake[.]*")
#avoid enfless compiler warnings
  string(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  string(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  string(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
  string(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  string(REGEX REPLACE "/W[0-9]" "/W0" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
  string(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  string(REPLACE "/Zi" "/ZI" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  string(REPLACE "/Zi" "/ZI" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  string(REPLACE "/Zi" "/ZI" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
endif()

function(AddAssimpTo3rdparty)
  set(ASSIMP_LIB_STAND_ALONE 1)
  set(DEBUG_POSTFIX "" CACHE STRING "Debug Postfitx for lib, samples and tools")
  add_definitions(-DASSIMP_BUILD_NO_OWN_ZLIB)

  add_subdirectory(assimp)
endfunction(AddAssimpTo3rdparty)

function(AddPngTo3rdparty)
  set(PNG_STATIC OFF CACHE BOOL "Build static lib")
  set(SKIP_INSTALL_ALL TRUE)
  add_subdirectory(libpng)
endfunction(AddPngTo3rdparty)

function(AddOpenexrTo3rdparty)
  if (WIN32)
    include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/config.windows)
  else()
    include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/config)
  endif()

  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/Half)
  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/HalfTest)
  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/Iex)
  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/IexMath)
  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/IexTest)
  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/IlmThread)
  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/Imath)
  include_directories(${3rdparty_SOURCE_DIR}/openexr/ilmbase-2.2.0/ImathTest)

  set(NAMESPACE_VERSIONING OFF CACHE BOOL "Use Namespace Versioning")
  set(ILMBASE_PACKAGE_PREFIX ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

  add_subdirectory(openexr/ilmbase-2.2.0)
  add_subdirectory(openexr/openexr-2.2.0)
endfunction(AddOpenexrTo3rdparty)


function(AddBulletPhysicsTo3rdparty)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "Use shared libraries")
  set(USE_MSVC_RUNTIME_LIBRARY_DLL ON  CACHE BOOL "Use MSVC Runtime Library DLL (/MD or /MDd)")
  set(USE_MSVC_SSE ON  CACHE BOOL "Use MSVC /arch:sse option")
  set(USE_MSVC_FAST_FLOATINGPOINT ON  CACHE BOOL "Use MSVC /fp:fast option")
  set(USE_DOUBLE_PRECISION OFF CACHE BOOL "Use double precision")
  set(USE_GRAPHICAL_BENCHMARK ON CACHE BOOL "Use Graphical Benchmark")
  set(BUILD_BULLET3 ON CACHE BOOL "Set when you want to build Bullet 3")
  set(BUILD_CPU_DEMOS OFF CACHE BOOL "Build original Bullet CPU examples")
  set(USE_GLUT ON CACHE BOOL "Use Glut")
  set(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "Set when you want to build the OpenGL3+ demos")
  set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "Set when you want to build the Bullet 2 demos")
  set(BUILD_EXTRAS ON CACHE BOOL "Set when you want to build the extras")
  set(INSTALL_LIBS OFF CACHE BOOL "Set when you want to install libraries")
  set(INSTALL_EXTRA_LIBS OFF CACHE BOOL "Set when you want extra libraries installed")
  set(BUILD_UNIT_TESTS ON CACHE BOOL "Build Unit Tests")

  set(BUILD_SHARED_LIBS OFF)
  set(USE_MSVC_RUNTIME_LIBRARY_DLL ON)
  set(USE_MSVC_SSE ON)
  set(USE_MSVC_FAST_FLOATINGPOINT ON)
  set(USE_DOUBLE_PRECISION OFF)
  set(USE_GRAPHICAL_BENCHMARK OFF)
  set(BUILD_BULLET3 ON)
  set(BUILD_CPU_DEMOS OFF)
  set(USE_GLUT OFF)
  set(BUILD_OPENGL3_DEMOS OFF)
  set(BUILD_BULLET2_DEMOS OFF)
  set(BUILD_EXTRAS ON)
  set(INSTALL_LIBS OFF)
  set(INSTALL_EXTRA_LIBS OFF)
  set(BUILD_UNIT_TESTS ON)

  add_subdirectory(bulletphysics)
endfunction(AddBulletPhysicsTo3rdparty)

function(AddGlogTo3rdparty)
  add_subdirectory(glog)
endfunction(AddGlogTo3rdparty)

function(AddHdf5To3rdparty)
  set(HDF5_EXTERNALLY_CONFIGURED TRUE)
  set(HDF5_BUILD_TOOLS OFF CACHE BOOL "Build HDF5 Tools")
  set(HDF5_BUILD_EXAMPLES OFF CACHE BOOL "Build HDF5 Library Examples")
  set(BUILD_TESTING OFF CACHE BOOL "Build HDF5 Unit Testing")

  set(HDF5_BUILD_TOOLS OFF)
  set(HDF5_BUILD_EXAMPLES OFF)
  set(BUILD_TESTING OFF)

  add_subdirectory(hdf5)
endfunction(AddHdf5To3rdparty)

function(AddCaffeTo3rdparty)

  set(GFLAGS_INCLUDE_DIRS "${CMAKE_BINARY_DIR}/gflags/include")
  set(GFLAGS_LIBRARIES "gflags_shared")

  set(GLOG_INCLUDE_DIRS "${3rdparty_SOURCE_DIR}/glog/src")
  set(GLOG_LIBRARIES "glog")

  set(PROTOBUF_INCLUDE_DIR "${3rdparty_SOURCE_DIR}/protobuf/src")
  set(PROTOBUF_LIBRARIES "libprotobuf")
  set(PROTOBUF_PROTOC_EXECUTABLE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}/protoc")
  set(PROTOBUF_FOUND TRUE)

  set(OpenBLAS_INCLUDE_DIR "${3rdparty_SOURCE_DIR}/openblas;${CMAKE_BINARY_DIR}/openblas")
  set(OpenBLAS_LIB "libopenblas")

  set(HDF5_HL_INCLUDE_DIR "${3rdparty_SOURCE_DIR}/hdf5/src;${3rdparty_SOURCE_DIR}/hdf5/hl/src;${CMAKE_BINARY_DIR}/hdf5")
  set(HDF5_LIBRARIES "hdf5-shared;hdf5_hl-shared")

  include_directories(SYSTEM ${HDF5_INCLUDE_DIRS} ${HDF5_HL_INCLUDE_DIR})
  list(APPEND Caffe_LINKER_LIBS ${HDF5_LIBRARIES})

  add_subdirectory(caffe)
endfunction(AddCaffeTo3rdparty)

function(AddOpenCVTo3rdparty)

  set(PNG_FOUND TRUE)
  set(PNG_LIBRARY "png")

  #MESSAGE( "PNG_LIBRARY set to" ${PNG_LIBRARY} )

  set(PNG_LIBRARIES ${PNG_LIBRARY})
  set(PNG_INCLUDE_DIR ${3rdparty_SOURCE_DIR}/libpng)
  set(PNG_DEFINITIONS "")

  set(BUILD_opencv_world ON)
  set(SAVE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
  MESSAGE(STATUS "SAVE_CXX_FLAGS=${SAVE_CXX_FLAGS}")
  string(REPLACE "/arch:AVX" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
  MESSAGE(STATUS "CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}")

  add_subdirectory(opencv)

  set(CMAKE_CXX_FLAGS_RELEASE ${SAVE_CXX_FLAGS})
  MESSAGE(STATUS "CMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}")

endfunction(AddOpenCVTo3rdparty)

function(AddProtobufTo3rdparty)
  set(protobuf_BUILD_TESTS OFF CACHE BOOL "Build tests")
  set(protobuf_BUILD_TESTS OFF)

  add_subdirectory(protobuf/cmake)
endfunction(AddProtobufTo3rdparty)

function(AddArucoTo3rdparty)

   set(EIGEN3_INCLUDE_PATH "${3rdparty_SOURCE_DIR}/eigen3/Eigen")

   set(OPENCV_DIR ${3rdparty_SOURCE_DIR}/opencv)
   set(OPENCV_INCLUDE_PATH ${OPENCV_DIR}/modules/core/include;${OPENCV_DIR}/modules/flann/include;${OPENCV_DIR}/modules/imgproc/include;${OPENCV_DIR}/modules/ml/include;${OPENCV_DIR}/modules/photo/include;${OPENCV_DIR}/modules/video/include;${OPENCV_DIR}/modules/imgcodecs/include;${OPENCV_DIR}/modules/shape/include;${OPENCV_DIR}/modules/videoio/include;${OPENCV_DIR}/modules/highgui/include;${OPENCV_DIR}/modules/objdetect/include;${OPENCV_DIR}/modules/superres/include;${OPENCV_DIR}/modules/features2d/include;${OPENCV_DIR}/modules/calib3d/include;${OPENCV_DIR}/modules/stitching/include;${OPENCV_DIR}/modules/videostab/include;${OPENCV_DIR}/modules/world/include)
   set(OpenCV_INCLUDE_DIRS ${OPENCV_INCLUDE_PATH})
   set(OpenCV_LIBS "opencv_world")

   message(status "OpenCV_INCLUDE_DIRS=${OpenCV_INCLUDE_DIRS}")
   message(status "OPENCV_LIBRARY=${OPENCV_LIBRARY}")

   add_subdirectory(aruco)
endfunction(AddArucoTo3rdparty)

COFFEEPrintSettings()

add_subdirectory(mem_leak_detection)

add_subdirectory(boost)

AddPngTo3rdparty()
AddBulletPhysicsTo3rdparty()

add_subdirectory(cmake)
add_subdirectory(bzip2)
add_subdirectory(bink)
add_subdirectory(expat)
add_subdirectory(fastmathparser)
add_subdirectory(fbxsdk)
add_subdirectory(freetype)
add_subdirectory(ftgl)
add_subdirectory(glew)
add_subdirectory(glfw)
add_subdirectory(imgui)
add_subdirectory(json_spirit)
add_subdirectory(libogg)
add_subdirectory(libvorbis)
add_subdirectory(libtheora)
add_subdirectory(libxml2)
add_subdirectory(libxslt)
add_subdirectory(openal)
add_subdirectory(python)
add_subdirectory(sqlite)
add_subdirectory(telemetry)
add_subdirectory(zlib)

if (WIN32)
  add_subdirectory(IGT264Codec)
  add_subdirectory(AscentSDKIGTGame)
  add_subdirectory(gflags)
  add_subdirectory(openblas)
  add_subdirectory(leapmotion)
  add_subdirectory(ultrahaptics)
  add_subdirectory(seefront)
  add_subdirectory(seefrontv2)
  add_subdirectory(fann)
  add_subdirectory(royale)
  add_subdirectory(fox)
  add_subdirectory(directxtex)
  add_subdirectory(mayasdk)

  AddProtobufTo3rdparty()
  AddAssimpTo3rdparty()
  AddOpenexrTo3rdparty()
  AddGlogTo3rdparty()
  AddCaffeTo3rdparty()
  AddHdf5To3rdparty()
  AddOpenCVTo3rdparty()
  AddArucoTo3rdparty()
endif()


