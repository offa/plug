add_library(LibUsbMocks LibUsbMocks.cpp)
target_link_libraries(LibUsbMocks PRIVATE TestLibs)
target_include_directories(LibUsbMocks PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(LibUsbMocks PUBLIC -Wno-gnu-zero-variadic-macro-arguments)
endif()
