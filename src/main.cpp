#include <QApplication>
#include <QDBusConnection>
#include "desktop.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    if(!QDBusConnection::systemBus().isConnected()) {
        qWarning()<<QStringLiteral("System DBus connection failed, Please try again.");
        app.quit();
    }
    
    app.setApplicationName("Desktop");
    app.setOrganizationName("分享zhe");
    
    Desktop *desktop = new Desktop;
    desktop->show();
    
    return app.exec();
}