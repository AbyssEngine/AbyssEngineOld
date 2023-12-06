cmake_minimum_required(VERSION 3.27)

project(Abyss)


# If using clang, require version 16 or higher
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16)
    message(FATAL_ERROR "Clang version must be at least 16!\nClang version: ${CMAKE_CXX_COMPILER_VERSION}\nClang path: ${CMAKE_CXX_COMPILER}\nhttps://www.kitware.com/import-cmake-the-experiment-is-over/")
  endif ()
endif ()

# If using gcc, it must be version 14 or higher
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 14)
    message(FATAL_ERROR "GCC version must be at least 14!\nGCC version: ${CMAKE_CXX_COMPILER_VERSION}\nGCC path: ${CMAKE_CXX_COMPILER}\nhttps://www.kitware.com/import-cmake-the-experiment-is-over/")
  endif ()
endif ()

# If using visual studio, must be MSVC 14.34 toolset or higher
if (MSVC)
  if (MSVC_TOOLSET_VERSION VERSION_LESS 14.34)
    message(FATAL_ERROR "MSVC toolset version must be at least 14.34!\nMSVC toolset version: ${MSVC_TOOLSET_VERSION}\nhttps://www.kitware.com/import-cmake-the-experiment-is-over/")
  endif ()
endif ()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include(CPM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE REQUIRED)
CPMAddPackage(NAME imgui VERSION 1.90 DOWNLOAD_ONLY YES
        URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.zip
        URL_HASH SHA256=fbe086f5bfc631db509acea851734f6996d6d2ef8fdeacb3f097f6def21e8af8)

if (APPLE)
  find_library(OSX_VIDEOTOOLBOX VideoToolbox)
  find_library(OSX_COREMEDIA CoreMedia)
  find_library(OSX_SECURITY Security)
endif ()

add_subdirectory(src)



