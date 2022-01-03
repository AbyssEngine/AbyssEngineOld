cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# -------------------------------------------------------------------------------------------------
# Abyss Project
# -------------------------------------------------------------------------------------------------

set(abyss_VERSION_MAJOR 0)
set(abyss_VERSION_MINOR 3)

project(abyss VERSION "${abyss_VERSION_MAJOR}.${abyss_VERSION_MINOR}")

# -------------------------------------------------------------------------------------------------
# CMake and  modules configuration
# -------------------------------------------------------------------------------------------------

set(ABYSS_REQUIRED_LUA 5.4)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_BINARY_DIR}")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(FindPackageMessage)
include(CPM)
include(ConfigureVCPKG)
include(Stormlib)
include(Casclib)
include(SDL2)
include(CMakeRC)
include(Homebrew)
include(Lualibs)

# -------------------------------------------------------------------------------------------------
# Build release mode by default
# -------------------------------------------------------------------------------------------------

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    set(VCPKG_BUILD_TYPE release)
endif ()


# -------------------------------------------------------------------------------------------------
# Compiler Configurations
# -------------------------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
#add_link_options(-fsanitize=address -fno-omit-frame-pointer)
if (NOT MSVC)
    add_compile_options(-Wall -Wextra)
endif()


# -------------------------------------------------------------------------------------------------
# Config.h file
# -------------------------------------------------------------------------------------------------

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)
include_directories("${PROJECT_BINARY_DIR}")


# -------------------------------------------------------------------------------------------------
# Platform Specific Configurations
# -------------------------------------------------------------------------------------------------

if (MSVC)
    add_definitions(-D_HAS_DEPRECATED_RESULT_OF=1)
endif ()


# -------------------------------------------------------------------------------------------------
# Dependencies
# -------------------------------------------------------------------------------------------------

configure_vcpkg()
configure_brew()
find_package(absl CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(cppzmq CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(FFMPEG COMPONENTS AVCODEC AVFORMAT AVUTIL SWSCALE SWRESAMPLE POSTPROC REQUIRED)
find_package(Lua ${ABYSS_REQUIRED_LUA} EXACT REQUIRED)
find_package(PNG REQUIRED)
find_package(ICU COMPONENTS uc REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(pango REQUIRED)
include_sdl2()
include_stormlib()
include_casclib()
include_lualibs()

if (APPLE)
    find_library(OSX_VIDEOTOOLBOX VideoToolbox)
    find_library(OSX_COREMEDIA CoreMedia)
    find_library(OSX_SECURITY Security)
endif ()


# -------------------------------------------------------------------------------------------------
# Include the projects
# -------------------------------------------------------------------------------------------------

add_subdirectory(libabyss)
add_subdirectory(apps)
add_subdirectory(tests)


# -------------------------------------------------------------------------------------------------
# Installation Packages
# -------------------------------------------------------------------------------------------------

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VENDOR "Abyss Project Contributors")
set(CPACK_PACKAGE_CONTACT "Tim Sarbin <tim.sarbin@gmail.com>")
set(CPACK_PACKAGE_VERSION_MAJOR "${abyss_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${abyss_VERSION_MINOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/package")
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
string(TOLOWER ${CMAKE_SYSTEM_NAME} _sys)
string(TOLOWER ${PROJECT_NAME} _project_lower)
set(CPACK_PACKAGE_FILE_NAME "${_project_lower}-${_sys}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${_project_lower}-${PROJECT_VERSION}")

install(FILES
        ${CPACK_RESOURCE_FILE_README}
        ${CPACK_RESOURCE_FILE_LICENSE}
        DESTINATION share/docs/${PROJECT_NAME}
        )

include(CPack)
