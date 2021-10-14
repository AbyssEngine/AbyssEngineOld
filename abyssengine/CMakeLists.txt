cmake_minimum_required(VERSION 3.20)

project(abyssengine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(PNG REQUIRED)

if(MSVC)
        set(OPTIONS WIN32)
        set(SOURCE_EXTRA resource.rc)
endif(MSVC)
if(APPLE)

        set_source_files_properties(abyssbadge.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
        set(OPTIONS
                MACOSX_BUNDLE
                ./abyssbadge.icns
        )
endif(APPLE)

add_executable(${PROJECT_NAME} ${OPTIONS}
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Applications/"
        BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER abyssengine
        OUTPUT_NAME "Abyss Engine"
        MACOSX_BUNDLE_BUNDLE_NAME "Abyss Engine"
        MACOSX_BUNDLE_BUNDLE_VERSION "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}"
        MACOSX_BUNDLE_ICON_FILE abyssbadge.icns
        )

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)



target_sources(${PROJECT_NAME}
        PRIVATE
        src/abyssengine.c
        src/engine/engine.c src/engine/engine.h
        src/scripting/scripting.c src/scripting/scripting.h
        src/misc/util.c src/misc/util.h
        src/misc/ini.c src/misc/ini.h
        src/misc/resources.c src/misc/resources.h
        src/loader/loaderprovider.c src/loader/loaderprovider.h
        src/loader/filesystemloader.c src/loader/filesystemloader.h
        src/loader/loader.c src/loader/loader.h

        # lua
        src/lua/lapi.c src/lua/lapi.h src/lua/lauxlib.c src/lua/lauxlib.h src/lua/lbaselib.c src/lua/lcode.c src/lua/lcode.h
        src/lua/lcorolib.c src/lua/lctype.c src/lua/lctype.h src/lua/ldblib.c src/lua/ldebug.c src/lua/ldebug.h src/lua/ldo.c src/lua/ldo.h src/lua/ldump.c
        src/lua/lfunc.c src/lua/lfunc.h src/lua/lgc.c src/lua/lgc.h src/lua/linit.c src/lua/liolib.c src/lua/ljumptab.h src/lua/llex.c src/lua/llex.h
        src/lua/llimits.h src/lua/lmathlib.c src/lua/lmem.c src/lua/lmem.h src/lua/loadlib.c src/lua/lobject.c src/lua/lobject.h src/lua/lopcodes.c src/lua/lopcodes.h
        src/lua/lopnames.h src/lua/loslib.c src/lua/lparser.c src/lua/lparser.h src/lua/lprefix.h src/lua/lstate.c src/lua/lstate.h src/lua/lstring.c src/lua/lstring.h
        src/lua/lstrlib.c src/lua/ltable.c src/lua/ltable.h src/lua/ltablib.c src/lua/ltm.c src/lua/ltm.h src/lua/lua.h src/lua/lua.hpp
        src/lua/luaconf.h src/lua/lualib.h src/lua/lundump.c src/lua/lundump.h src/lua/lutf8lib.c src/lua/lvm.c src/lua/lvm.h src/lua/lzio.c
        src/lua/lzio.h

        ${SOURCE_EXTRA}
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/src/lua
        ${PROJECT_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        libabyss
        SDL2::SDL2main
        SDL2::SDL2
        PNG::PNG
)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

if(WIN32)
install(TARGETS ${PROJECT_NAME}
        RUNTIME_DEPENDENCIES
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
        DIRECTORIES C:/DevLibs/bin
        LIBRARY DESTINATION bin
        ARCHIVE DESTINATION bin
)
endif(WIN32)

if(UNIX AND NOT APPLE)
install(TARGETS ${PROJECT_NAME})
endif(UNIX AND NOT APPLE)

if(APPLE)

endif()
