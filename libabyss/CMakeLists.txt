cmake_minimum_required(VERSION 3.20)

project(libabyss C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

find_package(ZLIB REQUIRED)

if (WIN32)
    find_package(Threads REQUIRED)

    set(THREADS_PREFER_PTHREAD_FLAG ON)
    set(THREAD_SRC src/threading_windows.c include/libabyss/threading.h)
else ()
    set(THREAD_SRC src/threading_unix.c include/libabyss/threading.h)
endif ()

add_library(${PROJECT_NAME}
        # Public
        src/libabyss.c include/libabyss/libabyss.h
        src/utils.c include/libabyss/utils.h
        src/mpq.c include/libabyss/mpq.h
        src/log.c include/libabyss/log.h
        src/mpqstream.c include/libabyss/mpqstream.h
        src/mpqblock.c include/libabyss/mpqblock.h
        src/palette.c include/libabyss/palette.h
        src/dcc.c include/libabyss/dcc.h
        src/dc6.c include/libabyss/dc6.h
        include/libabyss/boundsdefs.h
        # Private
        src/crypto.c src/crypto.h
        src/blast.c src/blast.h
        src/compress.c src/compress.h
        src/streamreader.c src/streamreader.h
        src/streamwriter.c src/streamwriter.h
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
