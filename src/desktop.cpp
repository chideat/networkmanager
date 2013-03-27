#include "desktop.h"
#include <QCoreApplication>
#include <QWebFrame>
#include <QKeyEvent>
#include <QUrl>
#include <QDebug>

Desktop::Desktop(QWebView *parent):QWebView(parent) {
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowState(Qt::WindowFullScreen);
    
    setMouseTracking(true);
    
    page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    
    //export self and network to javascript
    connect(page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, [=](){
        page()->mainFrame()->addToJavaScriptWindowObject(QString("Q_Desktop"), this);
    });
    
    process = new Runner(this);
    
    load(QUrl("qrc:/desktop.html"));
}

Desktop::~Desktop(){
    
}

void Desktop::keyPressEvent(QKeyEvent *event) {
    if(event->key() == 16777268 ) {
        this->reload();
    }
    else if(event->key() == 16777216 ) {
        qApp->quit();
    }
}

void Desktop::mouseMoveEvent(QMouseEvent *event) {
    if(event->x() > 60 && event->x() < width() - 300){
        emit panelPopup("all", false);
    }
    else if(event->y() <= height()/4 || event->y() >= 3 * height()/4) {
        if(event->x() <= 1) {
            emit panelPopup("left", true);
        } 
        else if(event->x() >= width() - 1) {
            emit panelPopup("right", true);
        }
    }
}

/**
 * @brief Desktop::run this function used to analyze .desktop application and run the app
 * @param app
 */
void Desktop::run(QString app) {
    process->run(app);
}
