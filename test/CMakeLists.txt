
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
add_library(TestLibs INTERFACE)
target_include_directories(TestLibs SYSTEM
                                INTERFACE
                                    ${GTest_INCLUDE_DIR}
                                    ${GTest_Mock_INCLUDE_DIR}
                                    )
target_link_libraries(TestLibs INTERFACE
                                ${GTest_LIBRARY}
                                ${GTest_Mock_LIBRARY}
                                ${GTest_Main_LIBRARY}
                                ${CMAKE_THREAD_LIBS_INIT}
                                build-libs
                                )

add_subdirectory(mocks)



add_executable(MustangTest MustangTest.cpp
                    )
add_test(MustangTest MustangTest)
target_link_libraries(MustangTest plug-mustang
                                TestLibs
                                LibUsbMocks
                                )

add_executable(IdLookupTest IdLookupTest.cpp)
add_test(IdLookupTest IdLookupTest)
target_link_libraries(IdLookupTest TestLibs
                                )


add_custom_target(unittest MustangTest
                        COMMAND IdLookupTest

                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )

