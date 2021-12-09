cmake_minimum_required(VERSION 3.20)

set(APP_NAME abysstoolset)

find_package(spdlog CONFIG REQUIRED)
find_package(argh CONFIG REQUIRED)

add_executable(${APP_NAME})

if (MSVC)
    add_definitions(-D_HAS_DEPRECATED_RESULT_OF=1)
endif (MSVC)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE -DSPDLOG_DEBUG_ON -DSPDLOG_TRACE_ON)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

configure_file(
        ${CMAKE_SOURCE_DIR}/config.h.in
        ${PROJECT_BINARY_DIR}/config.h
)

target_sources(${APP_NAME}
        PRIVATE
        src/abt.cpp
        )

set_target_properties(${APP_NAME} PROPERTIES
        OUTPUT_NAME "abt"
        FOLDER "Applications/"
        )

target_include_directories(${APP_NAME} PRIVATE
        ${PROJECT_BINARY_DIR}
        )

target_link_libraries(${APP_NAME}
        PRIVATE
        libabyss
        spdlog::spdlog_header_only
        argh
        )

install(TARGETS ${APP_NAME})
