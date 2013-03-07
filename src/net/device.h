#ifndef WIRED_H
#define WIRED_H

#include <QObject>
#include "../types.h"

namespace Net {

/**
 * @brief The Wired class used to load the device specified information as MAC address
 * and it combined the information of connection, ipv4, ipv6 and secure together. 
 * this is more like a listener used to watch the device state change
 */
class Device:public QObject {
    Q_OBJECT
public :
    Device(QString d, QObject *parent = NULL);
    ~Device(){}
    QString device;
    Arr_Var properties;
public Q_SLOTS:
    QString getDevice() { return device; }
    void setDevice(QString &d) { device = d; }
    
    Arr_Var getProperties();
    void stateChanged(uint32_t newState, uint32_t, uint32_t);
    void disconnect();
Q_SIGNALS:
    void stateChanged(uint32_t newState);
    void actived(bool f);
private :
    uint32_t counter;
    
    
};

}

#endif //WIRED_H