cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# -------------------------------------------------------------------------------------------------
# Abyss Project
# -------------------------------------------------------------------------------------------------

project(abyss)

set(ABYSS_VERSION_MAJOR 0)
set(ABYSS_VERSION_MINOR 2)


# -------------------------------------------------------------------------------------------------
# CMake and  modules configuration
# -------------------------------------------------------------------------------------------------

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(${CMAKE_SOURCE_DIR}/cmake/CPM.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/ConfigureVCPKG.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Stormlib.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/Casclib.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/SDL2.cmake)


# -------------------------------------------------------------------------------------------------
# Build release mode by default
# -------------------------------------------------------------------------------------------------

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    set(VCPKG_BUILD_TYPE release)
endif ()


# -------------------------------------------------------------------------------------------------
# Compiler Configurations
# -------------------------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
#add_link_options(-fsanitize=address -fno-omit-frame-pointer)


# -------------------------------------------------------------------------------------------------
# Config.h file
# -------------------------------------------------------------------------------------------------

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)


# -------------------------------------------------------------------------------------------------
# Platform Specific Configurations
# -------------------------------------------------------------------------------------------------

if (MSVC)
    add_definitions(-D_HAS_DEPRECATED_RESULT_OF=1)
endif ()


# -------------------------------------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------------------------------------

configure_vcpkg()
find_package(absl CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE POSTPROC REQUIRED)
find_package(Lua 5.4 EXACT REQUIRED)
find_package(PNG REQUIRED)
include_sdl2()
include_stormlib()
include_casclib()

if (APPLE)
    find_library(OSX_VIDEOTOOLBOX VideoToolbox)
    find_library(OSX_COREMEDIA CoreMedia)
    find_library(OSX_SECURITY Security)
endif ()


# -------------------------------------------------------------------------------------------------
# Include the projects
# -------------------------------------------------------------------------------------------------

add_subdirectory(libabyss)
add_subdirectory(apps)
add_subdirectory(tests)

