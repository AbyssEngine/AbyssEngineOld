cmake_minimum_required(VERSION 3.20)
project(Abyss)

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
else ()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

find_package(SDL2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cxxopts CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE REQUIRED)

if (APPLE)
  find_library(OSX_VIDEOTOOLBOX VideoToolbox)
  find_library(OSX_COREMEDIA CoreMedia)
  find_library(OSX_SECURITY Security)
endif ()

add_subdirectory(src)



