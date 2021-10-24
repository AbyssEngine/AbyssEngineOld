cmake_minimum_required(VERSION 3.20)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif ()

project(abyss C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#set(VCPKG_LIBRARY_LINKAGE static)

#set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)

add_subdirectory(libabyss)
add_subdirectory(abyssengine)
add_subdirectory(abysstoolset)

option(PACKAGE_TESTS "Build the tests" ON)
if (PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif ()
