#ifndef WINDOW_H
#define WINDOW_H

#include <QWebView>

#define NOTIFICAION_REPLACE_ID 2048
#include "../notification.h"

class Window: public QWebView {
    Q_OBJECT
public:
    Window(QWebView *parent = NULL);
    ~Window(){}
public Q_SLOTS:
    void popup(QUrl &url);
protected:
    void focusOutEvent(QFocusEvent* event);
signals:
    void status(QString s);
};

#endif // WINDOW_H