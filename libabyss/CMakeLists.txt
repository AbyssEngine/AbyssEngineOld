cmake_minimum_required(VERSION 3.20)

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    set(VCPKG_BUILD_TYPE release)
endif()

project(libabyss CXX)

if(MSVC)
    add_definitions(-D_HAS_DEPRECATED_RESULT_OF=1)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
include(../cmake/CPM.cmake)

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_package(PNG REQUIRED)
find_library(STORMLIB storm)
if(STORMLIB)
    add_library(stormlib::stormlib UNKNOWN IMPORTED)
    set_target_properties(stormlib::stormlib PROPERTIES
        IMPORTED_LOCATION ${STORMLIB})
    message(STATUS "Using StormLib from ${STORMLIB}")
else()
    CPMAddPackage("https://github.com/ladislav-zezula/StormLib.git#5ab093b7a57b8779dff06a08fac19d46c40b3329")
    add_library(stormlib::stormlib ALIAS storm)
    target_include_directories(storm INTERFACE ${StormLib_SOURCE_DIR})
endif()
find_package(absl CONFIG REQUIRED)
find_package(fmt REQUIRED)

CPMAddPackage("https://github.com/ladislav-zezula/CascLib.git#85800c22281fe6827c61e8d9ebc9ca83d161d901")
file(GLOB_RECURSE CASC_SOURCES "${CascLib_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME}
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

        # TODO: make this a proper dependency, requires some packaging
        ${CASC_SOURCES}
        ${CascLib_SOURCE_DIR}/src/jenkins/lookup3.c)

set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Libraries/"
        )


target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CascLib_SOURCE_DIR}/src
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ZLIB::ZLIB
        BZip2::BZip2
        PNG::PNG
        stormlib::stormlib
        ${CMAKE_THREAD_LIBS_INIT}
        absl::str_format
        fmt::fmt
        )

install(TARGETS libabyss
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32) # for CASC
endif()
