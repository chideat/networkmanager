#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include <QObject>
#include <QDBusInterface>
#include <QDBusReply>
#include <QUuid>
#include <QMapIterator>
#include "setting.h"
#include "networkmanager.h"
#include "../types.h"
#include "setting.h"
#include <QDebug>

namespace Net {

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
        setting = NULL;
        uuid = QUuid::createUuid().toString().remove('{').remove('}');
        
        QDBusConnection::systemBus().connect(QString(DBUS_NET_SERVICE), 
                                             path, QString(DBUS_NET_INTERFACE_ACCESS_POINT), 
                                             QString(DBUS_NET_INTERFACE_ACCESSPOINT_SIGNAL_PropertiesChanged), 
                                             this, SLOT(propertiesChanged(Arr_Var)));
    }
    
    ~AccessPoint(){}
    Arr_Var properties;
    QString uuid;
    QString path;
    Setting *setting;
public Q_SLOTS:
    QString getUuid() { return uuid; }
    QString getPath() { return path;}
    QVariant getProperty(QString key) { return properties[key]; }
    bool encrypt() {
        if(properties[DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags] != 
                SEC_NONE || properties[DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags] != 
                SEC_NONE) 
            return true;
        return false;
    }
    
    Arr_Var getProperties() { return properties; }
    Setting *getSetting() {return setting; }
    
    void setSetting(Setting *s) { 
        setting = s; 
        uuid = s->get(PRO_CONNECTION, PRO_CONNECTION_UUID);
    }
    
    void propertiesChanged(Arr_Var properties) {
        //check few properties
        QMapIterator<QString, QVariant> iterator(properties);
        while(iterator.hasNext()) {
            iterator.next();
            if(iterator.value() != this->properties[iterator.key()])  {
                emit propertyChanged(iterator.key(), iterator.value());
                this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid] = iterator.value();
            }
        }
    }
    
private :
    uint32_t count;
Q_SIGNALS:
    void propertyChanged(QString key, QVariant value);
};
}
#endif // ACCESSPOINT_H
