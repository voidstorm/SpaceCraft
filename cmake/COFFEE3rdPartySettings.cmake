if(NOT DEFINED COFFEE_3RDPARTY_DIR)
  get_filename_component(COFFEE_3RDPARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/../../3rdparty ABSOLUTE)
endif()

if(NOT DEFINED BOOST_ROOT)
  get_filename_component(BOOST_ROOT ${COFFEE_3RDPARTY_DIR}/boost ABSOLUTE)
endif()

# --------------------------------------------------------------------
# setup 3rdparty build dir and library search path
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/aruco/src)
if(WIN32)

  if(NOT DEFINED COFFEE_3RDPARTY_BUILD_DIR)
    if(CMAKE_GENERATOR MATCHES "Visual Studio[.]*")
      get_filename_component(COFFEE_3RDPARTY_BUILD_DIR ${COFFEE_3RDPARTY_DIR}/${COFFEE_BUILD_FOLDER} ABSOLUTE)
    elseif(CMAKE_GENERATOR MATCHES "NMake[.]*")
      get_filename_component(COFFEE_3RDPARTY_BUILD_DIR ${COFFEE_3RDPARTY_DIR}/build_nmake ABSOLUTE)
    endif()
  endif()

  list(APPEND CMAKE_LIBRARY_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/lib)
  list(APPEND CMAKE_LIBRARY_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/bin)
  list(APPEND CMAKE_LIBRARY_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/lib/Release)
  list(APPEND CMAKE_LIBRARY_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/bin/Release)
  list(APPEND CMAKE_LIBRARY_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/lib/Debug)
  list(APPEND CMAKE_LIBRARY_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/bin/Debug)

  set(COFFEE_3RDPARTY_LIB_DIR ${COFFEE_3RDPARTY_BUILD_DIR}/lib/${COFFEE_CONFIG_DIR})
  set(COFFEE_3RDPARTY_BIN_DIR ${COFFEE_3RDPARTY_BUILD_DIR}/bin/${COFFEE_CONFIG_DIR})
else()

  if(NOT DEFINED COFFEE_3RDPARTY_BUILD_DIR)
    get_filename_component(COFFEE_3RDPARTY_BUILD_DIR ${COFFEE_3RDPARTY_DIR}/build_linux/${CMAKE_BUILD_TYPE} ABSOLUTE)
  endif()

  set(CMAKE_LIBRARY_PATH
    ${CMAKE_LIBRARY_PATH}
    ${COFFEE_3RDPARTY_BUILD_DIR}/lib
    ${COFFEE_3RDPARTY_BUILD_DIR}/bin
  )

  set(COFFEE_3RDPARTY_LIB_DIR ${COFFEE_3RDPARTY_BUILD_DIR}/lib)
  set(COFFEE_3RDPARTY_BIN_DIR ${COFFEE_3RDPARTY_BUILD_DIR}/bin)

endif()

# --------------------------------------------------------------------
# Needed executable paths for finding 3rd party stuff.
list(APPEND CMAKE_PROGRAM_PATH ${COFFEE_3RDPARTY_DIR}/gsoap)

# --------------------------------------------------------------------
# Needed include paths for finding 3rd party stuff.
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_BUILD_DIR})
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/caffe/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/gflags/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_BUILD_DIR}/glog)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/assimp/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/bink/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/bulletphysics/src)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/bulletphysics/Extras/ConvexDecomposition)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/bzip2)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/caffe/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/cajun)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/directxtex)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/eigen3)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/expat/lib)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/fastmathparser)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/fbxsdk/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/fox/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/freetype/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/ftgl/src)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/glew/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/glfw/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/glm)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/glog/src)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/gsoap)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/imgui/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/json_spirit/json_spirit)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/leapmotion/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/libogg/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/libpng)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/libtheora/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/libvorbis/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/libxml2/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/libxslt)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/mem_leak_detection/inc)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/mayasdk/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/openal/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/openblas)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/opencv/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/protobuf/src)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/python/Include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/royale/${COFFEE_OS_DIRECTORY}/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/seefront/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/seefront/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/seefrontv2/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/sqlite)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/telemetry/include)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/ultrahaptics/${COFFEE_OS_DIRECTORY}/include/ultrahaptics)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/zlib)
list(APPEND CMAKE_INCLUDE_PATH ${COFFEE_3RDPARTY_DIR}/IGT264Codec/inc)

