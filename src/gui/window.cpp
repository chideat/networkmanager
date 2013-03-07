#include "window.h"
#include <QWebFrame>
#include <QMouseEvent>
#include <QDebug>

Window::Window(QWebView *parent) : QWebView(parent) {
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
    setContextMenuPolicy(Qt::NoContextMenu);
    
    network = new Network(this);
    
    page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    
    //export self and network to javascript
    connect(page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, [=](){
        page()->mainFrame()->addToJavaScriptWindowObject(QString("Q_Operator"), this);
        //expode the interface to javascript
        page()->mainFrame()->addToJavaScriptWindowObject(QString("Q_Network"), network);
    });
    //load network data after web page loadfinished
    connect(this, &Window::loadFinished, [=](bool f) {
        if(f) {
            network->load();
        }
    });
    
    load(QUrl("qrc:/index.html"));
}

void Window::popup(QUrl &url) {
    //load html template
    load(url);
}

void Window::focusOutEvent(QFocusEvent*event) {
    if(event->type() == QFocusEvent::FocusOut) {
        //hide();
        //exit(EXIT_SUCCESS);
    }
}
