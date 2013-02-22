QMAKE_CXXFLAGS += -std=c++0x

QT += widgets webkitwidgets

HEADERS += \
    src/ppp/pppoe.h \
    src/types.h \
    src/notification.h \
    src/gui/window.h \
    src/config.h \
    src/ppp/relay.h \
    src/ppp/pppoe-server.h \
    src/ppp/md5.h \
    src/interface/unblock.h

OTHER_FILES += \
    doc/pppoe \
    doc/steps

SOURCES += \
    src/discovery.cpp \
    src/ppp/pppoe.cpp \
    src/gui/window.cpp \
    src/main.cpp \
    src/ppp/relay.cpp \
    src/ppp/pppoe-sniff.cpp \
    src/ppp/pppoe-server.cpp \
    src/ppp/ppp.cpp \
    src/ppp/plugin.cpp \
    src/ppp/md5.cpp \
    src/ppp/if.cpp \
    src/ppp/discovery.cpp \
    src/ppp/common.cpp

RESOURCES += \
    src/xml/resources.qrc