set(Python_ADDITIONAL_VERSIONS 2.6)

# per default we setup path to MLD
find_package(MLD)
include_directories(${MLD_INCLUDE_DIRS})

# per default we setup path to Boost
find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

# set default 3rdparty lib search path
link_directories(${COFFEE_3RDPARTY_LIB_DIR})
link_directories(${COFFEE_3RDPARTY_BIN_DIR})

function(COFFEEAddCopy3rdPartyTarget)

    find_package(Boost)
    find_package(SeeFront)
    find_package(SeeFront2)
    find_package(LeapMotion)
    find_package(Ultrahaptics)
    find_package(Royale)
    find_package(OpenCV)
    find_package(IGT264)

    if(WIN32)
        if (MSVC14)
           set (BOOST_VC_VERSION vc140)
        elseif (MSVC12)
           set (BOOST_VC_VERSION vc120)
        elseif(MSVC10)
           set (BOOST_VC_VERSION vc100)
        endif()

        set (ASCENT_IGT_GAME_FILES
           IGT.Game.Core.Communication.AutomationServices.dll
           IGT.Game.Core.Communication.Cabinet.CSI.Schemas.dll
           IGT.Game.Core.Communication.Cabinet.CsiTransport.dll
           IGT.Game.Core.Communication.Cabinet.Standard.dll
           IGT.Game.Core.Communication.Cabinet.dll
           IGT.Game.Core.Communication.CommunicationLib.dll
           IGT.Game.Core.Communication.FastPlay.dll
           Foundation.InterfaceExtensions.dll
           InterfaceExtensions.CdsBingo.dll
           InterfaceExtensions.CdsItaly.dll
           IGT.Game.Core.Communication.Foundation.InterfaceExtensions.CdsPullTab.dll
           InterfaceExtensions.ForceGameCompletion.dll
           InterfaceExtensions.GameGroupControl.dll
           InterfaceExtensions.GameLink.dll
           InterfaceExtensions.Identification.dll
           InterfaceExtensions.JackpotItaly.dll
           InterfaceExtensions.ParcelComm.dll
           InterfaceExtensions.PlayerSession.dll
           InterfaceExtensions.PlayerSessionParams.dll
           InterfaceExtensions.ProtocolIppForwarding.dll
           InterfaceExtensions.ThemedHandpay.dll
           InterfaceExtensions.TiltManagement.dll
           InterfaceExtensions.Tournament.dll
           InterfaceExtensions.Ugp.dll
           IGT.Game.Core.Communication.Foundation.F2L.Schemas.dll
           IGT.Game.Core.Communication.Foundation.F2L.dll
           IGT.Game.Core.Communication.Foundation.F2X.Schemas.dll
           IGT.Game.Core.Communication.Foundation.F2X.dll
           IGT.Game.Core.Communication.Foundation.F2XTransport.dll
           IGT.Game.Core.Communication.Foundation.Standard.dll
           IGT.Game.Core.Communication.Foundation.Transport.dll
           IGT.Game.Core.Communication.Foundation.dll
           IGT.Game.Core.Communication.GL2PInterceptorLib.dll
           IGT.Game.Core.Communication.GL2PInterceptorLib.SocketCommunication.dll
           IGT.Game.Core.Communication.Logic.CommServices.dll
           IGT.Game.Core.Communication.LogicPresentationBridge.dll
           IGT.Game.Core.Communication.Presentation.CommServices.dll
           IGT.Game.Core.Communication.UC.Cabinet.dll
           protobuf-net.dll
           IGT.UC.CabinetControlProtocol.Contracts.dll
           IGT.UC.CabinetControlProtocol.dll
           IGT.Game.Core.Communication.dll
           IGT.Game.Core.CompactSerialization.dll
           IGT.Game.Core.GameMeter.dll
           IGT.Game.Core.GameReport.GamePerformance.Schemas.dll
           IGT.Game.Core.GameReport.GamePerformance.dll
           IGT.Game.Core.GameReport.Interfaces.dll
           IGT.Game.Core.GameReport.dll
           IGT.Game.Core.Linq.dll
           IGT.Game.Core.Logging.dll
           IGT.Game.Core.Logic.BetFramework.dll
           IGT.Game.Core.Logic.Denomination.dll
           IGT.Game.Core.Logic.Evaluator.Providers.dll
           IGT.Game.Core.Logic.Evaluator.Schemas.dll
           IGT.Game.Core.Logic.Evaluator.dll
           IGT.Game.Core.Logic.Foundation.ServiceProviders.dll
           IGT.Game.Core.Logic.ProgressiveController.ServiceProviders.dll
           IGT.Game.Core.Logic.ProgressiveController.dll
           IGT.Game.Core.Logic.RandomNumbers.dll
           IGT.Game.Core.Logic.Services.dll
           IGT.Game.Core.Logic.StateMachine.StateHelpers.dll
           IGT.Game.Core.Logic.StateMachine.dll
           IGT.Game.Core.Metrics.dll
           IGT.Game.Core.Money.dll
           IGT.Game.Core.Presentation.BankSynchronization.dll
           IGT.Game.Core.Presentation.Cabinet.dll
           IGT.Game.Core.Presentation.CabinetServices.dll
           IGT.Game.Core.Presentation.DepthCamera.dll
           IGT.Game.Core.Presentation.MechanicalReels.dll
           IGT.Game.Core.Presentation.Monitor.dll
           IGT.Game.Core.Presentation.PeripheralLights.ChromaBlend.dll
           IGT.Game.Core.Presentation.PeripheralLights.Devices.dll
           IGT.Game.Core.Presentation.PeripheralLights.Streaming.dll
           IGT.Game.Core.Presentation.PeripheralLights.dll
           IGT.Game.Core.Presentation.Services.dll
           IGT.Game.Core.Presentation.States.dll
           IGT.Game.Core.Presentation.UC.dll
           IGT.Game.Core.Presentation.VideoTopper.dll
           IGT.Game.Core.Registries.Internal.dll
           IGT.Game.Core.Session.dll
           IGT.Game.Core.Testing.FastPlay.Schemas.dll
           IGT.Game.Core.Testing.FastPlay.Serialization.dll
           IGT.Game.Core.Testing.FastPlay.dll
           IGT.Game.Core.Threading.dll
           IGT.Game.Core.Tilts.dll
           IGT.Game.Core.Timing.dll
           IGT.Game.Core.ToolSupport.Schemas.dll
        )

        set(3RDPARTY_FILES
          ${ASCENT_IGT_GAME_FILES}
          FOXDLL-1.6$<$<CONFIG:Debug>:D>.dll
          OpenAL32.dll
          assimp.dll
          boost_regex-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_date_time-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_filesystem-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_iostreams-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_program_options-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_serialization-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_system-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_wserialization-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_log-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_log_setup-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_chrono-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          boost_thread-${BOOST_VC_VERSION}-mt-$<$<CONFIG:Debug>:gd->${Boost_LIB_VERSION}.dll
          bz2.dll
          expat.dll
          freetype.dll
          ftgl.dll
          glew.dll
          libxml2.dll
          libxslt.dll
          libexslt.dll
          libfbxsdk.dll
          ogg.dll
          theora.dll
          vorbis.dll
          vorbisfile.dll
          zlib.dll
          glfw3.dll
          libpng.dll
          imgui.dll
          Half.dll
          Iex.dll
          IexMath.dll
          IlmImf.dll
          IlmImfUtil.dll
          IlmThread.dll
          Imath.dll
          opencv_world${OPENCV_COPY_VERSION}.dll
          gflags.dll
          glog.dll
          hdf5$<$<CONFIG:Debug>:_D>.dll
          hdf5_cpp$<$<CONFIG:Debug>:_D>.dll
          hdf5_hl$<$<CONFIG:Debug>:_D>.dll
          hdf5_hl_cpp$<$<CONFIG:Debug>:_D>.dll
          libprotobuf$<$<CONFIG:Debug>:d>.dll
          libopenblas.dll
          caffe-${CMAKE_CFG_INTDIR}.dll
          aruco209$<$<CONFIG:Debug>:_d>.dll
          mld_glob.dll
          mld_heap.dll
        )

        if (IGT264_FOUND)
          set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                IGT264Decoder.dll
              )

          if (COFFEE_IS_64BIT)
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                amf-component-uvd-windesktop64.dll
                amf-core-windesktop64.dll
               )
          else()
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                amf-component-uvd-windesktop32.dll
                amf-core-windesktop32.dll
               )
          endif()
        endif()

        if (SEEFRONT_FOUND)
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                seefrontblueboxcontrol.dll
                ftd2xx.dll
               )
        endif()

        if (SEEFRONT2_FOUND)
          set(3RDPARTY_FILES
              ${3RDPARTY_FILES}
              sfbluebox2control.dll
              ftd2xx.dll
             )
        endif()

        if (LEAP_MOTION_FOUND)
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                Leap.dll
               )
        endif()

        if (ROYALE_FOUND)
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                royale.dll
               )
        endif()

        if (ULTRAHAPTICS_FOUND)
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                libusb-1.0.dll
                Ultrahaptics.dll
                UltrahapticsCommon.dll
                UltrahapticsDriver.dll
                UltrahapticsHardware.dll
               )
        endif()

        if (COFFEE_IS_64BIT)
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                bink2w64.dll
                telemetry64.dll
                telemetry64c.dll
               )
        else()
            set(3RDPARTY_FILES
                ${3RDPARTY_FILES}
                bink2w32.dll
                telemetry32.dll
                telemetry32c.dll
               )
        endif()

        if(CMAKE_GENERATOR MATCHES "Visual Studio[.]*")
            message(STATUS "binaries should be copied from: ${COFFEE_3RDPARTY_BIN_DIR}")
            COFFEEAddCopyFilesTarget(xx_copy_3rdparty x50_copy_targets "${3RDPARTY_FILES}"
                             "${COFFEE_3RDPARTY_BIN_DIR}"
                             "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}")
        else()
            message(STATUS "binaries should be copied from: ${COFFEE_3RDPARTY_BIN_DIR}")
            COFFEEAddCopyFilesTarget(xx_copy_3rdparty x50_copy_targets "${3RDPARTY_FILES}"
                             "${COFFEE_3RDPARTY_BIN_DIR}"
                             "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")
        endif()

    else()
        set(3RDPARTY_FILES
            libboost_date_time-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_filesystem-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_iostreams-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_program_options-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_regex-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_serialization-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_system-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_wserialization-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_log-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_log_setup-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_chrono-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libboost_thread-mt.so.${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}
            libbz2.so
            libexpat.so
            libfreetype.so
            libftgl.so
            libglew.so
            liblibxml2.so
            liblibxslt.so
            liblibexslt.so
            libogg.so
            libopenal.so
            libTelemetryX86.so
            libTelemetryX86c.so
            libtheora.so
            libvorbis.so
            libvorbisfile.so
            libz.so
            libglfw.so.3
            libpng.so
            libimgui.so
            libmld_glob.so
            libmld_heap.so
        )

        COFFEEAddCopyFilesTarget(xx_copy_3rdparty x50_copy_targets "${3RDPARTY_FILES}"
                             "${COFFEE_3RDPARTY_LIB_DIR}"
                             "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}")

    endif()

endfunction(COFFEEAddCopy3rdPartyTarget)
