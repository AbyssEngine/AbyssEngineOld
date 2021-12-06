cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

macro(configure_vcpkg)
    message(STATUS "Configuring VCPKG")
    set(VCPKG_LIBRARY_LINKAGE dynamic)
    set(VCPKG_BUILD_TYPE release)

    if (DEFINED CMAKE_TOOLCHAIN_FILE)
        message(STATUS "Using toolchain file: " ${CMAKE_TOOLCHAIN_FILE})
    elseif (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
        message(STATUS "Auto-detected toolchain file: " ${CMAKE_TOOLCHAIN_FILE})
    endif ()
endmacro()