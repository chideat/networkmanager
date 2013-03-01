#ifndef WIRED_H
#define WIRED_H

#include <QObject>
#include "connection.h"

namespace Net {

/**
 * @brief The Wired class used to load the device specified information as MAC address
 * and it combined the information of connection, ipv4, ipv6 and secure together. 
 */
class Wired:public QObject {
    Q_OBJECT
public :
    Wired(QObject *parent = NULL): QObject(parent) {}
    ~Wired(){}
};

}

#endif //WIRED_H