include common.mk
#all: qterm/qterm cterm/cterm
#
#qterm: qterm.o libterm.so
#	gcc -Wall -g3 -o $@ $< -L. -lterm
#
#cterm: cterm.o libterm.so
#	gcc -Wall -g3 -o $@ $< -L. -lterm -lcurses
#

SRC = term.c fork.c render.c memory.c

obj/libterm.so: $(addprefix obj/,$(SRC:.c=.o))
	gcc $(CFLAGS) -shared -o $@ $^ -lutil
