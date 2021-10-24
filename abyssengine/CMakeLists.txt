cmake_minimum_required(VERSION 3.20)

project(abyssengine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(APP_NAME "abyssengine")

find_package(ZLIB REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(Lua 5.4 EXACT REQUIRED)
find_package(Threads REQUIRED)
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE POSTPROC REQUIRED)

if (MSVC)
    set(OPTIONS WIN32)
    set(SOURCE_EXTRA resource.rc)
    set(APP_NAME "AbyssEngine")
endif (MSVC)

if (APPLE)
    set(APP_NAME "Abyss Engine")
    set_source_files_properties(abyssbadge.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(OPTIONS
            MACOSX_BUNDLE
            ./abyssbadge.icns)
endif (APPLE)

add_executable(${PROJECT_NAME} ${OPTIONS})

set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Applications/"
        BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_NAME}
        MACOSX_PRODUCT_BUNDLE_IDENTIFIER abyssengine
        PRODUCT_BUNDLE_IDENTIFIER ABYSSENGINE
        OUTPUT_NAME ${APP_NAME}
        MACOSX_BUNDLE_BUNDLE_NAME ${APP_NAME}
        MACOSX_BUNDLE_BUNDLE_VERSION "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
        MACOSX_BUNDLE_ICON_FILE abyssbadge.icns
        )

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)

target_compile_options(${PROJECT_NAME} PRIVATE ${FFMPEG_DEFINITIONS})

target_sources(${PROJECT_NAME} PRIVATE
        # Engine Core --------------------------------------------------
        src/abyssengine.c
        src/commondef.h
        src/engine/engine.c src/engine/engine.h
        src/engine/sysfont.c src/engine/sysfont.h

        # Engine Modes -------------------------------------------------
        src/engine/modeboot.c src/engine/modeboot.h
        src/engine/modecrash.c src/engine/modecrash.h
        src/engine/moderun.c src/engine/moderun.h
        src/engine/modevideo.c src/engine/modevideo.h

        # Node ----------------------------------------------------------
        src/node/node.c src/node/node.h

        # Sprite --------------------------------------------------------
        src/node/sprite/sprite.c src/node/sprite/sprite.h

        # Scripting ----------------------------------------------------
        src/scripting/scripting.c src/scripting/scripting.h
        src/scripting/node.c src/scripting/node.h
        src/scripting/log.c
        src/scripting/getconfig.c
        src/scripting/addloaderprovider.c
        src/scripting/shutdown.c
        src/scripting/showsystemcursor.c
        src/scripting/sleep.c
        src/scripting/setboottext.c
        src/scripting/loadstring.c
        src/scripting/exitbootmode.c
        src/scripting/loadpalette.c
        src/scripting/loadsprite.c
        src/scripting/setcursor.c
        src/scripting/getrootnode.c
        src/scripting/playvideo.c

        # Loaders ------------------------------------------------------
        src/loader/loaderprovider.c src/loader/loaderprovider.h
        src/loader/filesystemloader.c src/loader/filesystemloader.h
        src/loader/mpqloader.c src/loader/mpqloader.h
        src/loader/loader.c src/loader/loader.h

        # Misc ---------------------------------------------------------
        src/misc/util.c src/misc/util.h
        src/misc/ini.c src/misc/ini.h
        src/misc/resources.c src/misc/resources.h
        src/misc/appcompat.c src/misc/appcompat.h

        # Dynamic ------------------------------------------------------
        ${SOURCE_EXTRA}
        )

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}
        ${LUA_INCLUDE_DIR}
        ${FFMPEG_INCLUDE_DIRS}
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        libabyss
        Threads::Threads
        SDL2::SDL2main
        SDL2::SDL2
        PNG::PNG
        ${FFMPEG_LIBRARIES}
        ${LUA_LIBRARIES}
        )

if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m swresample avformat avcodec)
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
