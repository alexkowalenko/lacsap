#
# lacsap compiler
#
# Copyright © 2020 Alex Kowalenko
#


add_executable(testrunner testrunner.cpp)

add_test(NAME debugtest COMMAND testrunner -g WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME normtest COMMAND testrunner -O0 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME fasttest COMMAND testrunner -O1 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME opttests COMMAND testrunner -O2 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})