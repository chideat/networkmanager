#include "gui/window.h"
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>

#define WIDTH 300

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setApplicationName("networkmanager");
    app.setOrganizationName("分享zhe");
    QRect rect = app.desktop()->screenGeometry();
    Window *window = new Window;
    window->setGeometry(rect.width() - WIDTH, 24, WIDTH, rect.height() - 24);
    
    window->setAttribute (Qt::WA_ShowModal,true);
    window->setWindowOpacity (1);
    window->setWindowFlags (Qt::FramelessWindowHint);
    window->setAttribute (Qt::WA_TranslucentBackground);
    window->show();
    return app.exec();
}
