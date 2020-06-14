#
# lacsap compiler
#
# Copyright © 2020 Alex Kowalenko
#

set(CMAKE_CXX_FLAGS "-fno-exceptions -fno-rtti")

add_library(lac STATIC
  lexer.cpp source.cpp location.cpp token.cpp expr.cpp parser.cpp types.cpp constants.cpp builtin.cpp
  binary.cpp namedobject.cpp semantics.cpp trace.cpp stack.cpp utils.cpp callgraph.cpp)

add_executable(lacsap lacsap.cpp)

target_link_libraries(lacsap PRIVATE project_options project_warnings lac ${llvm_libs})

# install
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
install(TARGETS lacsap DESTINATION bin)
