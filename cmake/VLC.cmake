macro(include_libvlc)
    add_library(VLC::VLC IMPORTED UNKNOWN)
    if(MSVC)
        # TODO This should not be hardcoded...
        message(STATUS "Using LibVLC from ${CMAKE_SOURCE_DIR}/libvlc/VideoLAN.LibVLC.Windows.3.0.16/build/x64/libvlc.lib and hoping it's there")
        set_target_properties(VLC::VLC PROPERTIES
            IMPORTED_LOCATION
            "${CMAKE_SOURCE_DIR}/libvlc/VideoLAN.LibVLC.Windows.3.0.16/build/x64/libvlc.lib"
            )
        include_directories("${CMAKE_SOURCE_DIR}/libvlc/VideoLAN.LibVLC.Windows.3.0.16/build/x64/include")
        install(DIRECTORY libvlc/VideoLAN.LibVLC.Windows.3.0.16/build/x64/plugins DESTINATION ${CMAKE_INSTALL_BINDIR})

        file(COPY "${CMAKE_SOURCE_DIR}/libvlc/VideoLAN.LibVLC.Windows.3.0.16/build/x64/libvlc.dll"
            "${CMAKE_SOURCE_DIR}/libvlc/VideoLAN.LibVLC.Windows.3.0.16/build/x64/libvlccore.dll"
            DESTINATION "D:/a/AbyssEngine/AbyssEngine/builds/ninja-multi-vcpkg/apps/abyssengine/Debug")
        endif()
    if(UNIX)
        if(APPLE)
            message(STATUS "Using LibVLC from ${CMAKE_SOURCE_DIR}/libvlc/lib/libvlc.dylib and hoping it's there")
            set_target_properties(VLC::VLC PROPERTIES
                IMPORTED_LOCATION
                "${CMAKE_SOURCE_DIR}/libvlc/lib/libvlc.dylib"
                )
            include_directories("${CMAKE_SOURCE_DIR}/libvlc/include")
        else()
            find_library(LIBVLC vlc REQUIRED)
            message(STATUS "Using LibVLC from ${LIBVLC}")
            set_target_properties(VLC::VLC PROPERTIES IMPORTED_LOCATION ${LIBVLC})
        endif()
    endif()

    CPMAddPackage("https://code.videolan.org/videolan/libvlcpp.git#b0e7d093a89eae248ff17cf80e0fd8feb92089c0")
    CPMAddPackage("https://github.com/cameron314/readerwriterqueue.git@1.0.6")
    include_directories("${libvlcpp_SOURCE_DIR}" "${readerwriterqueue_SOURCE_DIR}")
endmacro()
