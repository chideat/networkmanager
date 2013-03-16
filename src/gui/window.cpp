#include "window.h"
#include <QWebFrame>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QDebug>
#include "../types.h"
#include "../net/networkmanager.h"

Window::Window(QWebView *parent) : QWebView(parent) {
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::Popup|Qt::Tool);
    setContextMenuPolicy(Qt::NoContextMenu);
    
    network = new Network(this);
    process = new QProcess(this);
    
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
    
    connect(process, &QProcess::started, [=](){
        this->hide();
    });
    
    //insert items to ui
    connect(network, &Network::loadFinished, [=] (){
        QList<Setting *> tmpSetting = network->getSettings();
        for(int i = 0;i < tmpSetting.length();i ++) {
            insertItem(tmpSetting.at(i));
        }
    });
    
    load(QUrl("qrc:/index.html"));
}

Window::~Window() {
    delete process;
    delete network;
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

void Window::keyPressEvent(QKeyEvent *event) {
    if(event->key() == 16777268 ) {
        this->reload();
    }
    else if(event->key() == 16777216 ) {
        qApp->quit();
    }
}

/**
 * @brief Window::insertItem
 * @param set
 * export necessory information to javascript. information include :
 ****************************
 * connection
 * id:
 * type:
 * uuid:
 * autoconnect
 * zone
 */
void Window::insertItem(Setting *set) {
    Json tmp = set->getSettings();
    QString script = QString("js_insertItem(\"%1\", \"%2\", \"%3\", %4)")
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_ID].toString())
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_UUID].toString())
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_TYPE].toString())
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_AUTOCONNECT].toBool());
    page()->mainFrame()->evaluateJavaScript(script);
}


void Window::nmEditor() {
    /**  the command to lunch nm-connection-editor is nm-connection-editor
     */
    process->start(NM_EDIROT);
}
