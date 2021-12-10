cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# -------------------------------------------------------------------------------------------------
# Project
# -------------------------------------------------------------------------------------------------

SET(APP_NAME libabyss)


# -------------------------------------------------------------------------------------------------
# Source Files
# -------------------------------------------------------------------------------------------------

add_library(${APP_NAME}
        src/casc.cpp
        src/cascstream.cpp
        src/mpq.cpp
        src/mpqstream.cpp
        src/palette.cpp
        src/dc6.cpp
        src/streamreader.cpp
        src/pngloader.cpp
        src/inifile.cpp
        src/abysssprite.cpp
        src/audiostream.cpp
        src/ringbuffer.cpp

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
