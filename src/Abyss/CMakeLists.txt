cmake_minimum_required(VERSION 3.15)

add_library(Abyss
        # 3rdParty
        ${CMAKE_SOURCE_DIR}/3rdParty/Blast/blast.c

        # ImGui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp

        # TODO: depend on it properly
        #${CASC_SOURCES}
)

target_sources(Abyss
        PUBLIC
        FILE_SET CXX_MODULES FILES
        Common/Animation.ixx
        Common/CommandLineOpts.ixx
        Common/Configuration.ixx
        Common/Huffman.ixx
        Common/Logging.ixx
        Common/MouseProvider.ixx
        Common/MouseState.ixx
        Common/RendererProvider.ixx
        Common/RingBuffer.ixx
        Common/Scene.ixx
        Common/SoundEffectProvider.ixx
        Common/WavDecompress.ixx

        Concepts/Drawable.ixx
        Concepts/FontRenderer.ixx

        DataTypes/DC6Frame.ixx
        DataTypes/DC6.ixx
        DataTypes/Palette.ixx

        Enums/BlendMode.ixx
        Enums/MouseButton.ixx

        FileSystem/Provider.ixx
        FileSystem/InputStream.ixx
        FileSystem/MPQ.ixx
        #FileSystem/CASC.ixx
        FileSystem/FileLoader.ixx

        Streams/AudioStream.ixx
        Streams/SoundEffect.ixx
        Streams/StreamReader.ixx
        Streams/VideoStream.ixx

        UI/Button.ixx
        UI/ButtonDef.ixx
        UI/Label.ixx
        UI/SpriteFont.ixx

        Singletons.ixx
        AbyssEngine.ixx
)

target_include_directories(Abyss
        PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

target_include_directories(Abyss
  PRIVATE
  ${CascLib_SOURCE_DIR}/src
)

target_compile_features(Abyss PUBLIC cxx_std_20)
target_link_libraries(Abyss
        PUBLIC
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main> $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
        spdlog::spdlog
        cxxopts::cxxopts
        ZLIB::ZLIB
        stormlib::stormlib
        ${FFMPEG_LIBRARIES}
        ${OSX_VIDEOTOOLBOX}
        ${OSX_COREMEDIA}
        ${OSX_SECURITY}
)

