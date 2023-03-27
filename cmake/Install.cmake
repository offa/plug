install(FILES
        ${CMAKE_SOURCE_DIR}/cmake/50-mustang.rules
        ${CMAKE_SOURCE_DIR}/cmake/70-mustang-uaccess.rules
        ${CMAKE_SOURCE_DIR}/cmake/70-mustang-plugdev.rules
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/udev/rules.d
        )
install(FILES ${CMAKE_SOURCE_DIR}/cmake/plug.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
        )

install(FILES ${CMAKE_SOURCE_DIR}/cmake/mustang-plug.svg
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps/
        )

install(EXPORT plug-config DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/plug/cmake)
