include common.mk

SRC = cterm.c

obj/cterm: $(addprefix obj/,$(SRC:.c=.o)) obj/libterm.so
	gcc -Wall -g3 -o $@ $< -Lobj -lterm -lcurses
