#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMetaType>
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
            qDBusRegisterMetaType<Arr_Var>();
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
        
        if(QDBusConnection::systemBus().connect(QString(DBUS_NET_SERVICE), 
                                             path, QString(DBUS_NET_INTERFACE_ACCESS_POINT), 
                                             DBUS_NET_INTERFACE_ACCESSPOINT_SIGNAL_PropertiesChanged, 
                                             this, SLOT(propertiesChanged(Arr_Var))) == false) {
            qDebug()<<"connection failed"<<path;
        }
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
        uuid = s->get(PRO_CONNECTION, PRO_CONNECTION_UUID).toString();
    }
    
    void propertiesChanged(Arr_Var properties) {
        //check few properties
        QMapIterator<QString, QVariant> iterator(properties);
        while(iterator.hasNext()) {
            iterator.next();
            if(iterator.value() != this->properties[iterator.key()])  {
                emit propertyChanged(iterator.key(), iterator.value());
                this->properties[iterator.key()] = iterator.value();
            }
        }
    }
    /**
     * @brief createConnection, a connection is needed for the first connection of accesspoints.
     * create the connection with the properties from the access point.
     * properties:
     *    
     */
    void createConnection() {
        Json connection;
        connection[PRO_CONNECTION][PRO_CONNECTION_UUID] = uuid;
        connection[PRO_CONNECTION][PRO_CONNECTION_TYPE] = PRO_802_11_WIRELESS;
        connection[PRO_CONNECTION][PRO_CONNECTION_ID] = QString("Auto %1")
                .arg(properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid].toString());
        
        
        /*': {u'mac-address': [0, 38, 198, 101, 175, 58],
                      u'mode': u'infrastructure',
                      u'security': u'802-11-wireless-security',
                      u'seen-bssids': [u'14:E6:E4:4E:15:78'],
                      u'ssid': [115, 114, 118]},*/
        
        connection[PRO_802_11_WIRELESS][PRO_802_11_WIRELESS_ssid] 
                = properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid];
        
        QDBusInterface interface(DBUS_NET_SERVICE, DBUS_NET_PATH_SETTINGS, 
                                 DBUS_NET_IFACE_SETTINGS, QDBusConnection::systemBus());
        
        QDBusReply<QDBusObjectPath> reply = interface.call(QString(DBUS_NET_IFACE_SETTINGS_AddConnection), 
                       QVariant::fromValue(connection));
        
        if(reply.isValid()) {
            //get settings 
            QDBusObjectPath path = reply.value();
            setting = new Setting(path.path());
        }
        else {
            qDebug()<<reply.error().message()<<"   create error "<<reply.error().name();
        }
        
    }
    
private :
    uint32_t count;
Q_SIGNALS:
    void propertyChanged(QString key, QVariant value);
};
}
#endif // ACCESSPOINT_H
