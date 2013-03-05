#ifndef PPPoE_H
#define PPPoE_H

#include <QObject>

namespace Net {

class PPPoE : public QObject {
    Q_OBJECT
public :
    PPPoE(QObject *parent = NULL){}
    ~PPPoE(){}
};

}

#endif