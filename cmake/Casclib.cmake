cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

macro(include_casclib)
    message(STATUS "Including CascLib")
    CPMAddPackage("https://github.com/ladislav-zezula/CascLib.git#85800c22281fe6827c61e8d9ebc9ca83d161d901")
    file(GLOB_RECURSE CASC_SOURCES "${CascLib_SOURCE_DIR}/src/*.cpp")
    #set(CASC_SOURCES ${CASC_SOURCES} ${CascLib_SOURCE_DIR}/src/jenkins/lookup3.c)
endmacro()
