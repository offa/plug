find_package(GTest REQUIRED)


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
                ConnectionFactoryTest.cpp
                UsbCommTest.cpp
                )
add_test(CommunicationTest CommunicationTest)
target_link_libraries(CommunicationTest PRIVATE
                        plug-communication
                        TestLibs
                        UsbDeviceMock
                        )

add_executable(UsbTest
    UsbTest.cpp
    )
add_test(UsbTest UsbTest)
target_link_libraries(UsbTest PRIVATE
                        plug-communication-usb
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
                        COMMAND UsbTest
                        COMMAND IdLookupTest

                        COMMENT "Running unittests\n\n"
                        VERBATIM
                        )
