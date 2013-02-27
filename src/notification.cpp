#include "notification.h"
#include <QtDBus>



unsigned int Notification::notify(QString name, unsigned int replace_id, Category cate, QString summary, QString body, QStringList actions, QMap<QString, QVariant> hint, int timeout) {
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
    
    interface.call(QLatin1String(NT_FUNCTION), QVariant::fromValue(name), QVariant::fromValue(replace_id),
                                                    QVariant::fromValue(icon), QVariant::fromValue(summary), QVariant::fromValue(body), 
                                                    QVariant::fromValue(actions), QVariant::fromValue(hint), QVariant::fromValue(timeout));
    return 0;
}