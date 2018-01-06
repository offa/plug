
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
add_library(TestLibs INTERFACE)
target_include_directories(TestLibs INTERFACE
                                    ${GTest_INCLUDE_DIR}
                                    ${GTest_Mock_INCLUDE_DIR}
                                    )
target_link_libraries(TestLibs INTERFACE
                                ${GTest_LIBRARY}
                                ${GTest_Main_LIBRARY}
                                ${CMAKE_THREAD_LIBS_INIT}
                                )



add_executable(MustangTest MustangTest.cpp
                    )
add_test(MustangTest MustangTest)
target_link_libraries(MustangTest plug-mustang
                                TestLibs
                                )


add_custom_target(unittest MustangTest

                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )

