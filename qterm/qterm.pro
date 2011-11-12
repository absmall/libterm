QMAKE_CXXFLAGS = -O0 -g3
SOURCES += qterm.cpp
HEADERS += qterm.h
LIBS += -L../obj -L../lib/nto/arm/so.le.v7 -lterm
INCLUDEPATH=../include
