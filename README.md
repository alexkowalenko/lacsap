# LACSAP compiler

This is a nearly complete Pascal compiler, based on LLVM.

- Updated to build with CMake build system. CTest runs the test cases.

- Compiles as C++17 with LLVM Clang++ 10.0.0, compiles without warnings.

- Compiles using with LLVM version 6.0.1 instead of a particular commit mentioned in [llvmversion](llvmversion), so no need to build a custom version of LLVM.

- clang-format provided. Code re-organised.

## Original README

*The compiler now passes the ISO7185 Pascal Acceptance Test suite.*

See BUILD.md for build instructions. (*This is now out of date.*)

The purpose of the project is mainly to learn how LLVM operates together with a compiler.

Among things NOT yet supported are:
- Debug symbols (somewhat working).
- Classes & objects are only somewhat supported.
- Good error handling. Now slightly improved.
- Proper type checking. Now better - but not perfect.
- Support for unsigned integral types. Some support.
- Constant expressions (e.g. const x = 7; type arr = array [1..x+1] of integer;)
- Refactor AST dump functions to use visitors.
- Separate compile units - partly working.
- Stop using clang as the "linker". This may never happen... (Now supporting gcc as alternative)

Lots of other small and large things that I can't think of right now.

The above list is a rough "todo" list in prioritised order.

Regards,
Mats
