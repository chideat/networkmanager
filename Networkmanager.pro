QT += widgets webkitwidgets dbus
CONFIG += C++11

HEADERS += \ 
    src/notification.h \
    src/gui/window.h \
    src/types.h \
    src/net/wireless.h \
    src/net/pppoe.h \
    src/net/ppp.h \
    src/net/connection.h \
    src/net/setting.h \
    src/net/network.h \
    src/net/networkmanager.h \
    src/net/device.h \
    src/net/accesspoint.h \
    src/desktop/desktop.h

OTHER_FILES += \
    doc/pppoe \
    doc/steps \
    doc/Problems.txt

SOURCES += \ 
    src/notification.cpp \
    src/main.cpp \
    src/gui/window.cpp \
    src/net/wireless.cpp \
    src/net/pppoe.cpp \
    src/net/ppp.cpp \
    src/net/setting.cpp \
    src/net/network.cpp \
    src/net/device.cpp \
    src/desktop/desktop.cpp

RESOURCES += \
    src/xml/resources.qrc
