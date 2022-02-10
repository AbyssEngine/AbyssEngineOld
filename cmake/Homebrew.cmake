macro(configure_brew)
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
                    SET(ICU_ROOT "${brew_icu}")
                endif()
            endif()

            execute_process(COMMAND "${brew}" --prefix pango
                RESULT_VARIABLE brew_pango_f
                OUTPUT_VARIABLE brew_pango OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            if(brew_pango_f EQUAL 0)
                find_package_message(brew_pango "Pango via Homebrew: ${brew_pango}" "${brew_pango}")
                set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${brew_pango}/lib/pkgconfig")
            endif()

            execute_process(COMMAND "${brew}" --prefix pangommmm
                RESULT_VARIABLE brew_pangomm_f
                OUTPUT_VARIABLE brew_pangomm OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            if(brew_pangomm_f EQUAL 0)
                find_package_message(brew_pangomm "Pangomm via Homebrew: ${brew_pangomm}" "${brew_pangomm}")
                set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${brew_pangomm}/lib/pkgconfig")
            endif()
        endif()
    endif()
endmacro()
