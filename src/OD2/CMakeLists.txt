cmake_minimum_required(VERSION 3.15)


add_executable(OpenDiablo2 WIN32
        main.ixx
)

target_sources(OpenDiablo2
        PUBLIC
        FILE_SET CXX_MODULES FILES
        Startup.ixx
        Common/PaletteManager.ixx
        Scenes/MainMenu/MainMenu.ixx
        Scenes/MainMenu/Logo.ixx
        Common/ButtonDefManager.ixx
        Common/ResourcePaths.ixx
        main.ixx
)

if (WIN32)
  target_compile_options(OpenDiablo2 PRIVATE "/SUBSYSTEM:WINDOWS")
endif ()


target_include_directories(OpenDiablo2
        PRIVATE
        src/Extras/Blast src/Extras/Imgui src/
        ${FFMPEG_INCLUDE_DIRS}
)

target_compile_features(OpenDiablo2 PUBLIC cxx_std_20)
target_link_libraries(OpenDiablo2
        PRIVATE
        Abyss
)
