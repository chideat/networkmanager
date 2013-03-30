#ifndef ACCESSPOINT_H
#define ACCESSPOINT_H

#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusMetaType>
#include <QUuid>
#include <QMapIterator>
#include "setting.h"
#include "networkmanager.h"
#include "../types.h"
#include "setting.h"
#include <QDebug>

namespace Net {

class AccessPoint : public QObject {
    Q_OBJECT
public:
    AccessPoint(QString path, QObject *parent = NULL): QObject(parent) {
        qDBusRegisterMetaType<Arr_Var>();
        this->path = path;
        QDBusInterface interface(DBUS_NET_SERVICE, path, DBUS_NET_PROPERTIES,
                                 QDBusConnection::systemBus());
        QDBusReply<Arr_Var> reply;
        count = 0;
        do {
            reply = interface.call(QString(DBUS_NET_PROPERTIES_GetAll), 
                                   QVariant(DBUS_NET_INTERFACE_ACCESS_POINT));
        }while(!reply.isValid() && count ++ < COUNT);
        
        if(reply.isValid()) {
            properties = reply.value();
        }
        setting = NULL;
        uuid = QUuid::createUuid().toString().remove('{').remove('}');
        
        if(QDBusConnection::systemBus().connect(QString(), 
                                                QString(path), QString(DBUS_NET_INTERFACE_ACCESS_POINT), 
                                                QString(DBUS_NET_INTERFACE_ACCESSPOINT_SIGNAL_PropertiesChanged), 
                                                this, SLOT(propertiesChanged(Arr_Var))) == false) {
            qDebug()<<"connection failed"<<path;
        }
        flags = properties[DBUS_NET_INTERFACE_ACCESS_POINT_Flags].toInt();
        wpaFlags = properties[DBUS_NET_INTERFACE_ACCESS_POINT_WpaFlags].toInt();
        rsnFlags = properties[DBUS_NET_INTERFACE_ACCESS_POINT_RsnFlags].toInt();
        mode = properties[DBUS_NET_INTERFACE_ACCESS_POINT_Mode].toInt();
        
        
    }
    
    ~AccessPoint(){}
    
    Arr_Var properties;
    QString uuid;
    QString path;
    Setting *setting;
    int flags;
    int wpaFlags;
    int rsnFlags;
    int mode;
    
public Q_SLOTS:
    
    QString getUuid() { return uuid; }
    QString getPath() { return path;}
    QVariant getProperty(QString key) { return properties[key]; }
    bool encrypt() {
        if( ((flags & FLAGS_NM_802_11_AP_PRIVACY) && (wpaFlags == SEC_NONE) && (rsnFlags == SEC_NONE))
                || wpaFlags != SEC_NONE || rsnFlags != SEC_NONE)  {
            return true;
        }
        return false;
    }
    
    Arr_Var getProperties() { return properties; }
    Setting *getSetting() {return setting; }
    
    void setSetting(Setting *s) {
        setting = s; 
        uuid = s->get(PRO_CONNECTION, PRO_CONNECTION_UUID).toString();
    }
    
    void propertiesChanged(Arr_Var properties) {
        //check few properties
        QMapIterator<QString, QVariant> iterator(properties);
        while(iterator.hasNext()) {
            iterator.next();
            if(iterator.value() != this->properties[iterator.key()])  {
                emit propertyChanged(iterator.key(), iterator.value());
                this->properties[iterator.key()] = iterator.value();
            }
        }
    }
    
    QString getMode() {
        //one of 'infrastructure', 'adhoc' or 'ap'
        switch(mode) {
        case 0:
            return ""; // unknown
            break;
        case 1:
            return "adhoc";
            break;
        case 2: 
            return "infrastructure";
            break;
        case 3:
            return "ap";
            break;
        }
        return "infrastructure";
    }
    
    QString apWpaRsnFlags(int flags) {
        QStringList result;
        if (flags & NM_802_11_AP_SEC_PAIR_WEP40)
            result.append("pair_wpe40");
        if (flags & NM_802_11_AP_SEC_PAIR_WEP104)
            result.append("pair_wpe104");
        if (flags & NM_802_11_AP_SEC_PAIR_TKIP)
            result.append("pair_tkip");
        if (flags & NM_802_11_AP_SEC_PAIR_CCMP)
            result.append("pair_ccmp");
        if (flags & NM_802_11_AP_SEC_GROUP_WEP40)
            result.append("group_wpe40");
        if (flags & NM_802_11_AP_SEC_GROUP_WEP104)
            result.append("group_wpe104");
        if (flags & NM_802_11_AP_SEC_GROUP_TKIP)
            result.append("group_tkip");
        if (flags & NM_802_11_AP_SEC_GROUP_CCMP)
            result.append("group_ccmp");
        if (flags & NM_802_11_AP_SEC_KEY_MGMT_PSK)
            result.append("psk");
        if (flags & NM_802_11_AP_SEC_KEY_MGMT_802_1X)
            result.append("802.1X");
        
        if (result.length() == 0)
            result.append("none");
        
        return result.join(QChar(' '));
    }
    
