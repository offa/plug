cmake_minimum_required(VERSION 3.10.0)

project(plug VERSION 1.3.1)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(UNITTEST "Build Unit Tests" ON)
message(STATUS "Unit Tests : ${UNITTEST}")

option(INTEGRATIONTEST "Build Integrationtests" OFF)
message(STATUS "Integrationtests : ${INTEGRATIONTEST}")

option(COVERAGE "Enable Coverage" OFF)
message(STATUS "Coverage : ${COVERAGE}")

option(LTO "Enable LTO" OFF)
message(STATUS "LTO : ${LTO}")

option(SANITIZER_ASAN "Enable ASan" OFF)
message(STATUS "ASan : ${SANITIZER_ASAN}")

option(SANITIZER_UBSAN "Enable UBSan" OFF)
message(STATUS "UBSan : ${SANITIZER_UBSAN}")


if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()


if( LTO )
    include(LTO)
endif()

if( COVERAGE )
    include(Coverage)
endif()

include(Sanitizer)
include(Install)
include(Template)
include(ClangFormat)

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
find_package(libusb-1.0 REQUIRED)


include_directories("include")
add_subdirectory(src)


if( UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

if( INTEGRATIONTEST )
    add_subdirectory("test/integration")
endif()
