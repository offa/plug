find_package(PkgConfig)
find_package(Threads REQUIRED)


pkg_check_modules(PKG_GTest QUIET libGTest)
set(GTest_DEFINITIONS ${PKG_GTest_CFLAGS_OTHER})

find_path(GTest_INCLUDE_DIR "gtest/gtest.h"
                                HINTS ${PKG_GTest_INCLUDE_DIRS}
                                        "${GTest_DIR}/include"
                                )
find_path(GTest_Mock_INCLUDE_DIR "gmock/gmock.h"
                                HINTS ${PKG_GMock_INCLUDE_DIRS}
                                        "${GMock_DIR}/include"
                                        ${GTest_INCLUDE_DIR}
                                )

find_library(GTest_LIBRARY NAMES gtest
                                HINTS ${PKG_GTest_LIBDIR}
                                        ${PKG_GTest_LIBRARY_DIRS}
                                )
find_library(GTest_Mock_LIBRARY NAMES gmock
                                HINTS ${PKG_GTest_LIBDIR}
                                        ${PKG_GTest_LIBRARY_DIRS}
                                )
find_library(GTest_Main_LIBRARY NAMES gtest_main
                                HINTS ${PKG_GTest_LIBDIR}
                                        ${PKG_GTest_LIBRARY_DIRS}
                                )


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTest DEFAULT_MSG
                                    GTest_INCLUDE_DIR
                                    GTest_Mock_INCLUDE_DIR
                                    GTest_LIBRARY
                                    GTest_Mock_LIBRARY
                                    GTest_Main_LIBRARY
                                    )
mark_as_advanced(GTest_INCLUDE_DIR
                GTest_Mock_INCLUDE_DIR
                GTest_LIBRARY
                GTest_Mock_LIBRARY
                GTest_Main_LIBRARY
                )


add_library(GTest::GTest UNKNOWN IMPORTED)
set_target_properties(GTest::GTest PROPERTIES
                        IMPORTED_LOCATION "${GTest_LIBRARY}"
                        IMPORTED_LINK_INTERFACE_LANGUAGES CXX
                        INTERFACE_INCLUDE_DIRECTORIES "${GTest_INCLUDE_DIR}"
                        INTERFACE_LINK_LIBRARIES Threads::Threads
                        )

add_library(GTest::Main UNKNOWN IMPORTED)
set_target_properties(GTest::Main PROPERTIES
                        IMPORTED_LOCATION "${GTest_Main_LIBRARY}"
                        IMPORTED_LINK_INTERFACE_LANGUAGES CXX
                        )

add_library(GTest::Mock UNKNOWN IMPORTED)
set_target_properties(GTest::Mock PROPERTIES
                        IMPORTED_LOCATION "${GTest_Mock_LIBRARY}"
                        IMPORTED_LINK_INTERFACE_LANGUAGES CXX
                        INTERFACE_INCLUDE_DIRECTORIES "${GTest_Mock_INCLUDE_DIR}"
                        )

