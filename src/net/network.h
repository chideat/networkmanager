#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QDBusObjectPath>
#include "../types.h"

#include "wired.h"
#include "wireless.h"

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
public :
    Network(QObject *parent = NULL);
    ~Network();
    
    enum C_TYPE {
        NONE,
        WIRED,
        WIRELESS,
        PPPoE
    };
    
    Json settings;
    Arr_Str devices;
    
    static void enableNetwork(bool f);
    static void enableWireless(bool f);
    
protected:
    bool networkUp;
    bool wirelessUp;
    bool wirelessHardwareUp;
    
    C_TYPE getConnectionType(QString type);
    Json getSettings(QDBusObjectPath &op);
    
protected Q_SLOTS:
    void newConnection(QDBusObjectPath op);
    void connectionRemoved();
    
    
};
/**
 * Questions
 * 1.how to select the device?
 *  use connection's type
 */

}

#endif // NETWORK_H
