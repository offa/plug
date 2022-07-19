set(PLUG_UDEV_RULE_PATH "/lib/udev/rules.d" CACHE PATH "Udev rules directory.")
set(PLUG_DESKTOP_PATH "/lib/share/applications" CACHE PATH "Desktop file directory.")
set(PLUG_ICON_PATH "/usr/share/icons/hicolor" CACHE PATH "HiColor icon theme directory.")

install(FILES
        ${CMAKE_SOURCE_DIR}/cmake/50-mustang.rules
        ${CMAKE_SOURCE_DIR}/cmake/70-mustang-uaccess.rules
        ${CMAKE_SOURCE_DIR}/cmake/70-mustang-plugdev.rules
        DESTINATION ${PLUG_UDEV_RULE_PATH}
        )
install(FILES ${CMAKE_SOURCE_DIR}/cmake/plug.desktop
  DESTINATION ${PLUG_DESKTOP_PATH}
        )

install(FILES ${CMAKE_SOURCE_DIR}/cmake/mustang-plug.svg
        DESTINATION ${PLUG_ICON_PATH}/scalable/apps/
)

install(EXPORT plug-config DESTINATION share/plug/cmake)

