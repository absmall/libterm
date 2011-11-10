qshell: qshell.o libterm.so
	gcc -Wall -g3 -o $@ $< -L. -lterm

libterm.so: term.o fork.o term_render.o
	gcc -Wall -g3 -shared -o $@ $^ -lutil

%.o:%.c
	gcc -Wall -g3 -c -o $@ -I. $<
