add_subdirectory(com)
add_subdirectory(ui)

add_executable(plug Main.cpp)
target_link_libraries(plug
                        PRIVATE
                            plug-version
                            plug-ui
                            plug-mustang
                            plug-communication
                            plug-updater
                            build-libs
                            libusb-1.0::libusb-1.0
                        )

install(TARGETS plug EXPORT plug-config DESTINATION bin)
