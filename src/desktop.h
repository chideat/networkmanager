#ifndef DESKTOP_H
#define DESKTOP_H

#include <QWebView>
#include "run.h"

class Desktop: public QWebView {
    Q_OBJECT
public :
    Desktop(QWebView *parent = NULL);
    ~Desktop();
public Q_SLOTS:
    void run(QString app);
    
protected: 
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
Q_SIGNALS:
    void panelPopup(QString position, bool show);
private : 
    Runner *process;
};


#endif // DESKTOP_H
