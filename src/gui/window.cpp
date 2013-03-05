#include "window.h"
#include <QWebFrame>
#include <QMouseEvent>

Window::Window(QWebView *parent) : QWebView(parent) {
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
    
    setContextMenuPolicy(Qt::NoContextMenu);
    page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    connect(page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, [=](){
        page()->mainFrame()->addToJavaScriptWindowObject(QString("Q_Operator"), this);
        //expode the interface to javascript
        //page()->mainFrame()->addToJavaScriptWindowObject(QString("Q_Notify"), notify);
    });
    
    QUrl url("qrc:/index.html");
    popup(url);
}

void Window::popup(QUrl &url) {
    //load html template
    //load(QUrl("qrc:/index.html"));
    load(url);
}

void Window::focusOutEvent(QFocusEvent*event) {
    if(event->type() == QFocusEvent::FocusOut) {
        //hide();
        //exit(EXIT_SUCCESS);
    }
}
