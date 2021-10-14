cmake_minimum_required(VERSION 3.20)

project(libabyss C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(ZLIB REQUIRED)

add_library(${PROJECT_NAME}
        src/libabyss.c include/libabyss/libabyss.h
        src/mpq.c include/libabyss/mpq.h
        src/log.c include/libabyss/log.h
        src/crypto.c src/crypto.h
        src/mpqstream.c include/libabyss/mpqstream.h
        src/mpqblock.c include/libabyss/mpqblock.h
        src/blast.c src/blast.h
        src/compress.c src/compress.h
        src/streamreader.c src/streamreader.h
        src/streamwriter.c src/streamwriter.h
        )


set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Libraries/"
        )

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
        ZLIB::ZLIB
        )

install(TARGETS libabyss)
