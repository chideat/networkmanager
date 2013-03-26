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
    // use device type to determine the conection
    for(int i = 0;i < devices.length(); i ++) {
        if(devices[i]->getDeviceType() == DEVICE_TYPE_WIFI){
            //get access point and 
            QDBusConnection::systemBus()
                    .connect(QString(DBUS_NET_SERVICE), QString(devices[i]->getDevice()), 
                             QString(DBUS_NET_INTERFACE_WIRELESS), 
                             QString(DBUS_NET_INTERFACE_WIRELESS_SIGNAL_AccessPointAdded), 
                             this, SLOT(accessPointAdded(QDBusObjectPath)));
            QDBusConnection::systemBus()
                    .connect(QString(DBUS_NET_SERVICE),  QString(devices[i]->getDevice()), 
                             QString(DBUS_NET_INTERFACE_WIRELESS), 
                             QString(DBUS_NET_INTERFACE_WIRELESS_SIGNAL_AccessPointRemoved), 
                             this, SLOT(accessPointRemoved(QDBusObjectPath)));
        }
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
    if(tmp->get(PRO_CONNECTION, PRO_CONNECTION_TYPE) == PRO_802_11_WIRELESS) 
        settingsWirless<<tmp;
    else 
        settings<<tmp;
    connect(tmp, &Net::Setting::removed, [=]() {
        Net::Setting *tmp = qobject_cast<Net::Setting *>(sender());
        if(tmp->get(PRO_CONNECTION, PRO_CONNECTION_TYPE) == PRO_802_11_WIRELESS)
            settingsWirless.removeAt(settingsWirless.indexOf(tmp));
        else 
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

/**
 * @brief Net::Network::tryConnect 
 *              this function used to active the connection. 
 * @param u - uuid
 */
void Net::Network::tryConnect(QString u) {
    QDBusObjectPath path;
    Net::Setting *setting = NULL;
    for(Net::Setting *tmp : settings) {
        if(tmp->getSettings()[PRO_CONNECTION][PRO_CONNECTION_UUID] == u) {
            //get object path
            path.setPath(tmp->path);
            setting = tmp;
            break;
        }
    }
    if(path.path().isNull() || path.path().isEmpty()) {
        for(Net::AccessPoint *ap : accessPoints) {
            if(ap->getSetting() != NULL && ap->getUuid() == u) {
                path.setPath(ap->getSetting()->path);
                setting = ap->getSetting();
                break;
            }
            else if(ap->getUuid() == u && ap->getSetting() == NULL) {
                //create settings from accesspoint
                ap->createConnection();
                path.setPath(ap->getSetting()->path);
                setting = ap->getSetting();
                break;
            }
        }
    }
    if(setting == NULL) return ;
    switch (getConnectionType(
                setting->getSettings()[PRO_CONNECTION][PRO_CONNECTION_TYPE].toString())) {
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
            setting->setActivePath(activedPath.path());
        }
        break;
    }
    case Net::Network::WIRELESS: {
        // here if the wireless connection is not created, then create and active
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
            setting->setActivePath(activedPath.path());
        }
        break;
    }
    case Net::Network::PPPoE : {
        // check device state 
        // first 802-3-ethernet network
        // ethernet first, wireless later
        QList<Net::Device *> list;
        for(int i  = 0; i < devices.length(); i++) {
            if(devices[i]->getDeviceType() == DEVICE_TYPE_WIFI)
                list << devices[i];
            else 
                list.insert(0, devices[i]);
        }
        
        for(int i = 0; i < list.length();i ++) {
            Net::Device *d = list.at(i);
            if(!d->isActived())
                continue;
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
                setting->setActivePath(activedPath.path());
            }
        }
        break;
    }
    default:{
        _notify("", Notification::Info, "Networkmanager", QString("%1 connection failed")
                .arg(setting->getSettings()[PRO_CONNECTION][PRO_CONNECTION_ID].toString()));
        break;
    }
    }
    
    
    /* get connection type which used to decide which device it used
              * include 802-3-ethernet, 802-11-wireless, pppoe 
              * 802-3-ethernet use NM_DEVICE_TYPE_ETHERNET = 1
              * 802-11-wireless use  NM_DEVICE_TYPE_WIFI = 2
              * pppoe use NM_DEVICE_TYPE_ETHERNET and NM_DEVICE_TYPE_WIFI
              */
}

QString Net::Network::getDevice(DEVICE_TYPE t) {
    for(int i = 0;i < devices.length(); i ++) {
        if(devices[i]->getDeviceType() == t) 
            return devices[i]->getDevice();
    }
    return QString();
}

void Net::Network::accessPointAdded(QDBusObjectPath path) {
    AccessPoint *tmp = new AccessPoint(path.path(), this);
    for(int i = 0;i < settingsWirless.length(); i++) {
        // ssid or bssid?
        if(settingsWirless[i]->get(PRO_802_11_WIRELESS, PRO_802_11_WIRELESS_ssid) == 
                tmp->getProperty(DBUS_NET_INTERFACE_ACCESS_POINT_Ssid)) {
            tmp->setSetting(settingsWirless[i]);
            settingsWirless.removeAt(i);
        }
    }
    accessPoints<<tmp;
    connect(tmp, &AccessPoint::propertyChanged, [=](QString key, QVariant value) {
        emit accessPointProperty(tmp->getUuid(), key, value);
        qDebug()<<tmp->getUuid();
    });
    emit accessPoint(tmp, true);
}

void Net::Network::accessPointRemoved(QDBusObjectPath path) {
    for(int i = 0; i < accessPoints.length(); i ++) {
        if(accessPoints[i]->getPath() == path.path()) {
            emit accessPoint(accessPoints[i], false);
            if(accessPoints[i]->getSetting() != NULL) 
                settingsWirless<<accessPoints[i]->getSetting();
            delete accessPoints[i];
            accessPoints.removeAt(i);
            break;
        }
    }
}
