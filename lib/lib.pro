TEMPLATE = lib
TARGET = term

SOURCES += \
	fork.c \
	memory.c \
	render.c \
	term.c \
	escape.c \
	terminfo.c \
	callbacks.c \
	rect.c \

HEADERS += \
	libterm_internal.h \

INCLUDEPATH += \
	. \
	../include \

!qnx {
	LIBS += \
		-lutil \

}
