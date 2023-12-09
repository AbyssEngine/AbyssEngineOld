cmake_minimum_required(VERSION 3.15)
project(OpenDiablo2)

add_executable(OpenDiablo2 WIN32)

# -------------------------------------------------------------------------------------------------
# macOS DMG Disk Image Setup
# -------------------------------------------------------------------------------------------------
if (APPLE)
  set(CPACK_GENERATOR DragNDrop)
  set(CPACK_PACKAGE_NAME "OpenDiablo II")
  set(CPACK_PACKAGE_FILE_NAME "OpenDiablo2")
  set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/content/icon.icns")
  set(CPACK_DMG_VOLUME_NAME "OpenDiablo II")
  set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/content/dmg-background.png")
  set(CPACK_DMG_DS_STORE "${CMAKE_SOURCE_DIR}/content/icon.icns")
  set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/CMakeDMGSetup.scp")
endif ()
include(CPack)

# -------------------------------------------------------------------------------------------------
# Source Files
# -------------------------------------------------------------------------------------------------

target_sources(OpenDiablo2
        PUBLIC
        # Common
        Common/PaletteManager.h
        Common/ButtonDefManager.h
        Common/ResourcePaths.h
        Common/FontManager.h

        # Scenes
        Scenes/MainMenu/MainMenu.cpp Scenes/MainMenu/MainMenu.h
        Scenes/MainMenu/Logo.cpp Scenes/MainMenu/Logo.h

        # Main
        main.cpp
)


# -------------------------------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------------------------------

target_compile_options(OpenDiablo2 PRIVATE ${FFMPEG_DEFINITIONS})

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
# Enable universal binaries on macOS
# -------------------------------------------------------------------------------------------------
if (APPLE)
  set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version")
  set_target_properties(OpenDiablo2 PROPERTIES
          XCODE_ATTRIBUTE_ARCHS[sdk=macos*] "x86_64;arm64"
          XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO"
  )
endif ()

# -------------------------------------------------------------------------------------------------
# Installation Artifacts
# -------------------------------------------------------------------------------------------------
install(TARGETS OpenDiablo2
        BUNDLE DESTINATION . COMPONENT Runtime
        RUNTIME DESTINATION . COMPONENT Runtime
)

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
