#ifndef NETWORK_H
#define NETWORK_H

#include <QDBusObjectPath>
#include "../types.h"
#include "networkmanager.h"
#include "device.h"
#include "setting.h"
#include "accesspoint.h"

namespace Net {
/**
 * used to identify which type the deivce is
 * and this is the core processor
 * functions as :
 *     get device list
 *     get device type
 *     create new connection
 *     load connection settings
 *     watch connection or device state 
 *     communicate with UI
 * may be there are some other things with pppoe is worked here
 * and get system's data is connected here
 */
class Network: public QObject {
    Q_OBJECT
    Q_ENUMS(C_TYPE)
    Q_PROPERTY(bool networkUp READ isNetworkUp)
    Q_PROPERTY(bool wirelessHardwareUp READ isWirelessHardwareUp)
    Q_PROPERTY(bool wirelessUp READ isWirelessUp)
    Q_PROPERTY(QList<Setting *> settings READ getSettings)
    
public :
    Network(QObject *parent = NULL);
    ~Network();
    
    enum C_TYPE {
        NONE,
        WIRED,
        WIRELESS,
        PPPoE
    };
    
    QList<Setting *> settings;
    QList<Setting *> settingsWirless;
    QList<Device *> devices;
    QList<AccessPoint *> accessPoints;
    
    bool isNetworkUp() { return networkUp; }
    bool isWirelessUp() { return wirelessUp; }
    bool isWirelessHardwareUp() { return wirelessHardwareUp; }
    
protected:
    bool networkUp;
    bool wirelessUp;
    bool wirelessHardwareUp;
    
    C_TYPE getConnectionType(QString type);
    
public Q_SLOTS:
    void load();
    void addConnection(QString path);
    void tryConnect(QString u);
    
    void enableNetwork(bool f);
    void enableWireless(bool f);
    
    void getConnections();
    void getDevices();
    QString getDevice(DEVICE_TYPE t);
    
    Arr_Var getProperties(QString inter);
    QList<Setting *> getSettings() { return settings; }
    
    void newConnection(QDBusObjectPath path);
    void accessPointAdded(QDBusObjectPath path);
    void accessPointRemoved(QDBusObjectPath path);
Q_SIGNALS:
    void loadFinished();
    void accessPoint(AccessPoint *ap, bool flag);
    void accessPointProperty(QString uuid, QString key, QVariant value);
    
private :
    uint32_t counter;
};
/**
 * Questions
 * 1.how to select the device?[2013-3-6]
 *  use connection's type
 * 2. when to load the devices settings? [2013-3-7]
 */

}

#endif // NETWORK_H
