#ifndef CHECK_H
#define CHECK_H

#include <QObject>
#include <QDnsLookup>
#include <QDebug>
#include "device.h"


/**
 * @brief The Check class
 * network check includes 
 * device check
 *  connection check
 *  dns check
 *  route check
 */
class Check : public QObject {
    Q_OBJECT 
public :
    enum Network_State{
        N_OK = 0,
        N_DEV,
        N_CONN,
        N_DNS,
        N_ROUTE
    };
    Network_State state;
    Check(QList<Net::Device *> devices, QObject *parent = NULL): QObject(parent) {
        this->devices = devices;
        index = 0;
        domains << "www.baidu.com"
                <<"www.163.com"
               <<"www.google.com.hk";
        state = N_OK;
        
        dns = new QDnsLookup(this);
        connect(dns, &QDnsLookup::finished, this, &Check::dnsCheck);
    }
    
    ~ Check() {
        dns->deleteLater();
    }
    
    QDnsLookup *dns;
    QList<Net::Device *> devices;
public Q_SLOTS:
    void check(){
        connectionCheck();
        if(state != N_OK) {
            emit finished(state);
            return ;
        }
        dns->setType(QDnsLookup::ANY);
        dns->setName(domains[index ++]);
        dns->lookup();
    }
    
    /**
     * @brief device
     *  check all network devices state
     */
    void deviceCheck() {
        int i = 0;
        for(Net::Device *tmp: devices) {
            if(tmp->isActived()) 
                i ++;
        }
        if(i>0) 
            state = N_OK;
        else {
            state = N_DEV;
        }
    }
    
    /**
     * @brief connection
     * check if there exist any connection
     */
    void connectionCheck() {
        int i = 0;
        for(Net::Device *tmp: devices) {
            if(tmp->isActived()) 
                i ++;
        }
        if(i>0) 
            state = N_OK;
        else {
            state = N_DEV;
        }
    }
    
    /**
     * @brief dns
     *  use getaddrinfo to check if dns works or check if the /etc/resoluve.conf contains the necessary item
     */
    void dnsCheck() {
        if(dns->error() == QDnsLookup::NoError) {
            state = N_OK;
            routeCheck();
            emit finished(state);
        }
        else  {
            state = N_DNS;
                if(index < domains.length()) {
                dns->setType(QDnsLookup::ANY);
                dns->setName(domains[index++]);
                dns->lookup();
            }
        }
    }
    
    /**
     * @brief route
     *  just use tracroute to check if the route works
     */
    void routeCheck() {
        //only  test gateway
        state = N_ROUTE;
    }
    
Q_SIGNALS:
    void finished(Network_State state);
private :
    QList<QString> domains;
    int index;
};

#endif // CHECK_H