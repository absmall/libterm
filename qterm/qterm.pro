QMAKE_LFLAGS += '-Wl,-rpath,\'./app/native/lib\''
QMAKE_CXXFLAGS = -O0 -g3
SOURCES += qterm.cpp qpiekey.cpp qpiekeyboard.cpp
HEADERS += qterm.h qpiekey.h qpiekeyboard.h
LIBS += -L../obj.playbook/lib/ -lterm
INCLUDEPATH=../include
#-devMode -debugToken debugToken.bar
package.target = $${TARGET}.bar
package.depends = $${TARGET}
package.commands = blackberry-nativepackager \
    -devMode \
    -package $${TARGET}.bar -arg -platform -arg blackberry \
    blackberry-tablet.xml $${TARGET} \
    -e icon.png res/icon.png \
    -e splashscreen.png res/splashscreen.png \
    -e ../obj.playbook/lib/libterm.so lib/libterm.so \

QMAKE_EXTRA_TARGETS += package
