
find_program(GCOVR gcovr DOC "LCov")

if( NOT GCOVR )
    message(SEND_ERROR "'gcovr' not found")
endif()


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")

set(COV_DIR "${CMAKE_BINARY_DIR}/coverage")

if( NOT EXISTS ${COV_DIR} )
    file(MAKE_DIRECTORY ${COV_DIR})
endif()


add_custom_target(coverage ${CMAKE_COMMAND} -E make_directory ${COV_DIR}
    COMMAND ${GCOVR} --root ${PROJECT_SOURCE_DIR}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Generate coverage data"
    VERBATIM
    )
