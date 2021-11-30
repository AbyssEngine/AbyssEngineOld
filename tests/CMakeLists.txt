cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tests)

if(MSVC)
    add_definitions(-D_HAS_DEPRECATED_RESULT_OF=1)
endif()

find_package(GTest CONFIG REQUIRED)

add_executable(abyss_test
        bitstream_test.cpp
        mpq_test.cpp
)
target_link_libraries(abyss_test
        libabyss
         GTest::gmock_main
)

add_custom_target(check COMMAND abyss_test)
