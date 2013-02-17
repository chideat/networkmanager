QMAKE_CXXFLAGS += -std=c++0x

QT += widgets webkitwidgets

HEADERS += \
    src/ppp/pppoe.h \
    src/types.h \
    src/notification.h \
    src/gui/window.h \
    src/config.h

OTHER_FILES += \
    doc/pppoe \
    doc/steps

SOURCES += \
    src/discovery.cpp \
    src/ppp/pppoe.cpp \
    src/gui/window.cpp \
    src/main.cpp

RESOURCES += \
    src/xml/resources.qrc
