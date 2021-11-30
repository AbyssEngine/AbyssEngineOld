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

find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)
find_library(STORMLIB storm)
if(STORMLIB)
    add_library(stormlib::stormlib UNKNOWN IMPORTED)
    set_target_properties(stormlib::stormlib PROPERTIES
        IMPORTED_LOCATION ${STORMLIB})
    message(STATUS "Using StormLib from ${STORMLIB}")
else()
    include(../cmake/CPM.cmake)
    CPMAddPackage("https://github.com/ladislav-zezula/StormLib.git#5ab093b7a57b8779dff06a08fac19d46c40b3329")
    add_library(stormlib::stormlib ALIAS storm)
    target_include_directories(storm INTERFACE ${StormLib_SOURCE_DIR})
endif()
find_package(absl CONFIG REQUIRED)
find_package(fmt REQUIRED)


add_library(${PROJECT_NAME}
        src/mpq.cpp
        src/mpqstream.cpp
        src/palette.cpp
        src/dc6.cpp
        src/streamreader.cpp
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Libraries/"
        )


target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ZLIB::ZLIB
        BZip2::BZip2
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
