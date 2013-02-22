#ifndef WINDOW_H
#define WINDOW_H

#include <QWebView>
#include "../notification.h"
class Window: public QWebView {
    Q_OBJECT
public:
    Window(QWebView *parent = NULL);
    ~Window(){}
    static Notification notify;
   
signals:
    void status(QString s);
};




#endif // WINDOW_H