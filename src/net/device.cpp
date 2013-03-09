#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QDebug>
#include "networkmanager.h"
#include "device.h"


Net::Device::Device(QString d, QObject *parent):QObject(parent) {
    device = d;
    getProperties();
    
    QDBusConnection::systemBus().connect(QString(), QString(), DBUS_NET_INTERFACE_DEVICE, 
                                         DBUS_NET_INTERFACE_DEVICE_SIGNAL_StateChanged, this, SLOT(stateChanged(uint,uint,uint)));
}

Arr_Var Net::Device::getProperties() {
    if(properties.isEmpty()) {
        QDBusInterface interface(
                    DBUS_NET_SERVICE, device, DBUS_NET_PROPERTIES,
                    QDBusConnection::systemBus());
        QDBusReply<Arr_Var> reply;
        counter = 0;
        do {
            reply  = interface.call(DBUS_NET_PROPERTIES_GetAll,  DBUS_NET_INTERFACE_DEVICE);
            counter ++;
        }while(!reply.isValid() && counter < COUNT);
        properties = reply.value();
    }
    return properties;
}

void Net::Device::stateChanged(unsigned int newState, unsigned int /*oldState*/, unsigned int /*reason*/) {
//    getProperties();
    switch(newState) {
    case DEVICE_STATE_ACTIVATED: 
        getProperties(); 
        emit actived(true);
        break;
    case DEVICE_STATE_DISCONNECTED:
        getProperties();
        emit actived(false);
        break;
    }
}

void Net::Device::disconnect() {
    QDBusInterface interface(
                DBUS_NET_SERVICE, device, DBUS_NET_INTERFACE_DEVICE,
                QDBusConnection::systemBus());
    interface.call(QLatin1String(DBUS_NET_INTERFACE_DEVICE_Disconnect));
}

uint32_t Net::Device::getDeviceType() {
    return properties["DeviceType"].toUInt();
}
