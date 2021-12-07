cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# -------------------------------------------------------------------------------------------------
# Project
# -------------------------------------------------------------------------------------------------

project(abyss LANGUAGES CXX)


# -------------------------------------------------------------------------------------------------
# Source Files
# -------------------------------------------------------------------------------------------------

add_library(${PROJECT_NAME}
        src/casc.cpp include/libabyss/casc.h
        src/cascstream.cpp include/libabyss/cascstream.h
        src/mpq.cpp include/libabyss/mpq.h
        src/mpqstream.cpp include/libabyss/mpqstream.h
        src/palette.cpp include/libabyss/palette.h
        src/dc6.cpp include/libabyss/dc6.h
        src/streamreader.cpp include/libabyss/streamreader.h
        src/pngloader.cpp
        src/inifile.cpp include/libabyss/inifile.h
        src/abysssprite.cpp include/libabyss/abysssprite.h
        src/audiostream.cpp include/libabyss/audiostream.h
        )

# -------------------------------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------------------------------

include(GNUInstallDirs)
configure_vcpkg()
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Libraries/")

# -------------------------------------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------------------------------------

find_package(PNG REQUIRED)
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE POSTPROC REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
include_stormlib()
include_casclib()


# -------------------------------------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------------------------------------

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CascLib_SOURCE_DIR}/src
        ${FFMPEG_INCLUDE_DIRS}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

# -------------------------------------------------------------------------------------------------
# Libraries
# -------------------------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        PNG::PNG
        stormlib::stormlib
        absl::str_format
        fmt::fmt
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
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32) # for CASC
endif ()
