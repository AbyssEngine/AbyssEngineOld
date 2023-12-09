cmake_minimum_required(VERSION 3.20)

project(Abyss)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, defaulting to Release")
  set(CMAKE_BUILD_TYPE Release)
endif ()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(CPM)
include(Stormlib)
include(Casclib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE REQUIRED)
CPMAddPackage(NAME imgui VERSION 1.90 DOWNLOAD_ONLY YES
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.zip
        URL_HASH SHA256=fbe086f5bfc631db509acea851734f6996d6d2ef8fdeacb3f097f6def21e8af8)
include_stormlib()
#include_casclib()

if (APPLE)
  find_library(OSX_VIDEOTOOLBOX VideoToolbox)
  find_library(OSX_COREMEDIA CoreMedia)
  find_library(OSX_SECURITY Security)
endif ()

add_subdirectory(src)



