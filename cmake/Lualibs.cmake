function(include_lualibs)
    find_package(PkgConfig)
    set(lua_so_dir)
    if (PKG_CONFIG_FOUND)
        pkg_get_variable(lua_so_dir luajit INSTALL_CMOD)
    endif()

    function(add_lua_library lib version)
        find_library(LUA_LIB_${lib} NAMES ${lib} ${lib}.so PATHS ${lua_so_dir})
        if (LUA_LIB_${lib})
            message(STATUS "Found Lua library ${lib}: ${LUA_LIB_${lib}}")
            add_library(LUA::${lib} IMPORTED UNKNOWN)
            set_target_properties(LUA::${lib} PROPERTIES IMPORTED_LOCATION ${LUA_LIB_${lib}})
        else()
            message(STATUS "Using Lua library ${lib} from Luarocks")
            find_program(LUAROCKS luarocks REQUIRED)
            find_program(LUA_BIN NAMES luajit lua REQUIRED)
            file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lua_deps)
            if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/lua_deps/${lib}-${version})
                message(STATUS "Downloading Lua library ${lib}")
                execute_process(COMMAND ${LUAROCKS} unpack ${lib} ${version}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lua_deps
                    COMMAND_ERROR_IS_FATAL ANY)
            endif()
            execute_process(COMMAND ${LUA_BIN} ${CMAKE_SOURCE_DIR}/cmake/add_lua_library.lua
                ${CMAKE_CURRENT_BINARY_DIR}/lua_deps ${lib} ${version}
                OUTPUT_VARIABLE code
                COMMAND_ERROR_IS_FATAL ANY)
            #message(STATUS "|||${code}|||")
            cmake_language(EVAL CODE "${code}")
        endif()

    endfunction()

    add_lua_library(lpeg 1.0.2-1)
    add_lua_library(lsqlite3 0.9.5-1)
endfunction()
