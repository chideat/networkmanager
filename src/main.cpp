#include "gui/window.h"
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDebug>

#include "net/network.h"

#define WIDTH 300

using namespace Net;

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    
    if(!QDBusConnection::systemBus().isConnected()) {
        qWarning()<<QStringLiteral("System DBus connection failed, Please try again.");
        app.quit();
    }
    
    app.setApplicationName("networkmanager");
    app.setOrganizationName("分享zhe");
    QRect rect = app.desktop()->screenGeometry();
    
    Window *window = new Window;
    window->setGeometry(rect.width() - WIDTH, 24, WIDTH, rect.height() - 24);
    

    window->show();
    Network network;
    
    return app.exec();
}
