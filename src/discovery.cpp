/**
 * discovery.c
 * define the necessory function in PPPoE discovery stage
 * as the HostUnique tag and so on
 */

#include "ppp/pppoe.h"


#include <syslog.h>

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/time.h>
#include <time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <pppd/pppd.h>
#include <pppd/fsm.h>
#include <pppd/lcp.h>

/**
 * @brief parseForHostUniq
 * @param type
 * @param len
 * @param data
 * @param extra
 * if a hostUnique tag is found which matches our PID, set *extra to 1
 */
static void parseForHostUniq(uint16_t type, uint16_t len, uchar *data, void *extra) {
    int *val = (int *) extra;
    if(type == _TAG_HOST_UNIQ && len == sizeof(pid_t)) {
        pid_t tmp;
        memcpy(&tmp, data, len);
        if(tmp == getpid()) {
            *val = 1;
        }
    }
}

/**
 * @brief packetIsForMe
 * @param conn
 * @param packet
 * @return
 * if using the Host-Unique tag, verifies that packet contains out unique identifiler
 */
static int packetIsForMe(PPPoEConnection *conn, PPPoEPacket *packet) {
    int forMe = 0;
    /* if packet is not for host MAC addr*/
    if(memcmp(packet->ethHdr.h_dest, conn->hostMAC, ETH_ALEN)) return 0;
    /* if not using the Host0Unique tag, then accetp the packet */
    if(!conn->useHostUniq) return 1;
    
    parsePacket(packet, parseForHostUniq, &forMe);
    
    return forMe;
}

/**
 * @brief parsePADOTags
 * @param type
 * @param len
 * @param data
 * @param extra  extra user data. should point to a packetcriteria structure which gets 
 * filled in according to selected AC name and service name.
 * Picks interesting tags of a PADO packet 
 */
static void parsePADOTags(uint16_t type, uint16_t len, uchar *data, void *extra) {
    struct PacketCriteria *pc = (struct PacketCriteria *)extra;
    PPPoEConnection *conn = pc->conn;
    int i;
    uint16_t mru;
    
    switch(type) {
    case _TAG_AC_NAME: 
        pc->seenACName = 1;
        /* not necessory */
        if(conn->printACNames) {
            printf("Access-Concentrator: %*.s\n", (int)len, data);
        }
        if(conn->acName && len == strlen(conn->acName) && !strncmp((char *)data, conn->acName, len)) {
            pc->acNameOK = 1;
        }
        break;
    case _TAG_SERVICE_NAME:
        pc->seenServiceName = 1;
        if (conn->printACNames && len > 0) {
            printf("       Service-Name: %.*s\n", (int) len, data);
        }
        if(conn->serviceName && len == strlen(conn->serviceName) && !strncmp((char *)data, conn->serviceName, len)) {
            pc->serviceNameOK = 1;
        }
        break;
    case _TAG_AC_COOKIE:
        if (conn->printACNames) {
            printf("Got a cookie:");
            /* Print first 20 bytes of cookie */
            for (i=0; i<len && i < 20; i++) {
            printf(" %02x", (unsigned) data[i]);
            }
            if (i < len) printf("...");
            printf("\n");
        }
        conn->cookie.type = htons(type);
        conn->cookie.length = htons(len);
        memcpy(conn->cookie.payload, data, len);
        break;
    case _TAG_RELAY_SESSION_ID:
        if (conn->printACNames) {
            printf("Got a Relay-ID:");
            /* Print first 20 bytes of relay ID */
            for (i=0; i<len && i < 20; i++) {
            printf(" %02x", (unsigned) data[i]);
            }
            if (i < len) printf("...");
            printf("\n");
        }
        conn->relayID.type = htons(type);
        conn->relayID.length = htons(len);
        memcpy(conn->relayID.payload, data, len);
        break;
    case _TAG_SERVICE_NAME_ERROR:
        if (conn->printACNames) {
            printf("Got a Service-Name-Error tag: %.*s\n", (int) len, data);
        } 
        else {
            pktLogErrs("PADO", type, len, data, extra);
            pc->gotError = 1;
            if (!persist) {
            exit(1);
            }
        }
        break;
    case _TAG_AC_SYSTEM_ERROR:
        if (conn->printACNames) {
                printf("Got a System-Error tag: %.*s\n", (int) len, data);
        } 
        else {
                pktLogErrs("PADO", type, len, data, extra);
                pc->gotError = 1;
                if (!persist) {
                exit(1);
                }
            }
            break;
    case _TAG_GENERIC_ERROR:
        if (conn->printACNames) {
            printf("Got a Generic-Error tag: %.*s\n", (int) len, data);
        } 
        else {
            pktLogErrs("PADO", type, len, data, extra);
            pc->gotError = 1;
            if (!persist) {
            exit(1);
            }
        }
        break;
    case _TAG_PPP_MAX_PAYLOAD:
        if(len == sizeof(mru)) {
            memcpy(&mru, data, sizeof(mru));
            mru = ntohs(mru);
            if(mru >= ETH_PPPoE_MTU) {
                if(lcp_allowoptions[0].mru > mru) lcp_allowoptions[0].mru = mru;
                conn->seenMaxPayload = 1;
            }
        }
        break;
    }
}

