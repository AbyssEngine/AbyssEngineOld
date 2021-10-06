cmake_minimum_required(VERSION 3.20)

project(abyssengine C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)
find_package(Lua 5.4 REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(PNG REQUIRED)

add_executable(${PROJECT_NAME}
        MACOSX_BUNDLE
        ./abyssbadge.icns
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
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

set_source_files_properties(abyssbadge.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

target_sources(${PROJECT_NAME}
PRIVATE
        src/abyssengine.c
        src/engine/engine.c
        src/scripting/scripting.c
        src/misc/util.c
        src/misc/ini.c
        src/loader/loaderprovider.c
        src/loader/filesystemloader.c
        src/loader/loader.c
)

add_definitions(
    ${PNG_DEFINITIONS}
)

include_directories(
    ${PROJECT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/libabyss/include
    SDL2::Include
    ${LUA_INCLUDE_DIR}
    ${PNG_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
PRIVATE
  libabyss
  ZLIB::ZLIB
  SDL2::SDL2main
  SDL2::SDL2-static
  ${PNG_LIBRARIES}
  ${LUA_LIBRARIES}
)
