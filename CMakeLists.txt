cmake_minimum_required(VERSION 3.9.4)

project(plug VERSION 0.1.3)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(UNITTEST "Build Unit Tests" ON)
message(STATUS "Unit Tests : ${UNITTEST}")

option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Coverage : ${COVERAGE}")

option(LTO "Enable LTO" OFF)
message(STATUS "LTO : ${LTO}")


if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()

if( LTO )
    include(LTO)
endif()

include(Install)


add_compile_options(-Wall
                    -Wextra
                    -pedantic
                    -Werror
                    -Wshadow
                    -Wold-style-cast
                    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



## Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)


## LibUSB
find_package(libusb-1.0 REQUIRED)
add_library(libusb INTERFACE)
target_link_libraries(libusb INTERFACE ${libusb-1.0_LIBRARY})
target_include_directories(libusb INTERFACE ${libusb-1.0_INCLUDE_DIR})


include_directories("include")
add_subdirectory(src)


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

