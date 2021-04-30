add_library(LibUsbMocks LibUsbMocks.cpp)
target_link_libraries(LibUsbMocks PRIVATE TestLibs)
target_include_directories(LibUsbMocks PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(UsbDeviceMock UsbDeviceMock.cpp)
target_link_libraries(UsbDeviceMock PRIVATE TestLibs)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(LibUsbMocks PUBLIC -Wno-gnu-zero-variadic-macro-arguments)
    target_compile_options(UsbDeviceMock PUBLIC -Wno-gnu-zero-variadic-macro-arguments)
endif()
