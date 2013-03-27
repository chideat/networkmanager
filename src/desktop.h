#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWebView>
#include "net/network.h"
#include "run.h"
#define NOTIFICAION_REPLACE_ID 2048

using namespace Net;

class Desktop: public QWebView {
    Q_OBJECT
public :
    Desktop(QWebView *parent = NULL);
    ~Desktop();
    
    Network *network;
public Q_SLOTS:
    void run(QString app);    
    void insertItem(Setting *set);
    void insertItem(AccessPoint *accessPoint);
    void nmEditor();
    
protected: 
    void keyPressEvent(QKeyEvent *event);
    //void mouseMoveEvent(QMouseEvent *event);
    
Q_SIGNALS:
    //void panelPopup(QString position, bool show);
    void removeItem(QString uuid);
private : 
    Runner *process;
};


#endif // DESKTOP_H
