FIND_PACKAGE(Qt4 REQUIRED)
SET(SOURCES qterm.cpp qpiekey.cpp qpiekeyboard.cpp logging.c qscrollterm.cpp main.cpp)
SET(HEADERS qterm.h qpiekey.h qpiekeyboard.h qscrollterm.h)
IF(${CMAKE_SYSTEM_NAME} MATCHES "qnx")
LIST(APPEND SOURCES qvkbhandler.cpp)
LIST(APPEND HEADERS qvkbhandler.h)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "qnx")
INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(../include)
QT4_WRAP_CPP(HEADERS_MOC ${HEADERS})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_EXECUTABLE(qterm ${SOURCES} ${HEADERS_MOC})
SET(QTERM_LIBRARIES ${QT_LIBRARIES} -L../lib -lterm -lm)
TARGET_LINK_LIBRARIES(qterm ${QTERM_LIBRARIES})

# Special qnx stuff
IF(${CMAKE_SYSTEM_NAME} MATCHES "qnx")

# Build a library for putting qterm and an application into a single package
ADD_LIBRARY(qtermlib SHARED ${SOURCES} ${HEADERS_MOC})
TARGET_LINK_LIBRARIES(qtermlib ${QTERM_LIBRARIES})
SET_TARGET_PROPERTIES(qtermlib PROPERTIES COMPILE_FLAGS -DFAKE_MAIN)
INSTALL(TARGETS qtermlib LIBRARY DESTINATION lib)

TARGET_LINK_LIBRARIES(qterm bps slog2)
ADD_CUSTOM_COMMAND(OUTPUT qterm.bar
                   COMMAND blackberry-nativepackager
                   -devMode
                   -package qterm.bar
				   ${CMAKE_CURRENT_SOURCE_DIR}/bar-descriptor.xml
                   -e ${CMAKE_CURRENT_BINARY_DIR}/qterm qterm
                   -e ${CMAKE_CURRENT_BINARY_DIR}/../lib/libterm.so lib/libterm.so
                   -e ${CMAKE_CURRENT_SOURCE_DIR}/../splashscreen.png splashscreen.png
                   -e ${CMAKE_CURRENT_SOURCE_DIR}/../icon.png icon.png
                   DEPENDS qterm bar-descriptor.xml ${CMAKE_CURRENT_BINARY_DIR}/../lib/libterm.so
				  )

ADD_CUSTOM_TARGET(qtermbar ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/qterm.bar)

ELSE(${CMAKE_SYSTEM_NAME} MATCHES "qnx")
TARGET_LINK_LIBRARIES(qterm term)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "qnx")
INSTALL(TARGETS qterm DESTINATION bin)
