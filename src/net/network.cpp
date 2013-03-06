#include "network.h"
#include "networkmanager.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QDebug>
/**
 * @brief Net::Network
 * @param parent
 *  init Network, get device list and get settings list
 */
Net::Network::Network(QObject *parent): QObject(parent) {
    qDBusRegisterMetaType<Json>();
    
    //get device state
    QDBusInterface inter_property(
                DBUS_NET_SERVICE,
                DBUS_NET_PATH,
                DBUS_NET_PROPERTIES,
                QDBusConnection::systemBus());
    QDBusReply<Arr_Var>  pros = inter_property.call(QLatin1String(DBUS_NET_PROPERTIES_GetAll),
                                                    DBUS_NET_INTERFACE);
    if(pros.isValid()) {
        Arr_Var result = pros.value();
        networkUp = result[DBUS_NET_INTERFACE_NetworkingEnabled].toBool();
        wirelessUp = result[DBUS_NET_INTERFACE_WirelessEnabled].toBool();
        wirelessHardwareUp = result[DBUS_NET_INTERFACE_WirelessHardwareEnabled].toBool();
        if(networkUp)
            qDebug()<<"enabled";
    }
    else {
        qDebug()<<pros.error().message();
    }
    
    
    QDBusInterface inter_device(
                DBUS_NET_SERVICE,
                DBUS_NET_PATH,
                DBUS_NET_INTERFACE,
                QDBusConnection::systemBus());

    //get device
    QDBusReply<QList<QDBusObjectPath> > reply_device = inter_device.call(QLatin1String(DBUS_NET_INTERFACE_GetDevices));
    if(reply_device.isValid()) {
        foreach(QDBusObjectPath path, reply_device.value()) {
            devices.append(path.path());
        }
    }
    else {
        qDebug()<<reply_device.error().message();
    }
    
    
    //get connections 
    QDBusInterface interface(
                DBUS_NET_SERVICE,
                DBUS_NET_PATH_SETTINGS,
                DBUS_NET_IFACE_SETTINGS,
                QDBusConnection::systemBus());
    QDBusReply<QList<QDBusObjectPath> > reply = interface.call(QLatin1String(DBUS_NET_IFACE_SETTINGS_ListConnections));
    if(reply.isValid()) {
        //foreach settings and store them to the setting
        foreach(QDBusObjectPath conn, reply.value()) {
            settings[conn.path().toHtmlEscaped()] = getSettings(conn)[PRO_CONNECTION];
            settings[conn.path().toHtmlEscaped()][PRO_CONNECTION_TYPE] 
                    = getConnectionType(settings[conn.path().toHtmlEscaped()][PRO_CONNECTION_TYPE].toString() );
        }
    }
    else {
        // get connections error
        qDebug()<<reply.error().message();
    }
    QDBusConnection::systemBus()
            .connect(QString(DBUS_NET_SERVICE), QString(DBUS_NET_PATH_SETTINGS), QString(DBUS_NET_IFACE_SETTINGS),
                     QString(DBUS_NET_IFACE_SETTINGS_SIGNAL_NewConnection),
                     this, SLOT(newConnection(QDBusObjectPath)));
    //connection remove
    QDBusConnection::systemBus()
            .connect(QString(), QString(), QString(DBUS_NET_IFACE_SETTINGS_CONNECTION),
                     QString(DBUS_NET_IFACE_SETTINGS_CONNECTION_SIGNAL_Removed),
                     this, SLOT(connectionRemoved()));
    

}

Net::Network::~Network() {}


Net::Network::C_TYPE Net::Network::getConnectionType(QString type) {
    if(type == CONNECTION_TYPE_802_3_ETHERNET) 
        return Net::Network::WIRED;
    else if(type == CONNECTION_TYPE_802_11_WIRELESS)
        return Net::Network::WIRELESS;
    else if(type == CONNECTION_TYPE_PPPoE) 
        return Net::Network::PPPoE;
    return Net::Network::NONE;
}


void Net::Network::newConnection(QDBusObjectPath op) {
    qDebug()<<op.path();
}

Json Net::Network::getSettings(QDBusObjectPath &op) {
    QDBusInterface connection(
                DBUS_NET_SERVICE,
                op.path(),
                DBUS_NET_IFACE_SETTINGS_CONNECTION,
                QDBusConnection::systemBus());
    QDBusReply<Json> r = connection.call(QLatin1String(DBUS_NET_IFACE_SETTINGS_CONNECTION_GetSettings));
    if(r.isValid()) {
        // add connection to settings
        return r.value();
    }
    else {
        qDebug()<<r.error().message();
        return Json();
    }
}

void Net::Network::connectionRemoved() {
    qDebug()<<"connection Removed";
}


void Net::Network::enableNetwork(bool f) {
    
}
