cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

macro(include_stormlib)
    message(STATUS "Including StormLib")

    find_library(STORMLIB storm)
    if (STORMLIB)
        message(STATUS "Found StormLib on the local system")
        add_library(stormlib::stormlib UNKNOWN IMPORTED)
        set_target_properties(stormlib::stormlib PROPERTIES
                IMPORTED_LOCATION ${STORMLIB})
        message(STATUS "Using StormLib from ${STORMLIB}")
    else ()
        message(STATUS "No StormLib found on the local system, pulling from CPM")
        CPMAddPackage("https://github.com/ladislav-zezula/StormLib.git#5ab093b7a57b8779dff06a08fac19d46c40b3329")
        add_library(stormlib::stormlib ALIAS storm)
        target_include_directories(storm INTERFACE ${StormLib_SOURCE_DIR})
    endif ()
endmacro()