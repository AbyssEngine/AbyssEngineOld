cmake_minimum_required(VERSION 3.15)


add_executable(OpenDiablo2 WIN32
        Startup.cpp
        Common/PaletteManager.cpp
        Scenes/MainMenu/MainMenu.cpp
        Scenes/MainMenu/Logo.cpp
        main.cpp
        Common/ButtonDefManager.cpp
        Common/ButtonDefManager.h
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
