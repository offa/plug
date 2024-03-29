
set(TEMPLATE_DIR "${CMAKE_SOURCE_DIR}/cmake/template")
set(GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")


configure_file(${TEMPLATE_DIR}/Version.cpp.in
                ${GENERATED_DIR}/Version.cpp
                @ONLY
                )

add_library(plug-version ${GENERATED_DIR}/Version.cpp)
