#ifndef WIRELESS_H
#define WIRELESS_H

#include <QObject>

namespace Net {

class Wireless:public QObject {
    Q_OBJECT
    
public:
    Wireless(QObject *parent = NULL){}
    ~Wireless(){}
    
};


}

#endif