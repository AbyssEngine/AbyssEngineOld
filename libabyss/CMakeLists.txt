cmake_minimum_required(VERSION 3.20)

project(libabyss C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME}
        src/libabyss.c include/libabyss/libabyss.h
        src/mpq.c include/libabyss/mpq.h
        src/log.c include/libabyss/log.h
        src/crypto.c src/crypto.h
        src/mpqstream.c include/libabyss/mpqstream.h
        src/mpqblock.c include/libabyss/mpqblock.h
        )


set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Libraries/"
        )

include_directories(
        ${CMAKE_SOURCE_DIR}/libabyss/include
)

install(TARGETS libabyss
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )
