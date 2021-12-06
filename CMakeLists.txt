cmake_minimum_required(VERSION 3.20)

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


# -------------------------------------------------------------------------------------------------
# Platform Specific Configurations
# -------------------------------------------------------------------------------------------------

if (MSVC)
    add_definitions(-D_HAS_DEPRECATED_RESULT_OF=1)
endif ()




add_subdirectory(libabyss)
#add_subdirectory(abyssengine)
#add_subdirectory(abysstoolset)
#add_subdirectory(tests)

#
#find_package(ZLIB REQUIRED)
#find_package(BZip2 REQUIRED)
#find_package(PNG REQUIRED)
#find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE POSTPROC REQUIRED)
#find_library(STORMLIB storm)
#if (STORMLIB)
#    add_library(stormlib::stormlib UNKNOWN IMPORTED)
#    set_target_properties(stormlib::stormlib PROPERTIES
#            IMPORTED_LOCATION ${STORMLIB})
#    message(STATUS "Using StormLib from ${STORMLIB}")
#else ()
#    CPMAddPackage("https://github.com/ladislav-zezula/StormLib.git#5ab093b7a57b8779dff06a08fac19d46c40b3329")
#    add_library(stormlib::stormlib ALIAS storm)
#    target_include_directories(storm INTERFACE ${StormLib_SOURCE_DIR})
#endif ()
#find_package(absl CONFIG REQUIRED)
#find_package(fmt REQUIRED)
#
#find_package(SDL2 CONFIG REQUIRED)
#find_package(spdlog CONFIG REQUIRED)
#find_package(Lua 5.4 EXACT REQUIRED)
#find_package(sol2 CONFIG REQUIRED)
#find_package(Threads REQUIRED)
#find_package(sdl2-ttf)
#find_package(absl CONFIG REQUIRED)
#find_package(cppzmq CONFIG REQUIRED)
#
#if (NOT sdl2-ttf_FOUND)
#    find_package(PkgConfig REQUIRED)
#    pkg_check_modules(SDL2TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
#    add_library(SDL2::SDL2_ttf ALIAS PkgConfig::SDL2TTF)
#endif ()
#
#if (APPLE)
#    find_library(OSX_VIDEOTOOLBOX VideoToolbox)
#    find_library(OSX_COREMEDIA CoreMedia)
#    find_library(OSX_SECURITY Security)
#endif ()
#
#CPMAddPackage("https://github.com/ladislav-zezula/CascLib.git#85800c22281fe6827c61e8d9ebc9ca83d161d901")
#file(GLOB_RECURSE CASC_SOURCES "${CascLib_SOURCE_DIR}/src/*.cpp")
#
#
#if (APPLE)
#    find_library(OSX_VIDEOTOOLBOX VideoToolbox)
#    find_library(OSX_COREMEDIA CoreMedia)
#    find_library(OSX_SECURITY Security)
#endif ()
#



