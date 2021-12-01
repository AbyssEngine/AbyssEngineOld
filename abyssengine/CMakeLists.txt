cmake_minimum_required(VERSION 3.20)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    set(VCPKG_BUILD_TYPE release)
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(APP_NAME "abyssengine")

project(abyssengine CXX)

find_package(ZLIB REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Lua 5.4 EXACT REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE POSTPROC REQUIRED)
find_package(sdl2-ttf)
find_package(absl CONFIG REQUIRED)

if (NOT sdl2-ttf_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
    add_library(SDL2::SDL2_ttf ALIAS PkgConfig::SDL2TTF)
endif ()

if (APPLE)
    find_library(OSX_VIDEOTOOLBOX VideoToolbox)
    find_library(OSX_COREMEDIA CoreMedia)
    find_library(OSX_SECURITY Security)
endif ()


add_definitions(-DSOL_ALL_SAFETIES_ON)

if (MSVC)
    set(OPTIONS WIN32)
    set(SOURCE_EXTRA resource.rc)
    set(APP_NAME "AbyssEngine")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_HAS_DEPRECATED_RESULT_OF=1)
endif (MSVC)

if (APPLE)
    set(APP_NAME "Abyss Engine")
    set_source_files_properties(abyssbadge.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(OPTIONS
            MACOSX_BUNDLE
            ./abyssbadge.icns src/abyssengine.cpp)
endif (APPLE)


add_executable(${PROJECT_NAME} ${OPTIONS})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE -DSPDLOG_DEBUG_ON -DSPDLOG_TRACE_ON)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Applications/"
        BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_NAME}
        MACOSX_PRODUCT_BUNDLE_IDENTIFIER abyssengine
        PRODUCT_BUNDLE_IDENTIFIER ABYSSENGINE
        OUTPUT_NAME ${APP_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
        CPACK_BUNDLE_APPLE_ENTITLEMENTS "macos.entitlements"
        MACOSX_BUNDLE_BUNDLE_VERSION "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
        MACOSX_BUNDLE_ICON_FILE abyssbadge.icns
        )

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)

target_compile_options(${PROJECT_NAME} PRIVATE ${FFMPEG_DEFINITIONS})

if (MSVC)
    list(APPEND SOURCE_EXTRA
            src/hostnotify/hostnotify_windows.cpp
            )
elseif (APPLE)
    find_library(APPKIT_LIBRARY AppKit)
    list(APPEND SOURCE_EXTRA
            src/hostnotify/hostnotify_mac.cpp
            src/hostnotify/hostnotify_mac_shim.m
            )
else ()
    list(APPEND SOURCE_EXTRA src/hostnotify/hostnotify_stub.cpp)
endif ()

target_sources(${PROJECT_NAME} PRIVATE
        # Engine Core --------------------------------------------------
        src/abyssengine.cpp
        src/engine/engine.cpp
        src/engine/loader.cpp
        src/engine/filesystemprovider.cpp
        src/engine/cascprovider.cpp
        src/engine/mpqprovider.cpp
        src/engine/spritefont.cpp
        src/common/inifile.cpp
        src/common/ringbuffer.cpp
        src/common/alignment.cpp
        src/common/blendmode.cpp
        src/common/color.cpp
        src/systemio/interface.cpp

        # Nodes --------------------------------------------------------
        src/node/node.cpp
        src/node/sprite.cpp
        src/node/dc6sprite.cpp
        src/node/video.cpp
        src/node/label.cpp
        src/node/button.cpp

        # Scripting ----------------------------------------------------
        src/scripting/scripthost.cpp

        # SystemIO - SDL2 ----------------------------------------------
        src/systemio/sdl2/sdl2systemio.cpp
        src/systemio/sdl2/sdl2texture.cpp

        # Dynamic ------------------------------------------------------
        ${SOURCE_EXTRA}

        # Others -------------------------------------------------------
        ${SOURCE_APP}
        )


target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}
        ${LUA_INCLUDE_DIR}
        ${FFMPEG_INCLUDE_DIRS}
        )

message(Defs: ${FFMPEG_LIBRARIES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        libabyss
        absl::flat_hash_map
        Threads::Threads
        SDL2::SDL2main
        SDL2::SDL2
        SDL2::SDL2_ttf
        PNG::PNG
        spdlog::spdlog_header_only
        sol2::sol2
        ${FFMPEG_LIBRARIES}
        ${LUA_LIBRARIES}
        )

if (UNIX)
    if (APPLE)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${OSX_VIDEOTOOLBOX} ${OSX_COREMEDIA} ${OSX_SECURITY})
    else ()
        target_link_libraries(${PROJECT_NAME} PRIVATE avformat avcodec swscale avutil swresample avdevice m z)
    endif ()
endif ()

if (WIN32)
    install(TARGETS ${PROJECT_NAME}
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            )
endif (WIN32)

if (UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME})
endif (UNIX AND NOT APPLE)
