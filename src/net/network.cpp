#include "network.h"
#include "networkmanager.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QDebug>
#include "../notification.h"
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
        wirelessHardwareUp = 
                result_pro[DBUS_NET_INTERFACE_WirelessHardwareEnabled].toBool();
    }
    //get device
    getDevices();
    //get connections 
    getConnections();
    
    QDBusConnection::systemBus()
            .connect(QString(DBUS_NET_SERVICE), QString(DBUS_NET_PATH_SETTINGS), 
                     QString(DBUS_NET_IFACE_SETTINGS),
                     QString(DBUS_NET_IFACE_SETTINGS_SIGNAL_NewConnection),
                     this, SLOT(newConnection(QDBusObjectPath)));
    
    //connections access point
    for(int i = 0;i < devices.length(); i ++) {
        QDBusInterface interface(DBUS_NET_SERVICE,devices[i]->device, 
                                 DBUS_NET_PROPERTIES, QDBusConnection::systemBus());
        
        
    }
    
    
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

Net::Network::C_TYPE Net::Network::getConnectionType(QString type) {
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
    QDBusInterface interface(
                DBUS_NET_SERVICE, DBUS_NET_PATH, DBUS_NET_INTERFACE,
                QDBusConnection::systemBus());
    interface.call(QString(DBUS_NET_INTERFACE_Enable), 
                   QVariant(f));
}

void Net::Network::enableWireless(bool f) {
    QDBusInterface interface(
                DBUS_NET_SERVICE, DBUS_NET_PATH, DBUS_NET_PROPERTIES,
                QDBusConnection::systemBus());
    if(wirelessHardwareUp) {
        //enable wireless network
        interface.call(QString(DBUS_NET_PROPERTIES_Set), QString(DBUS_NET_INTERFACE), 
                                              QVariant(DBUS_NET_INTERFACE_WirelessEnabled), QVariant::fromValue(QDBusVariant(f)));
    }
}

//here send connect reference
void Net::Network::tryConnect(QString u, bool flag) {
    for(int i = 0;i < settings.length(); i ++) {
        Net::Setting *tmp = settings[i];
        if(tmp->getSettings()[PRO_CONNECTION][PRO_CONNECTION_UUID] == u) {
            //get object path
            QDBusInterface interface(DBUS_NET_SERVICE, DBUS_NET_PATH_SETTINGS,
                                     DBUS_NET_IFACE_SETTINGS, QDBusConnection::systemBus());
            
            QDBusReply<QDBusObjectPath> reply = interface.call(
                        QString(DBUS_NET_IFACE_SETTINGS_GetConnectionByUuid),
                        QVariant::fromValue(u));
            if(!reply.isValid()) {
                return ;
            }
            QDBusObjectPath path = reply.value();
            switch (getConnectionType(
                        tmp->getSettings()[PRO_CONNECTION][PRO_CONNECTION_TYPE]
                        .toString())) {
            case Net::Network::WIRED: {
                QDBusObjectPath device(getDevice(DEVICE_TYPE_ETHERNET));
                QDBusObjectPath specific(DBUS_NET_PATH_SPECIFIC);
                QDBusInterface conn(DBUS_NET_SERVICE, DBUS_NET_PATH, 
                                    DBUS_NET_INTERFACE,QDBusConnection::systemBus());
                QDBusReply<QDBusObjectPath> actived = conn.call(
                            QString(DBUS_NET_INTERFACE_ActivateConnection),
                            QVariant::fromValue(path), QVariant::fromValue(device),  
                            QVariant::fromValue(specific));
                if(actived.isValid()) {
                    QDBusObjectPath activedPath = actived.value();
                    //tmp->getSettings()[DBUS_NET_INTERFACE_ActivateConnection] = activedPath.path();
                }
                break;
            }
            case Net::Network::WIRELESS: {
                QDBusObjectPath device(getDevice(DEVICE_TYPE_WIFI));
                QDBusObjectPath specific(DBUS_NET_PATH_SPECIFIC);
                QDBusInterface conn(DBUS_NET_SERVICE, DBUS_NET_PATH, 
                                    DBUS_NET_INTERFACE, QDBusConnection::systemBus());
                QDBusReply< QDBusObjectPath> actived = conn.call(
                            QString(DBUS_NET_INTERFACE_ActivateConnection), 
                            QVariant::fromValue(path), QVariant::fromValue(device), 
                            QVariant::fromValue(specific));
                if(actived.isValid()) {
                    QDBusObjectPath activedPath = actived.value();
                   //tmp->getSettings()[DBUS_NET_INTERFACE_ActivateConnection] = activedPath.path();
                }
                break;
            }
            case Net::Network::PPPoE : {
                //check device state 
                //first 802-3-ethernet network
                for(int i = 0; i < devices.length();i ++) {
                    Net::Device *d = devices.at(i);
                    if(d->getProperties()[DEVICE_State].toUInt() == DEVICE_STATE_ACTIVATED ) {
                        QDBusObjectPath device(d->getDevice());
                        QDBusObjectPath specific(DBUS_NET_PATH_SPECIFIC);
                        QDBusInterface conn(DBUS_NET_SERVICE, DBUS_NET_PATH, 
                                            DBUS_NET_INTERFACE, QDBusConnection::systemBus());
                        QDBusReply< QDBusObjectPath> actived = conn.call(
                                    QString(DBUS_NET_INTERFACE_ActivateConnection), 
                                    QVariant::fromValue(path), QVariant::fromValue(device), 
                                    QVariant::fromValue(specific));
                        if(actived.isValid()) {
                            QDBusObjectPath activedPath = actived.value();
                            //tmp->getSettings()[DBUS_NET_INTERFACE_ActivateConnection] = activedPath.path();
                        }
                    }
                }
                break;
            }
            default:{
                _notify("", Notification::Info, "Networkmanager", QString("%1 connection failed")
                        .arg(tmp->getSettings()[PRO_CONNECTION][PRO_CONNECTION_ID].toString()));
                break;
            }
            }
            
            
            /* get connection type which used to decide which device it used
              * include 802-3-ethernet, 802-11-wireless, pppoe 
              * 802-3-ethernet use NM_DEVICE_TYPE_ETHERNET = 1
              * 802-11-wireless use  NM_DEVICE_TYPE_WIFI = 2
              * pppoe use NM_DEVICE_TYPE_ETHERNET and NM_DEVICE_TYPE_WIFI
              */
            
            break;
        }
    }
}

QString Net::Network::getDevice(DEVICE_TYPE t) {
    for(int i = 0;i < devices.length(); i ++) {
        if(devices[i]->getDeviceType() == t) 
            return devices[i]->getDevice();
    }
    return QString();
}
