add_subdirectory(com)
add_subdirectory(ui)

add_executable(plug Main.cpp)
target_link_libraries(plug
                        PRIVATE
                            plug-version
                            plug-ui
                            plug-mustang
                            plug-communication
                            plug-communication-usb
                            plug-libusb
                            plug-updater
                            build-libs
                        )

install(TARGETS plug EXPORT plug-config DESTINATION bin)
