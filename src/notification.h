#ifndef NOTIFICATION_H
#define NOTIFICATION_H

/**
 * this file defined the necessary notification functions
 */
#include <QObject>

class Notification: public QObject {
    Q_OBJECT
public :
    Notification(QObject *parent = NULL): QObject(parent) {}
    ~Notification(){}
    
    enum Level {
        Information = 0,
        Warning = 1,
        Error = 2
    };
    //send the message to window
    bool setMessage(Level l, QString m) {
        QString tmp = "<span class=";
        switch(l) {
        case Information:
            tmp  += "\"information\"";
            break;
        case Warning:
            tmp += "\"warning\"";
            break;
        case Error:
            tmp += "\"error\"";
            break;
        default:
            return false;
        }
        tmp += ">" + m + "</span>";
        emit notification(tmp);
        return true;
    }

signals: 
    void notification(QString m);
};



#endif // NOTIFICATION_H
