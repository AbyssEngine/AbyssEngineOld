cmake_minimum_required(VERSION 3.15)

add_library(Abyss
        Common/Animation.hpp
        Common/Button.hpp
        Common/CommandLineOpts.hpp
        Common/Configuration.hpp
        Common/Drawable.hpp
        Common/FileProvider.hpp
        Common/Huffman.hpp
        Common/Logging.hpp
        Common/MouseState.hpp
        Common/RendererProvider.hpp
        Common/RingBuffer.hpp
        Common/Scene.hpp
        Common/SpriteFont.hpp
        Common/WavDecompress.hpp

        DataTypes/DC6.cpp DataTypes/DC6.hpp
        DataTypes/Palette.hpp

        Enums/BlendMode.hpp
        Enums/MouseButton.hpp

        MPQ/BlockEntry.cpp MPQ/BlockEntry.h
        MPQ/Crypto.cpp MPQ/Crypto.h
        MPQ/File.cpp MPQ/File.h
        MPQ/HashEntry.cpp MPQ/HashEntry.h
        MPQ/Header.cpp MPQ/Header.h

        Streams/AudioStream.cpp Streams/AudioStream.h
        Streams/InputStream.cpp Streams/InputStream.h
        Streams/StreamReader.cpp Streams/StreamReader.h
        Streams/MPQStream.cpp Streams/MPQStream.h

        Singletons.hpp
        AbyssEngine.cpp AbyssEngine.h

        # 3rdParty
        ${CMAKE_SOURCE_DIR}/3rdParty/Blast/blast.c

        # ImGui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
        Common/MouseProvider.hpp
)

target_include_directories(Abyss
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_compile_features(Abyss PUBLIC cxx_std_20)
target_link_libraries(Abyss
        PUBLIC
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        spdlog::spdlog
        cxxopts::cxxopts
        ZLIB::ZLIB
        ${FFMPEG_LIBRARIES}
        ${OSX_VIDEOTOOLBOX}
        ${OSX_COREMEDIA}
        ${OSX_SECURITY}
)

