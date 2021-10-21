cmake_minimum_required(VERSION 3.20)

project(abyss C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#set(VCPKG_LIBRARY_LINKAGE static)

# GoogleTest requires at least C++11
#set(CMAKE_CXX_STANDARD 11)

#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)
#
## For Windows: Prevent overriding the parent project's compiler/linker settings
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)

#add_compile_options(-fsanitize=memory)
#add_link_options(-fsanitize=memory)


set(${PROJECT_NAME}_VERSION_MAJOR 0)
set(${PROJECT_NAME}_VERSION_MINOR 1)

#set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

add_subdirectory(libabyss)
add_subdirectory(abyssengine)
add_subdirectory(abysstoolset)
