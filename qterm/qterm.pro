TEMPLATE = app
TARGET = qterm
CONFIG += 

SOURCES += \
	qterm.cpp \
	qpiekey.cpp \
	qpiekeyboard.cpp \
	logging.c \
	qscrollterm.cpp \
	main.cpp \

HEADERS += \
	qterm.h \
	qpiekey.h \
	qpiekeyboard.h \
	qscrollterm.h \

qnx {
	SOURCES += qvkbhandler.cpp
	HEADERS += qvkbhandler.h
}

INCLUDEPATH += \
	. \
	../include \

LIBS += \
	-L../lib -lterm -lm

qnx {
	LIBS += \
		-lbps -lslog2
}

OTHER_FILES +=
