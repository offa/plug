cmake_minimum_required(VERSION 2.8.6)

project(plug)


file(GLOB SRC_FILES *.cpp)
file(GLOB UI_FILES *.ui)


find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})
set(QT_USE_QTNETWORK TRUE)
set(CMAKE_AUTOMOC TRUE)
qt4_wrap_ui(UI_HDR_FILES ${UI_FILES})


find_library(LIB_USB usb-1.0)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME} ${SRC_FILES} ${UI_HDR_FILES})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} ${QT_QTNETWORK_LIBRARY} ${LIB_USB})
