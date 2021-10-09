cmake_minimum_required(VERSION 3.20)

project(abyss)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_C_FLAGS "-fPIE")

set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)

add_subdirectory(libabyss)
add_subdirectory(abyssengine)
add_subdirectory(abysstoolset)
