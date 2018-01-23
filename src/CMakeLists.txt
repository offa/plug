
add_executable(plug main.cpp
                    amp_advanced.cpp
                    amplifier.cpp
                    defaulteffects.cpp
                    effect.cpp
                    library.cpp
                    loadfromamp.cpp
                    loadfromfile.cpp
                    mainwindow.cpp
                    quickpresets.cpp
                    save_effects.cpp
                    saveonamp.cpp
                    savetofile.cpp
                    settings.cpp
                    )

add_library(plug-mustang mustang.cpp)

target_link_libraries(plug
                        PRIVATE
                            plug-version
                            plug-mustang
                            build-libs
                        PUBLIC
                            Qt5::Core
                            Qt5::Widgets
                            Qt5::Gui
                            Qt5::Network
                            libusb-1.0::libusb-1.0
                        )

