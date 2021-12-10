macro(include_libvlc)
    find_package(LIBVLC REQUIRED)
    add_library(VLC::VLC IMPORTED UNKNOWN)
    set_target_properties(VLC::VLC PROPERTIES IMPORTED_LOCATION ${LIBVLC_LIBRARY})

    CPMAddPackage("https://code.videolan.org/videolan/libvlcpp.git#b0e7d093a89eae248ff17cf80e0fd8feb92089c0")
    CPMAddPackage("https://github.com/cameron314/readerwriterqueue.git@1.0.6")
    include_directories("${libvlcpp_SOURCE_DIR}" "${readerwriterqueue_SOURCE_DIR}" "${LIBVLC_INCLUDE_DIR}")
endmacro()