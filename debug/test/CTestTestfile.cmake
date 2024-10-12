# CMake generated Testfile for 
# Source directory: /home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test
# Build directory: /home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/debug/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MustangTest "MustangTest")
set_tests_properties(MustangTest PROPERTIES  _BACKTRACE_TRIPLES "/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;22;add_test;/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;0;")
add_test(CommunicationTest "CommunicationTest")
set_tests_properties(CommunicationTest PROPERTIES  _BACKTRACE_TRIPLES "/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;36;add_test;/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;0;")
add_test(UsbTest "UsbTest")
set_tests_properties(UsbTest PROPERTIES  _BACKTRACE_TRIPLES "/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;47;add_test;/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;0;")
add_test(IdLookupTest "IdLookupTest")
set_tests_properties(IdLookupTest PROPERTIES  _BACKTRACE_TRIPLES "/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;56;add_test;/home/tim/ubuntu-pkg-rebuilds/mustang-plug/offa-plug/test/CMakeLists.txt;0;")
subdirs("mocks")
