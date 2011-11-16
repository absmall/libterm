CFLAGS = -g3 -Wall

.PHONY : clean

all: obj/cterm

clean:
	rm obj/*

include cterm/module.mk
include lib/module.mk
