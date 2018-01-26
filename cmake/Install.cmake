set(PLUG_UDEV_RULE_PATH "/lib/udev/rules.d" CACHE PATH "Udev rules directory.")

install(FILES ${CMAKE_SOURCE_DIR}/cmake/50-mustang.rules
        DESTINATION ${PLUG_UDEV_RULE_PATH}
        )

