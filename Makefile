CFLAGS = -g3 -Wall

all: obj/cterm

include cterm/module.mk
include lib/module.mk
