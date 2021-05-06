set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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
