cmake_minimum_required(VERSION 3.20)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    set(VCPKG_BUILD_TYPE release)
endif ()

set(VCPKG_LIBRARY_LINKAGE dynamic)
set(VCPKG_BUILD_TYPE release)

if (MSVC)
    # TODO: remove this when depending on CASC
    add_definitions(-DCASCLIB_NO_AUTO_LINK_LIBRARY)
endif (MSVC)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif ()

project(abyss)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)

add_subdirectory(libabyss)
add_subdirectory(abyssengine)
add_subdirectory(abysstoolset)
add_subdirectory(tests)

