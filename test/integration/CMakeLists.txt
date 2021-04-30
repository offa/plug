
add_executable(CommIT Main.cpp)
target_link_libraries(CommIT
                        PRIVATE
                            plug-mustang
                            plug-version
                            plug-communication
                            plug-communication-usb
                            plug-libusb
                            build-libs
                            )
