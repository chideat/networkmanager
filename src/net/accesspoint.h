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
        url = QUuid::createUuid().toString().remove('{').remove('}');
        
        QDBusConnection::systemBus().connect(QString(DBUS_NET_SERVICE), 
                                             path, QString(DBUS_NET_INTERFACE_ACCESS_POINT), 
                                             QString(DBUS_NET_INTERFACE_ACCESSPOINT_SIGNAL_PropertiesChanged), 
                                             this, SLOT(propertiesChanged(Arr_Var)));
    }
    
    ~AccessPoint(){}
    Arr_Var properties;
    QString url;
    QString path;
    QString *settings;
public Q_SLOTS:
    QString getUrl() { return url; }
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
    void propertiesChanged(Arr_Var properties) {
        //check few properties
        if(properties.contains(QString(DBUS_NET_INTERFACE_ACCESS_POINT_Ssid)) &&
                properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid] != 
                this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid]) {
            emit propertyChanged(DBUS_NET_INTERFACE_ACCESS_POINT_Ssid, 
                                 properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid]);
            this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid] = 
                    properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid];
        } 
        if(properties.contains(QString(DBUS_NET_INTERFACE_ACCESS_POINT_Strength)) &&
                properties[DBUS_NET_INTERFACE_ACCESS_POINT_Strength] != 
            this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_Strength]) {
            emit propertyChanged(DBUS_NET_INTERFACE_ACCESS_POINT_Strength, 
                                 properties[DBUS_NET_INTERFACE_ACCESS_POINT_Strength]);
            this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_Strength] = 
                    properties[DBUS_NET_INTERFACE_ACCESS_POINT_Strength];
        }
        if(properties.contains(QString(DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags)) &&
                properties[DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags] != 
                this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags]) {
            emit propertyChanged(DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags, 
                                 properties[DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags]);
            this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags] = 
                    properties[DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags];
        }
        if(properties.contains(QString(DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags)) &&
                properties[DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags] != 
                this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags]) {
            emit propertyChanged(DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags, 
                                 properties[DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags]);
            this->properties[DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags] = 
                    properties[DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags];
        }
    }

private :
    uint32_t count;
Q_SIGNALS:
    void propertyChanged(QString key, QString value);
};

#endif // ACCESSPOINT_H
