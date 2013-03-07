#ifndef SETTING_H
#define SETTING_H

#include <QObject>
#include "../types.h"

namespace Net {

class Setting : public QObject {
    Q_OBJECT
    Q_PROPERTY(Json settings READ getSettings)
public:
    Setting(QString s, QObject *parent = NULL);
    ~Setting(){}
    
    QString path;
    Json settings;
public Q_SLOTS:
    void _delete();
    Json getSettings() { return settings; }
    void initSettings();
    void getSecrets(QString s);
    QVariant get(QString container, QString key);
    void set(QString container, QString key, QVariant value);
    
Q_SIGNALS:
    void removed();
    void updated();
    
private:
    uint32_t counter;
};

}
#endif // SETTING_H
