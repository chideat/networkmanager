#ifndef WINDOW_H
#define WINDOW_H

#include <QWebView>

#define NOTIFICAION_REPLACE_ID 2048
#include "../notification.h"
#include "../net/network.h"

using namespace Net;

class Window: public QWebView {
    Q_OBJECT
public:
    Window(QWebView *parent = NULL);
    ~Window(){}
    Network *network;
public Q_SLOTS:
    void popup(QUrl &url);
    
protected:
    void focusOutEvent(QFocusEvent* event);
signals:
    void status(QString s);
};

#endif // WINDOW_H