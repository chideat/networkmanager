QMAKE_CXXFLAGS += -std=c++0x

QT += widgets webkitwidgets

HEADERS += \
    src/ppp/pppoe.h \
    src/types.h \
    src/notification.h \
    src/gui/window.h

OTHER_FILES += \
    doc/pppoe

SOURCES += \
    src/discovery.cpp \
    src/ppp/pppoe.cpp \
    src/gui/window.cpp
