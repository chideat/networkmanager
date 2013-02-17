#include "window.h"
#include <QWebFrame>

Window::Window(QWebView *parent) : QWebView(parent) {
    setFixedSize(300, 500);
    setContextMenuPolicy(Qt::NoContextMenu);
    
    page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    
    connect(page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, [=](){
        page()->mainFrame()->addToJavaScriptWindowObject(QString("QObject"), this);
    });
    connect(this, &Window::loadFinished,[=](){
        //here load the necessary device info
        
    });
    
    //load html template
    load(QUrl("qrc:/index.html"));
}
