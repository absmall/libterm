include common.mk

SRC = term.c fork.c render.c memory.c vt100_escape.c

obj/libterm.so: $(addprefix obj/,$(SRC:.c=.o))
	gcc $(CFLAGS) -shared -o $@ $^ -lutil
