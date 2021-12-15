cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------------------------------------------------------------------------
# Project
# -------------------------------------------------------------------------------------------------

SET(APP_NAME libabyss)


# -------------------------------------------------------------------------------------------------
# Source Files
# -------------------------------------------------------------------------------------------------

add_library(${APP_NAME}
        src/pngloader.cpp

        src/formats/d2/casc.cpp
        src/formats/d2/mpq.cpp
        src/formats/d2/palette.cpp
        src/formats/d2/dc6.cpp
        src/formats/d2/dt1.cpp
        src/formats/d2/ds1.cpp

        src/formats/abyss/inifile.cpp
        src/formats/abyss/abysssprite.cpp

        src/streams/cascstream.cpp
        src/streams/mpqstream.cpp
        src/streams/streamreader.cpp
        src/streams/audiostream.cpp

        src/common/ringbuffer.cpp

        src/zone/zone.cpp
        src/zone/zonetile.cpp

        # Strange CASClib inclusion
        # TODO: depend on it properly
        ${CASC_SOURCES}
        )

# -------------------------------------------------------------------------------------------------
# Configuration
# -------------------------------------------------------------------------------------------------

include(GNUInstallDirs)
configure_vcpkg()
set_target_properties(${APP_NAME} PROPERTIES FOLDER "Libraries/")
target_compile_features(${APP_NAME} PUBLIC cxx_std_20)


# -------------------------------------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------------------------------------

target_include_directories(${APP_NAME}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE
        ${CascLib_SOURCE_DIR}/src
        ${FFMPEG_INCLUDE_DIRS}
        )


# -------------------------------------------------------------------------------------------------
# Libraries
# -------------------------------------------------------------------------------------------------

target_link_libraries(${APP_NAME}
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

install(TARGETS ${APP_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

if (WIN32)
    target_link_libraries(${APP_NAME} PRIVATE ws2_32) # for CASC
endif ()

target_compile_features(${APP_NAME} PRIVATE cxx_std_20)
