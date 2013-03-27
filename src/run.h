#ifndef RUN_H
#define RUN_H

#include <QProcess>
#include <QMap>
#include <QMapIterator>
#include <QUuid>

class Runner : public QObject  {
    Q_OBJECT
public:
    Runner(QObject *parent = NULL) : QObject(parent) {}
    ~Runner() {
        QMapIterator<QString, QProcess *> iterator(running);
        while(iterator.hasNext()) {
            iterator.next();
            delete iterator.value();
            running[iterator.key()] = NULL;
        }
        running.clear();
    }
    
    QMap<QString, QProcess *> running;
public Q_SLOTS:
    void run(QString app) {
        if(app.isNull() || app.isEmpty())
            return ;
        QString id = QUuid::createUuid().toString().remove('{').remove('}');
        running[id] = new QProcess(this);
        
        connect(running[id], &QProcess::destroyed, [=](QObject *obj){
            delete running[id];
            running.remove(id);
        });
        running[id]->start(app);
    }
};

#endif // RUN_H
