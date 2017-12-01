find_package(PkgConfig)
pkg_check_modules(PKG_libusb-1.0 QUIET libusb-1.0)
set(libusb-1.0_DEFINITIONS ${PKG_libusb-1.0_CFLAGS_OTHER})

find_path(libusb-1.0_INCLUDE_DIR "libusb-1.0/libusb.h"
                                HINTS ${PKG_libusb-1.0_INCLUDE_DIRS}
                                )

find_library(libusb-1.0_LIBRARY NAMES usb-1.0
                                HINTS ${PKG_libusb-1.0_LIBDIR}
                                        ${PKG_libusb-1.0_LIBRARY_DIRS}
                                        "${libusb-1.0_DIR}/lib"
                                )


set(libusb-1.0_LIBRARIES ${libusb-1.0_LIBRARY})
set(libusb-1.0_INCLUDE_DIRS ${libusb-1.0_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libusb-1.0 DEFAULT_MSG
                                    libusb-1.0_LIBRARY
                                    libusb-1.0_INCLUDE_DIR
                                    )

mark_as_advanced(libusb-1.0_INCLUDE_DIR libusb-1.0_LIBRARY)

