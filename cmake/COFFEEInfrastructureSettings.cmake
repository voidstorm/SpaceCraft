cmake_minimum_required (VERSION 2.8.3)

if (NOT COFFEE_INFRASTRUCTURE_SETTINGS_SETUP)
    # Installing crash server conflicts with JNI on Linux.
    if(UNIX)
      option(
        DISABLE_CRASH_OBSERVER
        "Flag specifying whether the crash observer will be installed at runtime."
        OFF)
    endif()

    # for speacila infrastructure builds
    set(PLATFORM_CODE_NAME $ENV{PLATFORM_CODE_NAME})
    if (PLATFORM_CODE_NAME)
        set (COFFEE_DEFINES "${COFFEE_DEFINES} -DPLATFORM_CODE_NAME_${PLATFORM_CODE_NAME}")
    else()
        message(STATUS "PLATFORM_CODE_NAME environment variable not defined (Will be set to DEFAULT)")
        set (COFFEE_DEFINES "${COFFEE_DEFINES} -DPLATFORM_CODE_NAME_DEFAULT")
    endif()

    # generate script enums
    set (COFFEE_DEFINES "${COFFEE_DEFINES} -DENUM_STRING_LIST_GEN")

    include (${CMAKE_CURRENT_LIST_DIR}/COFFEESettings.cmake)

    link_directories(
      ${COFFEE_BASE_DIR}/bin/${COFFEE_BIN_DIRECTORY}
    )

    COFFEEAddCopyAscentTarget()

    set (COFFEE_INFRASTRUCTURE_SETTINGS_SETUP TRUE)
endif()
