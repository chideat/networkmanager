#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusReply>
#include "setting.h"
#include "networkmanager.h"
#include "../types.h"

class AccessPoint : public QObject {
    Q_OBJECT
public:
    AccessPoint(QString path, QObject *parent = NULL): QObject(parent) {
        this->path = path;
        QDBusInterface interface(DBUS_NET_SERVICE, path, DBUS_NET_PROPERTIES,
                                 QDBusConnection::systemBus());
        QDBusReply<Arr_Var> reply;
        count = 0;
        do {
            reply = interface.call(QString(DBUS_NET_PROPERTIES_GetAll), 
                       QVariant(DBUS_NET_INTERFACE_ACCESS_POINT));
        }while(!reply.isValid() && count ++ < COUNT);
        if(reply.isValid()) {
            properties = reply.value();
        }
        settings = NULL;
    }
    
    ~AccessPoint(){}
    Arr_Var properties;
    QString path;
    QString *settings;
    QVariant getProperty(QString key);
private :
    uint32_t count;
};

#endif // ACCESSPOINT_H
