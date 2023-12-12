cmake_minimum_required(VERSION 3.15)

add_library(Abyss
        # ImGui
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp

        # TODO: depend on it properly
        ${CASC_SOURCES}

        Common/Animation.h
        Common/CommandLineOpts.cpp Common/CommandLineOpts.h
        Common/Configuration.cpp Common/Configuration.h
        Common/Logging.h
        Common/MouseProvider.h
        Common/MouseState.cpp Common/MouseState.h
        Common/RendererProvider.h
        Common/RingBuffer.h
        Common/Scene.h
        Common/SoundEffectProvider.h
        Common/WavDecompress.cpp Common/WavDecompress.h

        Concepts/Drawable.h
        Concepts/FontRenderer.h

        DataTypes/DC6.cpp DataTypes/DC6.h
        DataTypes/DC6Frame.cpp DataTypes/DC6Frame.h
        DataTypes/DT1.cpp DataTypes/DT1.h
        DataTypes/Palette.cpp DataTypes/Palette.h

        Enums/BlendMode.h
        Enums/MouseButton.h

        FileSystem/Provider.h
        FileSystem/InputStream.cpp FileSystem/InputStream.h
        FileSystem/Direct.cpp FileSystem/Direct.h
        FileSystem/MPQ.cpp FileSystem/MPQ.h
        FileSystem/CASC.cpp FileSystem/CASC.h
        FileSystem/FileLoader.cpp FileSystem/FileLoader.h

        Streams/AudioStream.cpp Streams/AudioStream.h
        Streams/SoundEffect.cpp Streams/SoundEffect.h
        Streams/StreamReader.cpp Streams/StreamReader.h
        Streams/VideoStream.cpp Streams/VideoStream.h

        UI/Button.h
        UI/ButtonDef.h
        UI/Label.cpp UI/Label.h
        UI/SpriteFont.h

        Singletons.cpp Singletons.h
        AbyssEngine.cpp AbyssEngine.h
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
        absl::flat_hash_map
        ${FFMPEG_LIBRARIES}
        ${OSX_VIDEOTOOLBOX}
        ${OSX_COREMEDIA}
        ${OSX_SECURITY}
)