/**
 * @brief parsePADSTags
 * @param type
 * @param len
 * @param data
 * @param extra
 * Picks interneting tags out of a PADS packet
 */
static void parsePADSTags(uint16_t type, uint16_t len, uchar *data, void *extra) {
    uint16_t mru;
    PPPoEConnection *conn = (PPPoEConnection *)extra;
    switch(type) {
    case _TAG_SERVICE_NAME:
        syslog(LOG_DEBUG, "PADS: Service-Name: '%.*s'", (int)len, data);
        break;
    case _TAG_GENERIC_ERROR:
    case _TAG_AC_SYSTEM_ERROR:
    case _TAG_SERVICE_NAME_ERROR:
        pktLogErrs("PADS", type, len,data, extra);
        conn->PADSHadError = 1;
        break;
    case _TAG_RELAY_SESSION_ID:
        conn->relayID.type = htons(type);
        conn->relayID.length = htons(len);
        memcpy(conn->relayID.payload, data, len);
        break;
    case _TAG_PPP_MAX_PAYLOAD:
        if(len == sizeof(mru)) {
            memcpy(&mru, data, sizeof(mru));
            mru = ntohs(mru);
            if(mru >= ETH_PPPoE_MTU) {
                if(lcp_allowoptions[0].mru > mru) lcp_allowoptions[0].mru = mru;
                conn->seenMaxPayload = 1;
            }
        }
        break;
    }
}

/**
 * @brief sendPADI
 * @param conn
 */
static void sendPADI(PPPoEConnection *conn) {
    PPPoEPacket packet;
    uchar *cursor = packet.payload;
    PPPoETag *svc = (PPPoETag *)(&packet.payload);
    uint16_t nameLen = 0;
    uint16_t plen;
    int omit_service_name = 0;
    
    if(conn->serviceName) {
        nameLen = (uint16_t) strlen(conn->serviceName);
        if(!strcmp(conn->serviceName, "NO-SERVICE-NAME-RFC-COMPLIANT")) {
            omit_service_name = 1;
        }
    }
    /* set destination to ethernet broadcast address */
    memset(packet.ethHdr.h_dest, 0xFF, ETH_ALEN);
    memcpy(packet.ethHdr.h_source, conn->hostMAC, ETH_ALEN);
    
    packet.ethHdr.h_proto = htons(ETH_PPPoE_DISCOVERY);
    packet.version = 1;
    packet.type = 1;
    packet.code = _PADI_;
    packet.session = 0;
    
    if(!omit_service_name) {
        plen = TAG_HDR_SIZE + nameLen;
        CHECK_ROOM(cursor, packet.payload, plen);
        
        svc->type = _TAG_SERVICE_NAME;
        svc->length = htons(nameLen);
        
        if(conn->serviceName) {
            memcpy(svc->payload, conn->serviceName, strlen(conn->serviceName));
        }
        cursor += nameLen + TAG_HDR_SIZE;
    }
    else {
        plen = 0;
    }
    
    /*if use Host-Uniq, copy it over*/
    if(conn->useHostUniq) {
        PPPoETag hostUniq;
        pid_t pid = getpid();
        hostUniq.type = htons(_TAG_HOST_UNIQ);
        hostUniq.length = htons(sizeof(pid));
        memcpy(hostUniq.payload, &pid, sizeof(pid));
        CHECK_ROOM(cursor, packet.payload, sizeof(pid) + TAG_HDR_SIZE);
        memcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
        cursor += sizeof(pid) + TAG_HDR_SIZE;
        plen += sizeof(pid) + TAG_HDR_SIZE;
    }
        /* add our maximun MTU/MRU*/
    if(MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru) > ETH_PPPoE_MTU) {
        PPPoETag maxPayload;
        uint16_t mru = htons(MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru));
        maxPayload.type = htons(_TAG_PPP_MAX_PAYLOAD);
        maxPayload.length = htons(sizeof(mru));
        memcpy(maxPayload.payload, &mru, sizeof(mru) + TAG_HDR_SIZE);
        CHECK_ROOM(cursor, packet.payload, sizeof(mru) + TAG_HDR_SIZE);
        memcpy(cursor, &maxPayload, sizeof(mru) + TAG_HDR_SIZE);
        cursor += sizeof(mru) + TAG_HDR_SIZE;
        plen += sizeof(mru) + TAG_HDR_SIZE;
    }
        
    packet.length = htons(plen);
    sendPacket(conn, conn->discoverySocket, &packet, (int)(plen + HDR_SIZE));
    /*ignore debugging*/
}

