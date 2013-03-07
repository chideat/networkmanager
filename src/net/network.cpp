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
}

Net::Network::~Network() {
    for(int i = 0; i < devices.length(); i ++) {
        delete devices.at(i);
    }
    devices.clear();
    
    for(int i = 0;i < settings.length(); i++) {
        delete settings[i];
    }
    settings.clear();
}


void Net::Network::load() {
    
    //get device state
    networkUp = false;
    wirelessUp = false;
    wirelessHardwareUp = false;
    Arr_Var result_pro = getProperties(DBUS_NET_INTERFACE);
    if(!result_pro.isEmpty()) {
        networkUp = result_pro[DBUS_NET_INTERFACE_NetworkingEnabled].toBool();
        wirelessUp = result_pro[DBUS_NET_INTERFACE_WirelessEnabled].toBool();
        wirelessHardwareUp = result_pro[DBUS_NET_INTERFACE_WirelessHardwareEnabled].toBool();
    }
    //get device
    getDevices();
    //get connections 
    getConnections();
    
    QDBusConnection::systemBus()
            .connect(QString(DBUS_NET_SERVICE), QString(DBUS_NET_PATH_SETTINGS), QString(DBUS_NET_IFACE_SETTINGS),
                     QString(DBUS_NET_IFACE_SETTINGS_SIGNAL_NewConnection),
                     this, SLOT(newConnection(QDBusObjectPath)));
    
    //tell the front end it's time to load the settings
    emit loadFinished();
}

/**
 * @brief Net::Network::getProperties
 * @param inter
 * @return
 * get all properties with getall 
 */
Arr_Var Net::Network::getProperties(QString inter) {
    QDBusInterface interface(
                DBUS_NET_SERVICE,
                DBUS_NET_PATH,
                DBUS_NET_PROPERTIES,
                QDBusConnection::systemBus());
    QDBusReply<Arr_Var>  pros;
    counter = 0;
    do {
        pros = interface.call(QLatin1String(DBUS_NET_PROPERTIES_GetAll), inter);
    }while(!pros.isValid() && counter++ < COUNT);
    
    if(pros.isValid()) {
        return  pros.value();
    }
    else {
        return Arr_Var();
    }
}

void Net::Network::getDevices() {
    QDBusInterface interface(
                DBUS_NET_SERVICE,
                DBUS_NET_PATH,
                DBUS_NET_INTERFACE,
                QDBusConnection::systemBus());
    //get device
    QDBusReply<QList<QDBusObjectPath> > reply_device;
    counter = 0;
    do {
        reply_device = interface.call(QLatin1String(DBUS_NET_INTERFACE_GetDevices));
    }while(!reply_device.isValid() && counter++ < COUNT);
    if(reply_device.isValid()) {
        foreach(QDBusObjectPath path, reply_device.value()) {
            devices<<(new Net::Device(path.path()));
        }
    }
}

void Net::Network::getConnections() {
    QDBusInterface interface(
                DBUS_NET_SERVICE,
                DBUS_NET_PATH_SETTINGS,
                DBUS_NET_IFACE_SETTINGS,
                QDBusConnection::systemBus());
    QDBusReply<QList<QDBusObjectPath> > reply; 
    counter = 0;
    do {
        reply = interface.call(QLatin1String(DBUS_NET_IFACE_SETTINGS_ListConnections));
    }while(!reply.isValid() && counter++ < COUNT);
    if(reply.isValid()) {
        //foreach settings and store them to the setting
        foreach(QDBusObjectPath conn, reply.value()) {
            addConnection(conn.path());
        }
    }
}

Net::Network::C_TYPE Net::Network::getConnectionType(QString &type) {
    if(type == CONNECTION_TYPE_802_3_ETHERNET) 
        return Net::Network::WIRED;
    else if(type == CONNECTION_TYPE_802_11_WIRELESS)
        return Net::Network::WIRELESS;
    else if(type == CONNECTION_TYPE_PPPoE) 
        return Net::Network::PPPoE;
    return Net::Network::NONE;
}


void Net::Network::addConnection(QString path) {
    Net::Setting *tmp = new Net::Setting(path);
    settings<<tmp;
    connect(tmp, &Net::Setting::removed, [=]() {
        Net::Setting *tmp = qobject_cast<Net::Setting *>(sender());
        settings.removeAt(settings.indexOf(tmp));
        delete tmp;
    });
}

void Net::Network::newConnection(QDBusObjectPath path) {
    addConnection(path.path());
}

void Net::Network::enableNetwork(bool f) {
    if(!networkUp) {
        f;
    }
}

void Net::Network::enableWireless(bool f) {
    if(wirelessHardwareUp && f) {
        
    }
    else if(wirelessHardwareUp && !f) {
        
    }
}

//here send connect reference
void Net::Network::tryConnect(QString u) {
    for(int i = 0;i < settings.length(); i ++) {
        if(settings[PRO_CONNECTION][PRO_CONNECTION_UUID] == u) {
            
            break;
        }
    }
}
