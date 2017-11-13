cmake_minimum_required(VERSION 3.9.4)

project(plug VERSION 0.1.3)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(UNITTEST "Build Unit Tests" ON)
option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Coverage : ${COVERAGE}")


if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()



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


## LibUSB
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)


include_directories("include")
add_subdirectory(src)


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

