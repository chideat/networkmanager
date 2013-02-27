#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QStringList>
#include <QMap>
#include <QVariant>

class Notification : public QObject {
    Q_OBJECT
public:
    Notification(QObject *parent = NULL):QObject(parent){}
    ~Notification(){}
#define NT_FUNCTION "Notify"
    
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
    
    static unsigned int notify(QString name, unsigned int replace_id, Category cate, 
                               QString summary, QString body, 
                               QStringList actions = QStringList(), 
                               QMap<QString, QVariant> hint = QMap<QString, QVariant>(), 
                               int timeout = 3);
signals :
};

#endif // NOTIFICATION_H
