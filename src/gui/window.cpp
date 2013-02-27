#include "window.h"
#include <QWebFrame>
#include <QMouseEvent>
#include <QDebug>
#include <QThread>

Window::Window(QWebView *parent) : QWebView(parent) {
    setWindowFlags(Qt::WindowStaysOnTopHint | this->windowFlags());
    
    setContextMenuPolicy(Qt::NoContextMenu);
    page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    //    connect(page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, [=](){
    //        page()->mainFrame()->addToJavaScriptWindowObject(QString("Q_Operator"), this);
    //        //expode the interface to javascript
    //        page()->mainFrame()->addToJavaScriptWindowObject(QString("Q_Notify"), notify);
    //    });
    
    //    connect(this, &Window::)
    //load html template
    load(QUrl("qrc:/index.html"));
    
}

void Window::focusOutEvent(QFocusEvent*event) {
    if(event->type() == QFocusEvent::FocusOut) {
        hide();
    }
}

void Window::focusInEvent(QFocusEvent *event) {
    if(event->type() == QFocusEvent::FocusIn)
        show();
    
}
