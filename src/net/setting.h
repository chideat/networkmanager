#ifndef SETTING_H
#define SETTING_H

#include "wired.h"
#include "wireless.h"
#include "connection.h"
#include "ppp.h"
#include "pppoe.h"

namespace Net {

class Setting : public QObject {
    Q_OBJECT
public:
    Setting(QObject *parent = NULL);
    ~Setting(){}
};

}
#endif // SETTING_H
