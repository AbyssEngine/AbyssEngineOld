cmake_minimum_required(VERSION 3.20)

# -------------------------------------------------------------------------------------------------
# Project
# -------------------------------------------------------------------------------------------------

project(abyss LANGUAGES CXX)


# -------------------------------------------------------------------------------------------------
# Source Files
# -------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME}
        src/casc.cpp src/cascstream.cpp src/mpq.cpp src/mpqstream.cpp src/palette.cpp src/dc6.cpp
        src/streamreader.cpp src/pngloader.cpp src/inifile.cpp src/abysssprite.cpp
        src/audiostream.cpp)


# -------------------------------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------------------------------

include(GNUInstallDirs)
configure_vcpkg()

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        FOLDER "Libraries/"
        )

# -------------------------------------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------------------------------------

find_package(PNG REQUIRED)
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE POSTPROC REQUIRED)
find_package(absl CONFIG REQUIRED)
include_stormlib()
include_casclib()


# -------------------------------------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CascLib_SOURCE_DIR}/src
        ${FFMPEG_INCLUDE_DIRS}
        )

# -------------------------------------------------------------------------------------------------
# Libraries
# -------------------------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        PNG::PNG
        stormlib::stormlib
        absl::str_format
        ${FFMPEG_LIBRARIES}
        )

# -------------------------------------------------------------------------------------------------
# Installation/Packaging
# -------------------------------------------------------------------------------------------------

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

if (WIN32)
    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            ws2_32
            ) # for CASC
endif ()
