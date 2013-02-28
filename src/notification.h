#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QStringList>
#include <QMap>
#include <QVariant>
#include "types.h"

class Notification : public QObject {
    Q_OBJECT
public:
    Notification(QObject *parent = NULL):QObject(parent){}
    ~Notification(){}
#define NT_FUNCTION "Notify"
 
    //define replace id
#ifndef NOTIFICAION_REPLACE_ID
#define NOTIFICAION_REPLACE_ID 4096
#endif
    
// dbus path and interface
#define NT_DBUS_SERVICE "org.freedesktop.Notifications"
#define NT_DBUS_PATH "/org/freedesktop/Notifications"
#define NT_DBUS_INTERFACE "org.freedesktop.Notifications"
    
// default icons
#define ICON_INFO "info"
#define ICON_WARNING "dialog-warning"
#define ICON_ERROR "error"
    
    enum Category {
        Info,
        Warning,
        Error
    };
    
    static uint32_t notify(QString name, Category cate, 
                               QString summary, QString body, 
                               uint32_t replace_id = NOTIFICAION_REPLACE_ID, 
                               QStringList actions = QStringList(), 
                               QMap<QString, QVariant> hint = QMap<QString, QVariant>(), 
                               int timeout = -1);
};

#define _notify Notification::notify

#endif // NOTIFICATION_H
