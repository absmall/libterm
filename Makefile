CFLAGS = -g3 -Wall

all: obj/cterm

include cterm/Makefile
include lib/Makefile
