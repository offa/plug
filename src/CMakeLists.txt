
add_executable(plug main.cpp
                    amp_advanced.cpp
                    amplifier.cpp
                    defaulteffects.cpp
                    effect.cpp
                    library.cpp
                    loadfromamp.cpp
                    loadfromfile.cpp
                    mainwindow.cpp
                    mustang.cpp
                    quickpresets.cpp
                    save_effects.cpp
                    saveonamp.cpp
                    savetofile.cpp
                    settings.cpp
                    )

qt5_use_modules(plug Core Widgets Gui Network)
target_link_libraries(plug ${LIBUSB_LIBRARIES})

