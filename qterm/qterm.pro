QMAKE_LFLAGS += '-Wl,-rpath,\'./app/native/lib\''
QMAKE_CXXFLAGS = -O0 -g3
SOURCES += qterm.cpp qpiekey.cpp qpiekeyboard.cpp
HEADERS += qterm.h qpiekey.h qpiekeyboard.h
LIBS += -L../obj.playbook/lib/ -lterm
QT += widgets
INCLUDEPATH=../include
#-devMode -debugToken debugToken.bar
package.target = $${TARGET}.bar
package.depends = $${TARGET}
package.commands = blackberry-nativepackager \
    -devMode \
    -package $${TARGET}.bar -arg -platform -arg qnx \
    blackberry-tablet.xml $${TARGET} \
    -e icon.png res/icon.png \
    -e splashscreen.png res/splashscreen.png \
    -e ../obj.playbook/lib/libterm.so lib/libterm.so \
    -e $$[QT_INSTALL_LIBS]/libQtCore.so.5 lib/libQtCore.so.5 \
    -e $$[QT_INSTALL_LIBS]/libQtGui.so.5 lib/libQtGui.so.5 \
    -e $$[QT_INSTALL_LIBS]/libQtWidgets.so.5 lib/libQtWidgets.so.5 \
    -e $$[QT_INSTALL_PLUGINS]/platforms/libqnx.so plugins/platforms/libqnx.so

QMAKE_EXTRA_TARGETS += package
