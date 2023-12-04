cmake_minimum_required(VERSION 3.15)

add_library(Abyss
        Common/CommandLineOpts.cpp
        Common/Configuration.cpp
        Common/Logging.cpp
        Common/MouseState.cpp
        Common/WavDecompress.cpp
        Common/Huffman.cpp
        DataTypes/DC6.cpp
        DataTypes/Palette.cpp
        MPQ/BlockEntry.cpp
        MPQ/Crypto.cpp
        MPQ/File.cpp
        MPQ/HashEntry.cpp
        MPQ/Header.cpp
        Streams/AudioStream.cpp
        Streams/InputStream.cpp
        Streams/StreamReader.cpp
        Streams/MPQStream.cpp
        AbyssEngine.cpp
        Extras/Blast/blast.c
        Extras/Imgui/imgui_impl_sdl2.cpp
        Extras/Imgui/imgui_impl_sdlrenderer2.cpp
)

target_include_directories(Abyss
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_features(Abyss PUBLIC cxx_std_20)
target_link_libraries(Abyss
        PUBLIC
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        spdlog::spdlog
        cxxopts::cxxopts
        ZLIB::ZLIB
        imgui::imgui
        ${FFMPEG_LIBRARIES}
)

