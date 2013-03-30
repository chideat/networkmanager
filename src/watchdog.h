#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <QStringList>
#include <QMap>
#include <QMapIterator>
#include <QFile>
#include <QTimerEvent>
#include <QDebug>

#define NET_PATH "/proc/net/dev"

class WatchDog: public QObject {
    Q_OBJECT
public :
    WatchDog(QObject *parent = NULL) : QObject(parent) {
        receive = new QMap<QString, QMap<QString, int> *>;
        transmit = new QMap<QString, QMap<QString, int> *>;
        timeInterval = 1000;
        steps = 500;
        this->startTimer(timeInterval);
        rec = 0;
        tran = 0;
        
        analyze();
    }
    
    ~WatchDog() {
        for(QMap<QString, int> * value: receive->values()) {
            delete value;
        }
        for(QMap<QString, int> * value: transmit->values()) {
            delete value;
        }
        receive->clear();
        transmit->clear();
        delete receive;
        delete transmit;
    }
    
    QMap<QString, QMap<QString, int> *> *receive;
    QMap<QString, QMap<QString, int> *> *transmit;
    
    int timeInterval;
    int steps;
    /**
 * @brief analyze
 * @return <device, <key, value> > 
 */
    void  analyze() {
        QFile file(QString(NET_PATH));
        file.open(QIODevice::ReadOnly);
        QStringList list = QString(file.readAll()).split('\n', QString::SkipEmptyParts);
        file.close();
        
        if(list.isEmpty()) 
            return;
        
        QList<QString> heads;
        
        for(QString line: list) {
            if(line.isNull()  || line.isEmpty()) 
                continue;
            if(line.contains("Receive") || line.contains("Transmit")) 
                continue;
            else if(line.contains("face")) {
                for(QString titles: line.remove(0, line.indexOf('|') + 1).split(QChar('|'), QString::SkipEmptyParts)) {
                    for(QString title: titles.split(QChar(' '), QString::SkipEmptyParts)) {
                        heads<<title;
                    }
                    heads<<"#";
                }
                heads.removeLast();
            }
            else {
                QStringList items = line.split(QChar(':'));
                if(items.isEmpty() || items.size() < 2)
                    continue;
                
                int i = 0;
                QMap<QString, int> *container;
                if(receive->contains(items[0])) 
                    container = (*receive)[items[0]];
                else 
                    container = new QMap<QString, int>;
                for(QString item: items[1].split(QChar(' '), QString::SkipEmptyParts)) {
                    if(heads[i] == "#") {
                        if(!receive->contains(items[0]))
                            (*receive)[items[0]] = container;
                        if(transmit->contains(items[0]))
                            container = (*transmit)[items[0]];
                        else 
                            container = new QMap<QString, int>;
                        i++;
                    }
                    (*container)[heads[i]] = item.toInt();
                    i++;
                }
                if(!transmit->contains(items[0]))
                    transmit->insert(items[0], container);
            }
        }
    }
    
protected:
    void timerEvent(QTimerEvent *event){
        selection();
        emit speed(upload/100.0, download/100.0);
    }
    
private: 
    int rec;
    int tran;
    int upload;
    int download; 
    /**
     * @brief selection 
     *  only select necessary parts
     */
    void selection() {
        int tmp_r = rec;
        int tmp_t = tran;
        rec = 0;
        tran = 0;
        analyze();
        for(QString inter: receive->keys()) {
            if(inter == "lo" || inter == "loop")
                continue;
            rec += (*receive)[inter]->value("bytes");
            tran +=(*transmit)[inter]->value("bytes");
        }
        
        tmp_r = 100 * (rec - tmp_r) / timeInterval;
        tmp_t =100 *  (tran - tmp_t)  / timeInterval;
        
        upload = tmp_t;
        download = tmp_r;
    }
Q_SIGNALS:
    void speed(double upload, double download);
    
};

#endif // WATCHDOG_H