#ifndef NET_H
#define NET_H

#include <QObject>
#include <QUuid>
#include <QDateTime>

#include "../types.h"
namespace Net {

class Connection : public QObject {
    Q_OBJECT 
    Q_PROPERTY(QString id READ getID WRITE setID)
    Q_PROPERTY(QUuid uuid READ getUUID WRITE setUUID)
    Q_PROPERTY(Arr_Str permissions READ getPermissions WRITE setPermissions)
    Q_PROPERTY(bool autoConnect READ isAutoConnection WRITE enableAutoConnection)
    Q_PROPERTY(uint64_t timestamp READ getTimeStamp)
    Q_PROPERTY(C_Zone zone READ getZone WRITE setZone)
public :
    Connection(QObject * parent = NULL): QObject(parent) {
        //initial
        name = QStringLiteral("connection");
        autoConnect = true;
        timestamp = 0;
        readOnly = false;
    }
    ~Connection(){}
    
    enum C_TYPE {
        /* for hardware-dependent connections */
        c_t_802_3_ethernet = 1,
        c_t_802_11_wireless,
        c_t_bluetooth,
        /* for non-hardware dependent connections */
        c_t_vpn,
        c_t_bridget
    };
    
    /* the true level of the connection. not set means the connection will be placed in 
     * the default zone as defined by the firewall
     */
    enum C_Zone {
        c_z_public = 1,
        c_z_work,
        c_z_home
    };
    
    /* default connection */
    QString name;                                
    /* user-readable connection name */
    QString id;                                        
    /* universally unique connection identifier. should be assigned when the connection
       * is created and never be changed
       */
    QUuid uuid;        
    /* see C_Type define */
    C_TYPE type;   
    /* default []
     * what access a given user has to this connection null or empty: all users are allowed 
     * to access 
     * format:  [type]:[id]:[reserved]. 
     */
    Arr_Str permissions;       
    /* default true. 
     * autoconnect, as if there it's not bool it's int format */
    bool autoConnect;
    /* in seconds. used when the connection was last successfully actived */
    uint64_t timestamp;
    /* default false 
      * if ture, can't be changed by the user or any other mechanism. is normally set for 
      * system connections .
      */
    bool readOnly;
    /* default as defined by the firewall  */
    C_Zone zone;
    /* interface name of the master device or UUID of the master connection */
    QString master;
    /* settig name describing the type of slave this connection is or NULL if this 
     * connection is not a slave.
     */
    QString slaveType;
    /* list of connection uuids that should be actived when the base connection itself is 
     * actived
     */
    Arr_Str secondaries;
    
public Q_SLOTS:
    QString getID() { return id; }
    void setID(QString id) {this->id = id; }
    
    QUuid getUUID() {return uuid; }
    void setUUID(QUuid uuid) { 
//        uuid = QUuid::createUuid().toString().remove('{').remove('}');
        this->uuid = uuid;
    }
    
    Arr_Str getPermissions() { return permissions; }
    void setPermissions(Arr_Str per) { this->permissions = per; }
    
    bool isAutoConnection() { return autoConnect; }
    void enableAutoConnection(bool isAuto) { autoConnect = isAuto; }
    
    uint64_t getTimeStamp() { return timestamp; }
    void updateTimeStamp() {
        timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    }
    
    C_Zone getZone() { return zone; }
    void setZone(C_Zone zone) { this->zone = zone; }
};

class IP : public QObject {
    Q_OBJECT
public :
    IP(QObject *parent = NULL): QObject(parent) {}
    ~IP(){}
    
    enum IP_Method {
        /* auto is specified, appropriate automatic method(DHCP, ppp) is used 
         *
         *  to Ipv6, the appropriate automatic method maybe PPP, router advertisment, etc.*/
        IP_auto = 0, 
        /* link-local is specified, a link-local address 169.254/16 rang will be assigned(ipv4) */
        IP_link_local,
        /* manual is specified, static IP adress is used and at leasd on ip address must be given
         *  in the addresses property.  */
        IP_manual,
        /* shared is specified(provide network access to other cimputers) then the interface 
          *  is assgned an address in the 10.42.x.1/24 and a DHCP and forwarding DNS server 
          *  are started. and the interface is NAT-ed to the current default network connection */
        IP_shared_no_IPv6,
        /* disabled means IP will not be used. */
        IPv4_disabled,
        /* ignored is spefified. IPv6 configuration is not done */
        IPv6_ignored,
        /* to fource use of DHCP only for ipv6, specify dhcp, and is only valid for ethernet-based 
         * hardware */
        IPv6_dhcp
    };
    
    /* default ipv4 */
    QString name; 
    IP_Method method;
    /* as above */
    Arr_Str dns_search;
    /* default FALSE
     * when method == auto && this == TRUE, auto config routes is ignored and only 
     * routes specified is used. */
    bool ignore_auto_routes;
    /* default FALSE
     * when method == auto && this == TRUE, auto config nameservers and search domains
     * are ignored and nameservers and search domains specified is used. */
    bool ignore_auto_dns;
    /* speficied hostname 
     * IPv4 if dhcp-send-hostname ==TRUE, the specified name will be send to DHCP server.*/
    QString dhcp_hostname;
    /* default FALSE 
     * if true, the connection will never be the default ipv4 connection.
     * never be assigned the de default route */
    bool never_default;
    /* default TRUE
     * if TRUE, allow overall network config to process even if config times out.
     * IPv6 only net, TRUE allow overall network config succeceed if ipv4 config fails but
     * ipv6 config successfully */
    bool may_fail;
    
    
    IP_Method getMethod() { return method; }
    void setMethod(IP_Method method) { this->method = method; }
    
