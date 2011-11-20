include_directories(../include)
add_executable(cterm cterm.c)
target_link_libraries(cterm term curses)
