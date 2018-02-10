add_library(LibUsbMocks LibUsbMocks.cpp)
target_link_libraries(LibUsbMocks PRIVATE TestLibs)
target_include_directories(LibUsbMocks PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
