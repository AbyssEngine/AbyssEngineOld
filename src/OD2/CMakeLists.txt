cmake_minimum_required(VERSION 3.15)
project(OpenDiablo2)

add_executable(OpenDiablo2 WIN32
        main.ixx
)

# -------------------------------------------------------------------------------------------------
# Source Files
# -------------------------------------------------------------------------------------------------

target_sources(OpenDiablo2
        PUBLIC
        FILE_SET CXX_MODULES FILES
        Startup.ixx
        Common/PaletteManager.ixx
        Common/ButtonDefManager.ixx
        Common/ResourcePaths.ixx
        Common/FontManager.ixx
        Scenes/MainMenu/MainMenu.ixx
        Scenes/MainMenu/Logo.ixx
        main.ixx
)


# -------------------------------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------------------------------

target_compile_options(OpenDiablo2 PRIVATE ${FFMPEG_DEFINITIONS})

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


# -------------------------------------------------------------------------------------------------
# Make sure the EXE and libraries all build/copy to the same folder on Windows
# -------------------------------------------------------------------------------------------------
if (WIN32)
  add_custom_command(TARGET OpenDiablo2 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/content ${CMAKE_BINARY_DIR})
  add_custom_command(TARGET OpenDiablo2 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/bin ${CMAKE_BINARY_DIR})
endif (WIN32)

# -------------------------------------------------------------------------------------------------
# macOS Application Bundle
# -------------------------------------------------------------------------------------------------
if (APPLE)
  file(COPY "${CMAKE_SOURCE_DIR}/content/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/OpenDiablo2.app/Contents/Resources")
  set_target_properties(OpenDiablo2 PROPERTIES
          MACOSX_BUNDLE TRUE
          MACOSX_BUNDLE_BUNDLE_NAME "OpenDiablo 2"
          MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
          MACOSX_BUNDLE_GUI_IDENTIFIER "com.abyssengine.opendiablo2"
          MACOSX_BUNDLE_ICON_FILE "icon.icns"
          MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in"
          MACOSX_BUNDLE_COPYRIGHT "(c) 2023 Krathic Studios, LLC"
  )

  set_source_files_properties("${CMAKE_SOURCE_DIR}/content/icon.icns" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Install path prefix, prepended onto install directories" FORCE)
endif ()
