#ifndef PPPOE_H
#define PPPOE_H

#include "../types.h"
//#include "../config.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_ether.h>                    /* define struct ethhdr and ETH_DATA_LEN=1500  */
#include <net/ethernet.h>                     /* defined ETHERMTU = ETH_DATA_LEN */

/* length */
#ifndef ETH_DATA_LEN
#define ETH_DATA_LEN 1500
#endif
#ifndef ETHERMTU
#define ETHERMTU ETH_DATA_LEN
#endif

#define BPF_BUFFER_IS_EMPTY 1
#define BPF_BUFFER_HAS_DATA 0


/* Ethernet frame types according to RFC 2516 */
#define ETH_PPPoE_DISCOVERY 0x8863
#define ETH_PPPoE_SESSION 0x8864

/* PPPoE codes */
#define _PADI_ 0x09
#define _PADO_ 0x07
#define _PADR_ 0x19
#define _PADS_ 0x65
#define _PADT_ 0xA7
/* extensions from draft-carrel-inf0-pppoe-ext-00 */
#define _PADM_ 0xD3
#define _PADN_ 0xD4
/* session code */
#define _SESS_ 0x00

/**
 * pppoe tags 
 * if the tag type is not unknown, then ignore it
 */
#define _TAG_END_OF_LIST  0x0000
#define _TAG_SERVICE_NAME 0x0101
#define _TAG_AC_NAME 0x0102
#define _TAG_HOST_UNIQ 0x0103
#define _TAG_AC_COOKIE 0x0104
#define _TAG_VENDOR_SPECIFIC 0x0105
#define _TAG_RELAY_SESSION_ID 0x0110
#define _TAG_PPP_MAX_PAYLOAD 0x0120
#define _TAG_SERVICE_NAME_ERROR 0x0201
#define _TAG_AC_SYSTEM_ERROR 0x0202
#define _TAG_GENERIC_ERROR 0x0203
/* extension from draft-carrel-info-pppoe-ext00 */
#define _TAG_HURL 0x111
#define _TAG_MOTM 0x112
#define _TAG_IP_ROUTE_ADD 0x121

/* discovery phase state */
#define STATE_SEND_PADI 0
#define STATE_RECEIVED_PADO 1
#define STATE_SEND_PADR 2
#define STATE_SESSION 3
#define STATE_TERMINATED 4

/*how many padi/pads attempts */
#define MAX_PADI_ATTEMPTS 3

/* initial timeout for PADO/PADS */
#define PADI_TIMEOUT 5

/* states for scanning ppp frames */
/* ??? */
#define FRAME_ESC 0x7D
#define FRAME_FLAG 0x7E
#define FRAME_ADDR 0xFF
#define FRAME_CTRL 0x03
#define FRAME_ENC 0x20


#define IPv4LEN 4
#define SMALLBUF 256

/* allow for 1500 bytes PPPoE data which makes the ethernet packet bigger by 8 bytes */
/* why add 8 */
#define ETH_JUMBO_LEN (ETH_DATA_LEN + 8)

/* define etherent struct  which carry pppoe package */
/* missing checksum */
typedef struct PPPoEPackageStruct {
    struct ethhdr ethHdr;                          /* ethernet header */
#ifdef  PACK_BITFIELDS_REVERSED
    unsigned int type: 4;                           /* PPPoE type (must be 1) */
    unsigned int version: 4;                       /* PPPoE Vserion(must be 1) */
#else 
    unsigned int version: 4;                       /* PPPoE Vserion(must be 1) */
    unsigned int type: 4;                            /* PPPoE type (must be 1) */
#endif
    unsigned int code: 8;                           /* PPPoE code */
    unsigned int session: 16;                    /* PPPoE session */
    unsigned int length: 16;                      /* payload length */
    unsigned char payload[ETH_JUMBO_LEN];                    /* a bit of room to spare */
}PPPoEPacket;

/* header size of a pppoe packet */
#define PPPoE_OVERHEAD 6                   /* include  version, type, code, sessioni_id and length */
#define HDR_SIZE(sizeof(struct ethhdr) + PPPoE_OVERHEAD)       /* ether head( and pppoe head(6)*/
#define MAX_PPPoE_PAYLOAD (ETH_JUMBO_LEN - PPPoE_OVERHEAD)
#define PPP_OVERHEAD 2
#define MAX_PPPoE_MTU (MAX_PPPoE_PAYLOAD - PPP_OVERHEAD)
#define TOTAL_OVERHEAD (PPPoE_OVERHEAD + PPP_OVERHEAD)     /* the whole payload head size */

/* normal PPPoE MTU without jumbo frames */
/* ??? */
#define ETH_PPPoE_MTU (ETH_DATA_LEN - TOTAL_OVERHEAD)

