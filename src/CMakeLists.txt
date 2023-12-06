cmake_minimum_required(VERSION 3.15)

project(AbyssEngine)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  include(gcc_modules.cmake)
endif ()

add_subdirectory(Abyss)
add_subdirectory(OD2)
