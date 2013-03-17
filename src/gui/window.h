#ifndef WINDOW_H
#define WINDOW_H

#include <QWebView>
#include <QProcess>

#define NOTIFICAION_REPLACE_ID 2048
#include "../notification.h"
#include "../net/network.h"

using namespace Net;

class Window: public QWebView {
    Q_OBJECT
public:
    Window(QWebView *parent = NULL);
    ~Window();
    Network *network;
public Q_SLOTS:
    void popup(QUrl &url);
    void insertItem(Setting *set);
    void insertItem(AccessPoint *accessPoint);
    void nmEditor();
    
protected:
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent* event);
    QProcess *process;
Q_SIGNALS:
    void status(QString s);
    void removeItem(QString uuid);
};

#endif // WINDOW_H