    /**
     * @brief createConnection, a connection is needed for the first connection of accesspoints.
     * create the connection with the properties from the access point.
     * properties:
     *    
     */
    bool createConnection(QString password = "", QString username = "") {
        Json connection;
        connection[PRO_CONNECTION][PRO_CONNECTION_UUID] = uuid;
        connection[PRO_CONNECTION][PRO_CONNECTION_TYPE] = PRO_802_11_WIRELESS;
        connection[PRO_CONNECTION][PRO_CONNECTION_ID] = QString("Auto %1")
                .arg(properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid].toString());
        
        /*': {u'mac-address': [0, 38, 198, 101, 175, 58],
                      u'mode': u'infrastructure',
                      u'security': u'802-11-wireless-security',
                      u'seen-bssids': [u'14:E6:E4:4E:15:78'],
                      u'ssid': [115, 114, 118]},*/
        
        connection[PRO_802_11_WIRELESS][PRO_802_11_WIRELESS_ssid] 
                = properties[DBUS_NET_INTERFACE_ACCESS_POINT_Ssid];
        if(encrypt()) {
            connection[PRO_802_11_WIRELESS][PRO_802_11_WIRELESS_security]
                    = PRO_802_11_WIRELESS_security_802_11;
            connection[PRO_802_11_WIRELESS][PRO_802_11_WIRELESS_mode]
                    = getMode();
            qDebug()<<"the mode is "<< getMode();
            
            /**
             *   key-mgmt:    none(WEP), 'ieee8021x'(Dynamic WEP), 'wpa-none'(WPA-PSK Ad-Hoc)
             *  'wpa-psk'(infrasture WPA-PSK), 'wpa-eap'(WPA-Enterprise).
             *   
             *   auth-alg:   when WEP is used(key-mgmt == none or ieee8021x), 'open' for open system
             *                 'shared' for shared key, 'leap' for Cisco LEAP,  when use Cisco LEAP(key-mgmt='ieee8021x')
             *                 and 'auth-alg'='leap', the leap-username' abd 'leap-password' properties must be specified
             *    proto:    if specified, will only connect to WPA networks provide specified pairwaire encryption capabilities.
             *               one of 'wep40', 'wep104', 'tkip', 'ccmp'
             *   group:     if specified, only connect to networks the provide the specified group/multicast encryption capabilities
             *               one of 'wep40', 'wep104', 'tkip', 'ccmp'
             *    psk:        pre-shared-key for WPA network. if the key is 64-characters long, it ,must contain only hex characters
             *               and is interpreted as a hexadecimal WPA key. other wise the key must be bettween 8 and 63 ASCII char. and 
             *                       is interpreted as a WPA passphrase .
             *    psk-flag:     indicate how to handle the WPA PSK key.
             */
            //secury
            
//            connection[PRO_IPv4][PRO_IP_DNS] = Arr_UInt32();
//            connection[PRO_IPv4][PRO_IP_ADDRESS] = Arr_Str();
            connection[PRO_IPv4][PRO_IP_METHOD] = PRO_IP_METHOD_AUTO;
//            connection[PRO_IPv4][PRO_IP_ROUTES] = Arr_Str();
            
//            connection[PRO_IPv6][PRO_IP_DNS] = Arr_UInt32();
//            connection[PRO_IPv6][PRO_IP_ADDRESS] = Arr_Str();
            connection[PRO_IPv6][PRO_IP_METHOD] = PRO_IP_METHOD_AUTO;
//            connection[PRO_IPv6][PRO_IP_ROUTES] = Arr_Str();
            
            //wep
            if (   !(flags & FLAGS_NM_802_11_AP_PRIVACY) && (wpaFlags == SEC_NONE) && (rsnFlags == SEC_NONE))  {
                connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_auth_alg]="open";
                connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_key_mgmt] = "none";
                connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_wep_key0] = password;
                //connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_wep_key_type] = 1;
            }
            if ((wpaFlags &SEC_KEY_MGMT_PSK) || (rsnFlags & SEC_KEY_MGMT_PSK)) {
                if(mode == 1) {
                    //ad-hoc
                    connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_key_mgmt] = "wpa-none";
                    connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_psk] = password;
                }
                else if(mode == 2) {
                    //inf
                    connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_key_mgmt] = "wpa-psk";
                    connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_psk] = password;
                }
                if (   (wpaFlags & SEC_KEY_MGMT_802_1X) || (rsnFlags & SEC_KEY_MGMT_802_1X)) 
                    //don't know
                    connection[PRO_802_11_WIRELESS_security_802_11][PRO_802_11_WIRELESS_security_802_11_key_mgmt] = "wpa-eap";
            }
        }
        
        QDBusInterface interface(DBUS_NET_SERVICE, DBUS_NET_PATH_SETTINGS, 
                                 DBUS_NET_IFACE_SETTINGS, QDBusConnection::systemBus());
        
        QDBusReply<QDBusObjectPath> reply = interface.call(QString(DBUS_NET_IFACE_SETTINGS_AddConnection), 
                                                           QVariant::fromValue(connection));
        
        if(reply.isValid()) {
            //get settings 
            QDBusObjectPath path = reply.value();
            setting = new Setting(path.path());
            return true;
        }
        else {
            qDebug()<<reply.error().message()<<"   create error "<<reply.error().name();
            return false;
        }
    }
    
private :
    uint32_t count;
Q_SIGNALS:
    void propertyChanged(QString key, QVariant value);
};

}
#endif // ACCESSPOINT_H