/* PPPoE tag */
typedef struct PPPoETagStruct {
    uint32_t type: 16;                       /* tag type */
    uint32_t length: 16;                    /* length if tag payload */
    uint32_t payload[ETH_JUMBO_LEN];  /* a lot of room to spare */
}PPPoETag;

/* header size of a pppoe tag */
#define TAG_HDR_SIZE 4

/* function passed to parsePacket */
typedef void ParseFunc(uint16_t type, uint16_t len, uint8_t *data, void *extra);

#define PPPINITFCS16 0xFFFF                 /* initial FCS value */

int socket;                                                   /* used to set interface paramters   from rf.kill */

/** struct used to keep track of the state of a connection  --collect everything in one spot*/
typedef struct PPPoEConnectionStruct {
    int discoveryState;                                 /* which state we are in */
    int discoverySocket;                              /* Raw socket for discovery frames */
    int sessionSocket;                                 /* raw socket for session frames */
    /* sockets add by myself */
    int pppSocket;                                        /* open system file device ppp */
    int pppConnSocket;                               /* used to connect  */
    uchar hostMAC[ETH_ALEN];                  /* host's MAC address  ETH_ALEN  == 6 */
    uchar peerMAC[ETH_ALEN];                 /* peer's MAC address */
    /** plugin */ 
    uchar req_peer_mac[ETH_ALEN];         /* required peer mac address */
    uchar req_peer;                                     /*  require mac addr to match req_peer_mac */
    uint16_t session;                                    /* session_id*/
    char *ifName;                                         /* interface name*/
    /* tag flag defined  */
    /** tag_value is an UTF-8 string that is not null terminated. if tag_length is zero, it indicate that any service is acceptable. 
     *  example : used to indicate an ISP name or a class or quality of service
     */
    char *serviceName;                               /* desired service name, if any. [RFC 2516-10]*/
    char *acName;                                       /* desired ac name, if any. [RFC 2516-10] */
    int synchronous;                                    /* use synchronous ppp */
    /** used by a Host to uniquely associate an access Concentrator response (PADO or PADS)  to a particular host request(PADI, PADR). 
      *   tag_value is binary data of any value and length
      *   if AC get the TAG, it must include the tag unmodified in the asocidate PADO or PADS response
       */
    int useHostUniq;                                    /* use host-uniq tag [RFC 2516-10]*/
    int printACNames;                                 /* just print AC names */
    int skipDiscovery;                                   /* skip discovery?  */
    int noDiscoverySocket;                          /*  don't even open discovery socket */
    int killSession;                                         /* kill session and exit */
    FILE *debugFile;                                     /* debug file for dumping packets , maybe not need it any more */
    int numPADOS;                                       /* number of PADO packets received */
    PPPoETag cookie;                                    /* have to send this is got it */
    PPPoETag relayID;                                   /* ditto */
    int PADSHadError;                                   /* if PADS had an error tag */
    int discoveryTimeout;                              /* timneout for discovery packets */
    /* kernel pppoe */
    int seenMaxPayload; 
    int mtu;
    int mru;
}PPPoEConnection;

/* struct used to determine acceptable PADO or PADS packet */
struct PacketCriteria {
    PPPoEConnection *conn;
    int acNameOK;
    int serviceNameOK;
    int seenACName;
    int seenServiceName;
    int gotError;
};

/* function prototypes */
int openInterface(char const **ifname, uint16_t type, uchar *hwaddr, uint16_t *mtu);

void sendPacket(PPPoEConnection *conn, int socket, PPPoEPacket *pkt, int size);
void receivePacket(int socket, PPPoEPacket *pkt, int *size);

void sendPADT(PPPoEConnection *conn, char const *msg);

int parsePacket(PPPoEPacket *packet, ParseFunc *func, void *extra);

void fatalSys(char const *str);
void printErr(char const *str);
void pktLogErrs(char const *pkt, uint16_t type, uint16_t len, uchar *data, void *extra);

#define CHECK_ROOM(cursor, start, len) \
    do {\
        if(((cursor) - (start)) + (len) > MAX_PPPoE_PAYLOAD ) {\
            syslog(LOG_ERR, "Would create too-long packet");\
            return ;\
        }\
    }while(0)
    
/* true of mac addr is broadcast or multicast*/
#define NOT_UNICAST(e) ((e) & 0x01 != 0)
#define BROADCAST(e) ((e[0] & e[1] &e[2] & e[3] & e[4] & e[5]) == 0xFF)
#define NOT_BROADCAST(e) ((e[0] & e[1] & e[2] & e[3] & e[4] & e[5]) != 0xFF)


#endif // PPPOE_H