    Arr_Str getDNSearch() { return dns_search; }
    void setDNSearch(Arr_Str dns_search) { this->dns_search = dns_search; }
    
    bool isIgnoreAutoRoute() { return ignore_auto_routes; }
    void enableIgnoreAutoRoute(bool r) { ignore_auto_routes = r; }
    
    bool isIgnoreAutoDNS() { return ignore_auto_dns; }
    void enableIgnoreAutoDNS(bool d) { ignore_auto_dns = d; }
    
    QString getHostName() { return dhcp_hostname; }
    void setHostName(QString name) { dhcp_hostname = name; }
    
    bool isNeverDefault() { return never_default; }
    void enableNeverDefault(bool d) { never_default = d; }
    
    bool isMayFail() { return may_fail; }
    void enableMayFail(bool mf) { may_fail = mf; }
};

class IPv4: public IP {
    Q_OBJECT
public :
    IPv4(IP *parent = NULL) : IP(parent) {
        name = "ipv4";
        ignore_auto_routes = false;
        ignore_auto_dns = false;
        dhcp_send_hostname = true;
        never_default = false;
        may_fail = true;
    }
    
    ~IPv4() {}
    
    /* dns servers(network byte order). 
     * auto method, these DNS servers are appended to those returned by automatic 
     * configuration. 
     * CAN use with auto
     * CAN NOT use with shared, link-local, disabled */
    Arr_UInt32 dns;
    /* ipv4-addr, prefix(1-32), ipv4-gateway
     * gateway may be left as 0 if no gateway exists for that subnet
     * CAN NOT used with shared, link-local, disabled */
    Arr_Arr_UInt32 addresses;
    /* dest-ipv4, prefix, next-hop, route metric
     * CAN NOT used with shared, link-local, disabled */
    Arr_Arr_UInt32 routes;
    /* a string send to the DHCP server to identify the local machine  */
    QString dhcp_client_id;
    /* default TRUE
     * when acquiring a lease, a hostname is send to the DHCP.
     * Some DHCP server use this to update DNS databases.
     * if is true, send computer's persistent hostname to the SERVER */
    bool dhcp_send_hostname;
    
    Arr_UInt32 getDNS() { return dns; }
    void setDNS(Arr_UInt32 dns) { this->dns = dns;}
    
    Arr_Arr_UInt32 getAddresses() { return addresses; }
    void setAddresses(Arr_Arr_UInt32 addr) { addresses = addr ;} 
    
    Arr_Arr_UInt32 getRoutes() { return routes; }
    void setRoutes(Arr_Arr_UInt32 r) { this->routes = r; }
    
    QString getClientID() { return dhcp_client_id; }
    void setClientID(QString id) { dhcp_client_id = id; }
    
    bool isSendHostName() { return dhcp_send_hostname; }
    void enableSendHostName(bool s) { dhcp_send_hostname = s; }
};


class IPv6 : public IP {
    Q_OBJECT
public :
    IPv6(IP *parent = NULL): IP(parent) {
        name = "ipv6";
        ignore_auto_routes = false;
        ignore_auto_dns = false;
        never_default = false;
        may_fail = true;
        ip6_privacy = -1;
    }
    
    /* CAN NOT used in shared, link-local 
     * all other method used DNS servers as the only DNS server */  
    Arr_Byte dns;
    /* ipv6 addr, 32-bit integer, ipv6-gateway 
     * if no gateway if given, the third element should be 0.
     * CAN NOT used with shared, link-local */
    Arr_Byte_UInt32_Byte addresses;
    /* dest, prefix, next-hop, route metric
     * CAN NOT used with shared, link-local */
    Arr_Byte_UInt32_Byte_UInt32 routes;
    /*  Default -1
     * config ipv6 Privacy Extensions for SLAAC.
     * if enable, it makes the kernel generate a temporary IPv6 addr in addition to the public
     * one generated from MAC addr via modified EUI-64.
     * 0: disabled
     * 1: enabled (prefer public addr)
     *  2 enabled (prefer temporary address) */
    int32_t ip6_privacy;
    
    
    Arr_Byte getDNS() { return dns; }
    void setDNS(Arr_Byte dns) { this->dns = dns; }
    
    Arr_Byte_UInt32_Byte getAddresses() { return addresses; }
    void setAddresses(Arr_Byte_UInt32_Byte addr) { addresses = addr; }
    
    Arr_Byte_UInt32_Byte_UInt32 getRoutes() { return routes; }
    void setRoutes(Arr_Byte_UInt32_Byte_UInt32 r) { routes = r; }
    
    int32_t getPrivacy() { return ip6_privacy; }
    void setPrivacy(int32_t p) { ip6_privacy = p; }
};
}

#endif // NET_H