/**
 * @brief waitForPADO
 * @param conn
 * @param timeout
 * wait for a PADO packet and copies useful information
 */
static void waitForPADO(PPPoEConnection *conn, int timeout) {
    fd_set readable;
    int r;
    struct timeval tv;
    struct timeval expire_at;
    struct timeval now;
    
    PPPoEPacket packet;
    int len;
    
    struct PacketCriteria pc;
    pc.conn = conn;
    /* kernel pppoe */
    conn->seenMaxPayload = 0;
    
    if(gettimeofday(&expire_at, null) < 0) {
        fatalSys("gettimeofday(waitForPADO");
    }
    expire_at.tv_sec += timeout;
    do {
        if(BPF_BUFFER_IS_EMPTY) {
            if(gettimeofday(&now, null) < 0) {
                fatalSys("gettimeofday (waitForPADO)");
            }
            tv.tv_usec = expire_at.tv_usec - now.tv_usec;
            tv.tv_sec = expire_at.tv_sec - now.tv_sec;
            if(tv.tv_usec < 0) {
                tv.tv_usec += 1000000;
                if(tv.tv_sec) tv.tv_sec --;
                else return ;
            }

            if(tv.tv_sec <= 0 && tv.tv_usec <= 0)
                return ;
            
            FD_ZERO(&readable);
            FD_SET(conn->discoverySocket, &readable);
            
            while(1) {
                r = select(conn->discoverySocket + 1, &readable, null, null, &tv);
                if(r >= 0 || error != EINTR) break;
            }
            if(r < 0) 
                fatalSys("select (waitForPADO)");
            if(r == 0) 
                return ;
        }
        
        /* get the packet  */
        receivePacket(conn->discoverySocket, &packet, &len);
        
        /*check len*/
        if(ntohs(packet.length) + HDR_SIZE > len) {
            syslog(LOG_ERR, "Bogus PPPoE length field (%u)", (unsigned int) ntohs(packet.length));
            continue;
        }
        /* not the correct packet */
        if(!packetIsForMe(conn, &packet)) continue;
        
        if(packet.code == _PADO_) {
            if(NOT_UNICAST(packet.ethHdr.h_source)) {
                printErr("Ignoring PADO packet from non-unicast MAC address");
                continue;
            }
            /* kernel pppoe*/
            if(conn->req_peer && memcmp(packet.ethHdr.h_source, conn->req_peer_mac, ETH_ALEN) != 0) {
                warn("Ignoring PADO packet from wrong MAC address");
                continue;
            }
            pc.gotError  = 0;
            pc.seenACName = 0;
            pc.seenServiceName = 0;
            pc.acNameOK = (conn->acName ) ? 0 : 1;
            pc.serviceNameOK = (conn->serviceName) ? 0 : 1;
            parsePacket(&packet, parsePADOTags, &pc);
            if(pc.gotError) {
                printErr("Error in PADO packet");
                continue;
            }
            
            if (!pc.seenACName) {
                printErr("Ignoring PADO packet with no AC-Name tag");
                continue;
            }
            if (!pc.seenServiceName) {
                printErr("Ignoring PADO packet with no Service-Name tag");
                continue;
            }
            conn->numPADOS ++;
            if(pc.acNameOK && pc.serviceNameOK) {
                memcpy(conn->peerMAC, packet.ethHdr.h_source, ETH_ALEN);
                if (conn->printACNames) {
                    printf("AC-Ethernet-Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
                       (unsigned) conn->peerEth[0],
                       (unsigned) conn->peerEth[1],
                       (unsigned) conn->peerEth[2],
                       (unsigned) conn->peerEth[3],
                       (unsigned) conn->peerEth[4],
                       (unsigned) conn->peerEth[5]);
                    printf("--------------------------------------------------\n");
                    continue;
                }
                conn->discoveryState = STATE_RECEIVED_PADO;
                break;
            }
        }
    }while(conn->discoveryState != STATE_RECEIVED_PADO);
}

