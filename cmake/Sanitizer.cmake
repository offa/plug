
add_library(build-libs INTERFACE)

macro(enable_sanitizer san)
    set(SAN_FLAG "-fsanitize=${san}")
    add_compile_options(${SAN_FLAG})
    target_link_libraries(build-libs INTERFACE ${SAN_FLAG})
endmacro()


if( SANITIZER_ASAN )
    enable_sanitizer(address)
endif()

if( SANITIZER_UBSAN )
    enable_sanitizer(undefined)
endif()

