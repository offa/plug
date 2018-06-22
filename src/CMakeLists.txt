add_subdirectory(com)

add_executable(plug main.cpp)

add_library(plug-ui amp_advanced.cpp
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
target_link_libraries(plug-ui
                        PUBLIC
                            Qt5::Widgets
                            Qt5::Gui
                            Qt5::Core
                        )


target_link_libraries(plug
                        PRIVATE
                            plug-version
                            plug-ui
                            plug-mustang
                            plug-updater
                            build-libs
                            libusb-1.0::libusb-1.0
                        )

install(TARGETS plug EXPORT plug-config DESTINATION bin)

