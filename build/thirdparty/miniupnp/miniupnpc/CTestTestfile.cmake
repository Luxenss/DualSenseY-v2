# CMake generated Testfile for 
# Source directory: /home/engine/project/thirdparty/miniupnp/miniupnpc
# Build directory: /home/engine/project/build/thirdparty/miniupnp/miniupnpc
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(validateminixml "/home/engine/project/build/thirdparty/miniupnp/miniupnpc/minixmlvalid")
set_tests_properties(validateminixml PROPERTIES  _BACKTRACE_TRIPLES "/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;255;add_test;/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;0;")
add_test(validateminiwget "/home/engine/project/testminiwget.sh")
set_tests_properties(validateminiwget PROPERTIES  ENVIRONMENT "TESTSERVER=/home/engine/project/build/minihttptestserver;TESTMINIWGET=/home/engine/project/build/testminiwget" _BACKTRACE_TRIPLES "/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;257;add_test;/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;0;")
add_test(validateupnpreplyparse "/home/engine/project/testupnpreplyparse.sh")
set_tests_properties(validateupnpreplyparse PROPERTIES  ENVIRONMENT "TESTUPNPREPLYPARSE=/home/engine/project/build/testupnpreplyparse" WORKING_DIRECTORY "/home/engine/project" _BACKTRACE_TRIPLES "/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;265;add_test;/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;0;")
add_test(validateportlistingparse "/home/engine/project/build/thirdparty/miniupnp/miniupnpc/testportlistingparse")
set_tests_properties(validateportlistingparse PROPERTIES  _BACKTRACE_TRIPLES "/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;271;add_test;/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;0;")
add_test(validateigddescparse1 "/home/engine/project/build/thirdparty/miniupnp/miniupnpc/testigddescparse" "new_LiveBox_desc.xml" "new_LiveBox_desc.values")
set_tests_properties(validateigddescparse1 PROPERTIES  WORKING_DIRECTORY "/home/engine/project/testdesc" _BACKTRACE_TRIPLES "/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;273;add_test;/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;0;")
add_test(validateigddescparse2 "/home/engine/project/build/thirdparty/miniupnp/miniupnpc/testigddescparse" "linksys_WAG200G_desc.xml" "linksys_WAG200G_desc.values")
set_tests_properties(validateigddescparse2 PROPERTIES  WORKING_DIRECTORY "/home/engine/project/testdesc" _BACKTRACE_TRIPLES "/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;276;add_test;/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;0;")
add_test(validateaddr_is_reserved "/home/engine/project/build/thirdparty/miniupnp/miniupnpc/testaddr_is_reserved")
set_tests_properties(validateaddr_is_reserved PROPERTIES  _BACKTRACE_TRIPLES "/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;279;add_test;/home/engine/project/thirdparty/miniupnp/miniupnpc/CMakeLists.txt;0;")
