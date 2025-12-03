cmake_minimum_required(VERSION 3.12.0)

project(plug VERSION 1.5.0)
message(STATUS "~~~ ${PROJECT_NAME} v${PROJECT_VERSION} ~~~")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
option(PLUG_UNITTEST "Build Unit Tests" ON)
message(STATUS "Unit Tests : ${PLUG_UNITTEST}")

option(PLUG_COVERAGE "Enable Coverage" OFF)
message(STATUS "Coverage : ${PLUG_COVERAGE}")

option(PLUG_LTO "Enable LTO" OFF)
message(STATUS "LTO : ${PLUG_LTO}")

option(PLUG_SANITIZER_ASAN "Enable ASan" OFF)
message(STATUS "ASan : ${PLUG_SANITIZER_ASAN}")

option(PLUG_SANITIZER_UBSAN "Enable UBSan" OFF)
message(STATUS "UBSan : ${PLUG_SANITIZER_UBSAN}")

include(DeprecatedOptions)


if( CMAKE_BUILD_TYPE )
    message(STATUS "Build Type : ${CMAKE_BUILD_TYPE}")
else()
    message(STATUS "Build Type : None")
endif()

include(GNUInstallDirs)

if( PLUG_LTO )
    include(LTO)
endif()

if( PLUG_COVERAGE )
    include(Coverage)
endif()

include(Sanitizer)
include(Install)
include(Template)
include(ClangFormat)

add_compile_options(-Wall
                    -Wextra
                    -pedantic
                    -pedantic-errors
                    -Werror
                    -Wshadow
                    -Wold-style-cast
                    -Wnull-dereference
                    -Wnon-virtual-dtor
                    -Woverloaded-virtual
                    )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 COMPONENTS Core Widgets Gui REQUIRED)
find_package(libusb-1.0 REQUIRED)


include_directories("include")
add_subdirectory(src)


if( PLUG_UNITTEST )
    enable_testing()
    add_subdirectory("test")
endif()

