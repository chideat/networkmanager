QMAKE_CXXFLAGS += -std=c++0x

QT += widgets webkitwidgets dbus

HEADERS += \ 
    src/notification.h \
    src/gui/window.h

OTHER_FILES += \
    doc/pppoe \
    doc/steps

SOURCES += \ 
    src/notification.cpp \
    src/main.cpp \
    src/gui/window.cpp

RESOURCES += \
    src/xml/resources.qrc
