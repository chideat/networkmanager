#include "setting.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QMapIterator>
#include "networkmanager.h"


Net::Setting::Setting(QString s, QObject *parent):QObject(parent) {
    path = s;
    
    QDBusConnection::systemBus().connect(QString(DBUS_NET_SERVICE), path, 
                                         QString(DBUS_NET_IFACE_SETTINGS_CONNECTION), 
                                         QString(DBUS_NET_IFACE_SETTINGS_CONNECTION_SIGNAL_Removed), 
                                         this, SIGNAL(removed()));
    QDBusConnection::systemBus().connect(QString(DBUS_NET_SERVICE), path, 
                                         QString(DBUS_NET_IFACE_SETTINGS_CONNECTION), 
                                         QString(DBUS_NET_IFACE_SETTINGS_CONNECTION_SIGNAL_Updated),
                                         this, SIGNAL(updated()));
    
    initSettings();
}

void Net::Setting::_delete() {
    QDBusInterface interface(
                DBUS_NET_SERVICE, path, DBUS_NET_IFACE_SETTINGS,
                QDBusConnection::systemBus());
    
    interface.call(QLatin1String(DBUS_NET_IFACE_SETTINGS_CONNECTION_Delete));
}

void Net::Setting::initSettings() {
    QDBusInterface interface (
                DBUS_NET_SERVICE,
                path, DBUS_NET_IFACE_SETTINGS_CONNECTION, 
                QDBusConnection::systemBus());
    
    QDBusReply<Json> result;
    counter = 0 ;
    do {
        result = interface.call(
                    QLatin1String(DBUS_NET_IFACE_SETTINGS_CONNECTION_GetSettings));
        counter ++;
    }while(!result.isValid() && counter < COUNT);
    
    settings = result.value();
    /* here need to add getSecrets sentens */
}

void Net::Setting::getSecrets(QString s) {
    QDBusInterface interface (
                DBUS_NET_SERVICE,
                path, DBUS_NET_IFACE_SETTINGS_CONNECTION, 
                QDBusConnection::systemBus());
    QDBusReply<Json> result;
    counter = 0;
    do {
        result = interface.call(QString(DBUS_NET_IFACE_SETTINGS_CONNECTION_GetSecrets));
    }while(!result.isValid() && counter ++ < COUNT);
    QMapIterator<QString, QMap<QString, QVariant> > iter(result.value());
    while(iter.hasNext()) {
        iter.next();
        settings[s][iter.key()] = iter.value();
    }
}

QVariant Net::Setting::get(QString container, QString key) {
    return settings[container][key];
}

void Net::Setting::set(QString container, QString key, QVariant value) {
    Json tmp;
    tmp[container][key] = value;
    QDBusInterface interface(DBUS_NET_SERVICE, path, 
                             DBUS_NET_IFACE_SETTINGS_CONNECTION, 
                             QDBusConnection::systemBus());
    interface.call(QString(DBUS_NET_IFACE_SETTINGS_CONNECTION_Update),
                   QVariant::fromValue(tmp));
}
