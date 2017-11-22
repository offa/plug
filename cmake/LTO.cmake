
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT lto_error)

if( lto_supported )
    message(STATUS "IPO / LTO enabled")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(STATUS "IPO / LTO not supported by the compiler")
endif()
