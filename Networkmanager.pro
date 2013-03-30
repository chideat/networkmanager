QT += widgets webkitwidgets dbus
CONFIG += C++11

HEADERS += \ 
    src/notification.h \
    src/gui/window.h \
    src/types.h \
    src/net/connection.h \
    src/net/setting.h \
    src/net/network.h \
    src/net/networkmanager.h \
    src/net/device.h \
    src/net/accesspoint.h \
    src/run.h \
    src/desktop.h \
    src/watchdog.h \
    src/net/check.h

OTHER_FILES += \
    doc/pppoe \
    doc/steps \
    doc/Problems.txt

SOURCES += \ 
    src/notification.cpp \
    src/main.cpp \
    src/gui/window.cpp \
    src/net/setting.cpp \
    src/net/network.cpp \
    src/net/device.cpp \ 
    src/desktop.cpp

RESOURCES += \
    src/xml/resources.qrc
