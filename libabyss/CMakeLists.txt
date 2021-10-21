cmake_minimum_required(VERSION 3.20)

project(libabyss C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

find_package(ZLIB REQUIRED)

if (WIN32)
    find_package(Threads REQUIRED)

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    set(THREAD_SRC src/threading_windows.c)
else ()
    set(THREAD_SRC src/threading_unix.c)
endif ()

add_library(${PROJECT_NAME}
        # Public
        src/libabyss.c
        src/utils.c
        src/mpq.c
        src/log.c
        src/mpqstream.c
        src/mpqblock.c
        src/palette.c
        src/dcc.c
        src/dc6.c
        # Private
        src/crypto.c
        src/blast.c
        src/compress.c
        src/streamreader.c
        src/streamwriter.c
        ${THREAD_SRC}
        )


set_target_properties(${PROJECT_NAME} PROPERTIES
        FOLDER "Libraries/"
        )


target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_link_libraries(${PROJECT_NAME}
        ZLIB::ZLIB
        ${CMAKE_THREAD_LIBS_INIT}
        )

install(TARGETS libabyss
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
