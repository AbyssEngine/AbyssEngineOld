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


add_executable(Abyss
        # Abyss Engine
        src/Abyss/Common/CommandLineOpts.cpp
        src/Abyss/Common/Configuration.cpp
        src/Abyss/Common/Logging.cpp
        src/Abyss/Common/MouseState.cpp
        src/Abyss/Common/WavDecompress.cpp
        src/Abyss/Common/Huffman.cpp
        src/Abyss/DataTypes/DC6.cpp
        src/Abyss/DataTypes/Palette.cpp
        src/Abyss/MPQ/BlockEntry.cpp
        src/Abyss/MPQ/Crypto.cpp
        src/Abyss/MPQ/File.cpp
        src/Abyss/MPQ/HashEntry.cpp
        src/Abyss/MPQ/Header.cpp
        src/Abyss/Streams/AudioStream.cpp
        src/Abyss/Streams/InputStream.cpp
        src/Abyss/Streams/StreamReader.cpp
        src/Abyss/Streams/MPQStream.cpp
        src/Abyss/AbyssEngine.cpp

        # OpenDiablo 2
        src/OD2/Startup.cpp
        src/OD2/Common/PaletteManager.cpp
        src/OD2/Scenes/MainMenu/MainMenu.cpp
        src/OD2/Scenes/MainMenu/Logo.cpp

        # Extras
        src/Extras/Blast/blast.c
        src/Extras/Imgui/imgui_impl_sdl2.cpp
        src/Extras/Imgui/imgui_impl_sdlrenderer2.cpp

        # Main
        src/main.cpp
        src/OD2/Common/ButtonDefManager.cpp
        src/OD2/Common/ButtonDefManager.h
)

target_include_directories(Abyss
        PRIVATE
        src/Extras/Blast src/Extras/Imgui src/
        ${FFMPEG_INCLUDE_DIRS}
)
target_compile_features(Abyss PUBLIC cxx_std_20)
target_link_libraries(Abyss
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        spdlog::spdlog
        cxxopts::cxxopts
        ZLIB::ZLIB
        imgui::imgui
        ${FFMPEG_LIBRARIES}
)

