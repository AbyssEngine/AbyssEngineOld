cmake_minimum_required(VERSION 3.20)

project(libabyss C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME}
        src/libabyss.c
        src/mpq.c
        src/log.c
        src/crypto.c
)

include_directories(
    ${CMAKE_SOURCE_DIR}/libabyss/include
)

install(TARGETS libabyss
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
