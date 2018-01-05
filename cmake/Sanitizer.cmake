
if( SANITIZER_ASAN )
    add_compile_options(-fsanitize=address)
endif()

if( SANITIZER_UBSAN )
    add_compile_options(-fsanitize=undefined)
endif()

