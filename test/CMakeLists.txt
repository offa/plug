find_package(GTest REQUIRED)

# #192 – Workaround for broken GTest
if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    target_compile_options(GTest::GTest INTERFACE -Wno-gnu-zero-variadic-macro-arguments)
endif()

add_library(TestLibs INTERFACE)
target_link_libraries(TestLibs INTERFACE
                        GTest::GTest
                        GTest::Mock
                        GTest::Main
                        build-libs
                        )

add_subdirectory(mocks)



add_executable(MustangTest
                MustangTest.cpp
                PacketSerializerTest.cpp
                PacketTest.cpp
                )
add_test(MustangTest MustangTest)
target_link_libraries(MustangTest PRIVATE
                        plug-mustang
                        plug-communication
                        TestLibs
                        LibUsbMocks
                        )



add_executable(CommunicationTest
                UsbCommTest.cpp
                ConnectionFactoryTest.cpp
                )
add_test(CommunicationTest CommunicationTest)
target_link_libraries(CommunicationTest PRIVATE
                        plug-communication
                        TestLibs
                        LibUsbMocks
                        )


add_executable(IdLookupTest IdLookupTest.cpp)
add_test(IdLookupTest IdLookupTest)
target_link_libraries(IdLookupTest PRIVATE
                        TestLibs
                        )


add_custom_target(unittest MustangTest
                        COMMAND CommunicationTest
                        COMMAND IdLookupTest

                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )
