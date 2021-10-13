cmake_minimum_required(VERSION 3.20)

project(abysstoolset C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME})

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)

target_sources(${PROJECT_NAME}
    PRIVATE
        src/abt.c
        src/help.c src/help.h
        src/mpqtool.c src/mpqtool.h
        src/common.c src/common.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "abt"
        FOLDER "Applications/"
        )

include_directories(
        ${PROJECT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/libabyss/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        libabyss
)


