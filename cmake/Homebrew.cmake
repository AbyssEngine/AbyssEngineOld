function(configure_brew)
    if(APPLE)
        find_program(brew brew)
        if(brew)
            find_package_message(brew "Homebrew found: ${brew}" "1;${brew}")
        else()
            find_package_message(brew "Homebrew not found" "0")
        endif()

        if(brew)
            if (NOT ICU_ROOT AND NOT DEFINED ENV{ICU_ROOT})
                execute_process(COMMAND "${brew}" --prefix icu4c
                    RESULT_VARIABLE brew_icu_f
                    OUTPUT_VARIABLE brew_icu OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
                if(brew_icu_f EQUAL 0)
                    find_package_message(brew_icu "ICU via Homebrew: ${brew_icu}" "${brew_icu}")
                    SET(ICU_ROOT "${brew_icu}" PARENT_SCOPE)
                endif()
            endif()

            execute_process(COMMAND "${brew}" --prefix pango
                RESULT_VARIABLE brew_f
                OUTPUT_VARIABLE brew_out OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            if (brew_f EQUAL 0)
                file(GLOB dirs LIST_DIRECTORIES true CONFIGURE_DEPENDS "${brew_out}/../*")
                foreach(dir IN LISTS dirs)
                    set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${dir}/lib/pkgconfig")
                endforeach()
                message(STATUS "Set PKG_CONFIG_PATH to $ENV{PKG_CONFIG_PATH}")
            else()
                message(STATUS "Could not execute brew")
            endif()
        endif()
    endif()
endfunction()
