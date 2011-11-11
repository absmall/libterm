NAME = $(dir $(lastword $(filter-out common.mk, $(MAKEFILE_LIST))))
MYDIR = $(addprefix $(NAME),$1)

obj/%.o:$(NAME)%.c
	gcc $(CFLAGS) -c -o $@ -I$(NAME) -Iinclude $<
