qshell: qshell.o libterm.so
	gcc -g3 -o $@ $< -L. -lterm

libterm.so: term.o
	gcc -g3 -shared -o $@ $^

%.o:%.c
	gcc -g3 -c -o $@ -I. $<
