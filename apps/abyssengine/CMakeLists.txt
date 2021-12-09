cmake_minimum_required(VERSION 3.20)



# -------------------------------------------------------------------------------------------------
# Project
# -------------------------------------------------------------------------------------------------

set(APP_NAME abyssengine)

if (MSVC)
    set(OPTIONS WIN32)
    set(SOURCE_EXTRA resource.rc)
    set(APP_NAME "AbyssEngine")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_HAS_DEPRECATED_RESULT_OF=1)
    add_compile_options(/bigobj)
endif (MSVC)

if (APPLE)
    set_source_files_properties(abyssbadge.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(OPTIONS MACOSX_BUNDLE abyssbadge.icns src/abyssengine.cpp)
endif (APPLE)

add_executable(${APP_NAME} ${OPTIONS})

# -------------------------------------------------------------------------------------------------
# Source Files
# -------------------------------------------------------------------------------------------------

target_sources(${APP_NAME} PRIVATE
        # Engine Core --------------------------------------------------
        src/abyssengine.cpp
        src/engine/engine.cpp
        src/engine/loader.cpp
        src/engine/filesystemprovider.cpp
        src/engine/cascprovider.cpp
        src/engine/mpqprovider.cpp
        src/engine/spritefont.cpp
        src/engine/soundeffect.cpp
        src/common/alignment.cpp
        src/common/blendmode.cpp
        src/common/color.cpp

        # Nodes --------------------------------------------------------
        src/node/node.cpp
        src/node/sprite.cpp
        src/node/dc6sprite.cpp
        src/node/d2rsprite.cpp
        src/node/video.cpp
        src/node/label.cpp
        src/node/button.cpp

        # Scripting ----------------------------------------------------
        src/engine/scripthost.cpp

        # SystemIO - SDL2 ----------------------------------------------
        src/systemio/sdl2/sdl2systemio.cpp
        src/systemio/sdl2/sdl2texture.cpp

        # Dynamic ------------------------------------------------------
        ${SOURCE_EXTRA}

        # Others -------------------------------------------------------
        ${SOURCE_APP}
        )


# -------------------------------------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------------------------------------

target_include_directories(${APP_NAME}
        PRIVATE
        ${PROJECT_BINARY_DIR}
        ${LUA_INCLUDE_DIR}
        ${FFMPEG_INCLUDE_DIRS}
        )


# -------------------------------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------------------------------

target_compile_features(${APP_NAME} PUBLIC cxx_std_20)
add_definitions(-DSOL_ALL_SAFETIES_ON)
target_compile_options(${APP_NAME} PRIVATE ${FFMPEG_DEFINITIONS})


# -------------------------------------------------------------------------------------------------
# HostNotify Source Files (Platform Dependant)
# -------------------------------------------------------------------------------------------------

if (MSVC)
    target_sources(${APP_NAME} PRIVATE src/hostnotify/hostnotify_windows.cpp)
elseif (APPLE)
    find_library(APPKIT_LIBRARY AppKit)
    target_sources(${APP_NAME} PRIVATE src/hostnotify/hostnotify_mac.cpp src/hostnotify/hostnotify_mac_shim.m)
else ()
    target_sources(${APP_NAME} PRIVATE src/hostnotify/hostnotify_stub.cpp)
endif ()


# -------------------------------------------------------------------------------------------------
# MacOS Bundle Information
# -------------------------------------------------------------------------------------------------

if (APPLE)
    set_target_properties(${APP_NAME} PROPERTIES
            FOLDER "Applications/"
            BUNDLE TRUE
            MACOSX_BUNDLE_GUI_IDENTIFIER "Abyss Engine"
            MACOSX_PRODUCT_BUNDLE_IDENTIFIER abyssengine
            PRODUCT_BUNDLE_IDENTIFIER ABYSSENGINE
            OUTPUT_NAME "Abyss Engine"
            MACOSX_BUNDLE_BUNDLE_NAME "Abyss Engine"
            CPACK_BUNDLE_APPLE_ENTITLEMENTS "macos.entitlements"
            MACOSX_BUNDLE_BUNDLE_VERSION "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
            MACOSX_BUNDLE_ICON_FILE abyssbadge.icns
            )
endif()


# -------------------------------------------------------------------------------------------------
# Libraries
# -------------------------------------------------------------------------------------------------

target_link_libraries(${APP_NAME}
        PRIVATE
        libabyss
        absl::flat_hash_map
        Threads::Threads
        SDL2::SDL2main
        SDL2::SDL2
        SDL2::SDL2_ttf
        spdlog::spdlog_header_only
        sol2::sol2
        cppzmq
        ${FFMPEG_LIBRARIES}
        ${LUA_LIBRARIES}
        )


# -------------------------------------------------------------------------------------------------
# FFMPEG Dependencies
# -------------------------------------------------------------------------------------------------

if (UNIX)
    if (APPLE)
        target_link_libraries(${APP_NAME} PRIVATE ${OSX_VIDEOTOOLBOX} ${OSX_COREMEDIA} ${OSX_SECURITY})
    else ()
        target_link_libraries(${APP_NAME} PRIVATE avformat avcodec swscale avutil swresample avdevice m z)
    endif ()
endif ()


# -------------------------------------------------------------------------------------------------
# Windows Installation
# -------------------------------------------------------------------------------------------------

if (WIN32)
    install(TARGETS ${APP_NAME}
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            )
endif (WIN32)


# -------------------------------------------------------------------------------------------------
# Linux Installation
# -------------------------------------------------------------------------------------------------

if (UNIX AND NOT APPLE)
    install(TARGETS ${APP_NAME})
endif (UNIX AND NOT APPLE)
