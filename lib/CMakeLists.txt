include_directories(.)
include_directories(../include)
add_library(term SHARED fork.c memory.c render.c term.c vt100_escape.c)
target_link_libraries(term util)
