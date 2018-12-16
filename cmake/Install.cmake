set(PLUG_UDEV_RULE_PATH "/lib/udev/rules.d" CACHE PATH "Udev rules directory.")
set(PLUG_DESKTOP_PATH "/lib/share/applications" CACHE PATH "Desktop file directory.")

install(FILES ${CMAKE_SOURCE_DIR}/cmake/50-mustang.rules
        DESTINATION ${PLUG_UDEV_RULE_PATH}
        )
install(FILES ${CMAKE_SOURCE_DIR}/cmake/plug.desktop
  DESTINATION ${PLUG_DESKTOP_PATH}
        )

install(EXPORT plug-config DESTINATION share/plug/cmake)

