cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(tests CXX)

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable(abyss_test
        bitstream_test.cpp
        mpq_test.cpp
        )
target_link_libraries(abyss_test
        abyss
        GTest::gmock_main
        stormlib::stormlib
        fmt::fmt
        )

add_custom_target(check COMMAND abyss_test)
