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
    
    process = new Runner(this);
    network = new Network(this);
    watchDog = new WatchDog(this);
    
    page()->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanCloseWindows, false);
    page()->settings()->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
    
    //export self and network to javascript
    connect(page()->mainFrame(), &QWebFrame::javaScriptWindowObjectCleared, [=](){
        page()->mainFrame()->addToJavaScriptWindowObject(QString("Operator"), this);
        //expode the interface to javascript
        page()->mainFrame()->addToJavaScriptWindowObject(QString("Network"), network);
    });
    
    //load network data after web page loadfinished
    connect(this, &Desktop::loadFinished, [=](bool f) {
        if(f) {
            network->load();
        }
    });
    
    //insert items to ui
    connect(network, &Network::loadFinished, [=] (){
        QList<Setting *> tmpSetting = network->getSettings();
        for(int i = 0;i < tmpSetting.length();i ++) {
            insertItem(tmpSetting.at(i));
        }
    });
    connect(network, &Network::accessPoint, [=](AccessPoint *ap, bool fg) {
        //if true, insert, else remove
        if(fg) {
            //insert
            insertItem(ap);
        }
        else {
            //remove
            emit removeItem(ap->getUuid());
        }
    });
    connect(watchDog, &WatchDog::speed, this, &Desktop::speed);
    
    
    load(QUrl("qrc:/desktop.html"));
}

Desktop::~Desktop(){
    delete process;
    delete network;
    delete watchDog;
}

//void Desktop::keyPressEvent(QKeyEvent *event) {
//    if(event->key() == 16777216 ) {
//        qApp->quit();
//    }
//}

//void Desktop::mouseMoveEvent(QMouseEvent *event) {
//    if(event->x() > 60 && event->x() < width() - 300){
//        emit panelPopup("all", false);
//    }
//    else if(event->y() <= height()/4 || event->y() >= 3 * height()/4) {
//        if(event->x() <= 1) {
//            emit panelPopup("left", true);
//        } 
//        else if(event->x() >= width() - 1) {
//            emit panelPopup("right", true);
//        }
//    }
//}

/**
 * @brief Desktop::run this function used to analyze .desktop application and run the app
 * @param app
 */
void Desktop::run(QString app) {
    process->run(app);
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
void Desktop::insertItem(Setting *set) {
    Json tmp = set->getSettings();
    QString script = QString("js_insertItem(\"%1\", \"%2\", \"%3\", %4,  0)")
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_ID].toString())
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_UUID].toString())
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_TYPE].toString())
            .arg(tmp[PRO_CONNECTION][PRO_CONNECTION_AUTOCONNECT].toBool());
    page()->mainFrame()->evaluateJavaScript(script);
}

void Desktop::insertItem(AccessPoint *accessPoint) {
    QString script = QString("js_insertItem(\"%1\", \"%2\", \"%3\", %4, %5)")
            .arg(accessPoint->getProperty(DBUS_NET_INTERFACE_ACCESS_POINT_Ssid).toString())
            .arg(accessPoint->getUuid())
            .arg("802-11-wireless")
            .arg(accessPoint->encrypt())
            .arg(accessPoint->getProperty(DBUS_NET_INTERFACE_ACCESS_POINT_Strength).toUInt());
    page()->mainFrame()->evaluateJavaScript(script);
}

void Desktop::nmEditor() {
    /**  the command to lunch nm-connection-editor is nm-connection-editor */
    process->run(NM_EDIROT);
}