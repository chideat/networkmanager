#include "notification.h"
#include <QDBusInterface>
#include <QDBusMetaType>

uint32_t Notification::notify(QString name, Category cate, QString summary, 
                              QString body, uint32_t replace_id, QStringList actions, 
                              QMap<QString, QVariant> hint, int timeout) {
    qDBusRegisterMetaType<QMap<QString, QVariant> >();
    static QDBusInterface interface(
                NT_DBUS_SERVICE,
                NT_DBUS_PATH,
                NT_DBUS_INTERFACE,
                QDBusConnection::sessionBus());
    QString icon;
    switch(cate) {
    case Info:
        icon = ICON_INFO; break;
    case Warning:
        icon = ICON_WARNING; break;
    case Error:
        icon = ICON_ERROR; break;
    }
    
    replace_id = replace_id != NOTIFICAION_REPLACE_ID && replace_id != 0 ? replace_id : NOTIFICAION_REPLACE_ID;
    
    interface.call(QLatin1String(NT_FUNCTION), QVariant::fromValue(name), QVariant::fromValue(replace_id),
                                                    QVariant::fromValue(icon), QVariant::fromValue(summary), QVariant::fromValue(body), 
                                                    QVariant::fromValue(actions), QVariant::fromValue(hint), QVariant::fromValue(timeout));
    
    return replace_id;
}