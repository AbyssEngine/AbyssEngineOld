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
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "abt"
        FOLDER "Applications/"
        )

target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}
        )

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        libabyss
        )

if (WIN32)
    install(TARGETS ${PROJECT_NAME}
            RUNTIME_DEPENDENCIES
            PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
            POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
            DIRECTORIES C:/DevLibs/bin
            LIBRARY DESTINATION bin
            ARCHIVE DESTINATION bin
            )
else (WIN32)
    install(TARGETS ${PROJECT_NAME})
endif (WIN32)
