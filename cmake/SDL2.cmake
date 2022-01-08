cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

macro(include_sdl2)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(sdl2-ttf)
endmacro()
