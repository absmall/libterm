FIND_PACKAGE(Qt4 REQUIRED)
SET(SOURCES app.cpp main.cpp logging.c terminalscreen.cpp)
SET(HEADERS app.hpp)
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
ADD_EXECUTABLE(casterm ${SOURCES} ${HEADERS_MOC})
ADD_DEFINITIONS(-I$ENV{QNX_TARGET}/usr/include/qt4 -I$ENV{QNX_TARGET}/usr/include/qt4/QtCore)
SET(CASTERM_LIBRARIES -L../lib -L$ENV{QNX_TARGET}/armle-v7/usr/lib/qt4/lib -lterm bbcascades QtCore QtDeclarative QtScript QtGui QtSql QtNetwork QtXml QtXmlPatterns QtSvg slog2)
TARGET_LINK_LIBRARIES(casterm ${CASTERM_LIBRARIES})

# Build a library for putting casterm and an application into a single package
ADD_LIBRARY(castermlib SHARED ${SOURCES})
TARGET_LINK_LIBRARIES(castermlib ${CASTERM_LIBRARIES})
SET_TARGET_PROPERTIES(castermlib PROPERTIES COMPILE_FLAGS -DFAKE_MAIN)
INSTALL(TARGETS castermlib LIBRARY DESTINATION lib)

ADD_CUSTOM_COMMAND(OUTPUT casterm.bar
                   COMMAND blackberry-nativepackager
				   -package
                   casterm.bar
                   -devmode
				   ${CMAKE_CURRENT_SOURCE_DIR}/bar-descriptor.xml casterm
				   -e ${CMAKE_CURRENT_SOURCE_DIR}/assets/main.qml assets/main.qml
				   -e ${CMAKE_CURRENT_BINARY_DIR}/../lib/libterm.so lib/libterm.so
				   -e ${CMAKE_CURRENT_SOURCE_DIR}/../icon.png res/icon.png
				   -e ${CMAKE_CURRENT_SOURCE_DIR}/../splashscreen.png res/splashscreen.png
                   DEPENDS casterm ${CMAKE_CURRENT_SOURCE_DIR}/bar-descriptor.xml ${CMAKE_CURRENT_BINARY_DIR}/../lib/libterm.so
				  )

ADD_CUSTOM_TARGET(castermbar ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/casterm.bar)

INSTALL(TARGETS casterm DESTINATION bin)