static void sendPADR(PPPoEConnection *conn) {
    PPPoEPacket packet;
    PPPoETag *svc = (PPPoETag *)packet.payload;
    uchar *cursor = packet.payload;
    
    uint16_t nameLen = 0;
    uint16_t plen;
    
    if(conn->serviceName) {
        nameLen = (uint16_t) strlen(conn->serviceName);
    }
    plen = TAG_HDR_SIZE + nameLen;
    CHECK_ROOM(cursor, packet.payload, plen);
    
    memcpy(packet.ethHdr.h_dest, conn->peerMAC, ETH_ALEN);
    memcpy(packet.ethHdr.h_source, conn->hostMAC, ETH_ALEN);
    
    packet.ethHdr.h_proto = htons(ETH_PPPoE_DISCOVERY);
    packet.version = 1;
    packet.type = 1;
    packet.code = _PADR_;
    packet.session = 0;
    
    svc->type = _TAG_SERVICE_NAME;
    svc->length = htons(nameLen);
    if(conn->serviceName) {
        memcpy(svc->payload, conn->serviceName, nameLen);
    }
    cursor += nameLen + TAG_HDR_SIZE;
    
    /* If we're using Host-Uniq, copy it over */
    if (conn->useHostUniq) {
        PPPoETag hostUniq;
        pid_t pid = getpid();
        hostUniq.type = htons(_TAG_HOST_UNIQ);
        hostUniq.length = htons(sizeof(pid));
        memcpy(hostUniq.payload, &pid, sizeof(pid));
        CHECK_ROOM(cursor, packet.payload, sizeof(pid)+TAG_HDR_SIZE);
        memcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
        cursor += sizeof(pid) + TAG_HDR_SIZE;
        plen += sizeof(pid) + TAG_HDR_SIZE;
    }
    
    /* Copy cookie and relay-ID if needed */
    if (conn->cookie.type) {
        CHECK_ROOM(cursor, packet.payload, ntohs(conn->cookie.length) + TAG_HDR_SIZE);
        memcpy(cursor, &conn->cookie, ntohs(conn->cookie.length) + TAG_HDR_SIZE);
        cursor += ntohs(conn->cookie.length) + TAG_HDR_SIZE;
        plen += ntohs(conn->cookie.length) + TAG_HDR_SIZE;
    }
    
    if (conn->relayId.type) {
        CHECK_ROOM(cursor, packet.payload, ntohs(conn->relayId.length) + TAG_HDR_SIZE);
        memcpy(cursor, &conn->relayId, ntohs(conn->relayId.length) + TAG_HDR_SIZE);
        cursor += ntohs(conn->relayId.length) + TAG_HDR_SIZE;
        plen += ntohs(conn->relayId.length) + TAG_HDR_SIZE;
    }
    /* kernel pppoe */
    /* Add our maximum MTU/MRU */
    if (MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru) > ETH_PPPoE_MTU) {
        PPPoETag maxPayload;
        UINT16_t mru = htons(MIN(lcp_allowoptions[0].mru, lcp_wantoptions[0].mru));
        maxPayload.type = htons(_TAG_PPP_MAX_PAYLOAD);
        maxPayload.length = htons(sizeof(mru));
        memcpy(maxPayload.payload, &mru, sizeof(mru));
        CHECK_ROOM(cursor, packet.payload, sizeof(mru) + TAG_HDR_SIZE);
        memcpy(cursor, &maxPayload, sizeof(mru) + TAG_HDR_SIZE);
        cursor += sizeof(mru) + TAG_HDR_SIZE;
        plen += sizeof(mru) + TAG_HDR_SIZE;
    }
    packet.length = htons(plen);
    sendPacket(conn, conn->discoverySocket, &packet, (int) (plen + HDR_SIZE));
}

















