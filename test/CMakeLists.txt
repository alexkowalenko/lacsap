#
# lacsap compiler
#
# Copyright © 2020 Alex Kowalenko
#


add_executable(testrunner testrunner.cpp)

add_test(NAME fulltests COMMAND testrunner -O1 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})