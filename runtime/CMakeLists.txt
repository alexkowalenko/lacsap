#
# lacsap compiler
#
# Copyright © 2020 Alex Kowalenko
#

add_library(runtime STATIC
            main.c math.c fileio.c write.c read.c readbin.c writebin.c alloc.c set.c string.c array.c 
            panic.c clock.c rangeerror.c assign.c getput.c params.c val.c)

# install
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
install(TARGETS runtime DESTINATION bin)
