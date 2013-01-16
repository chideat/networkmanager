#ifndef PPPOE_H
#define PPPOE_H

#include "../types.h"
//#include "../config.h"


#include <linux/if_ether.h>                    /* define struct ethhdr and ETH_DATA_LEN=1500  */
#include <net/ethernet.h>                     /* defined ETHERMTU = ETH_DATA_LEN */

/* length */
#ifndef ETH_DATA_LEN
#define ETH_DATA_LEN 1500
#endif
#ifndef ETHERMTU 
#define ETHERMTU ETH_DATA_LEN
#endif



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

/* pppoe tags */
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
#define STAT_SEND_PADI 0
#define STAT_RECEIVED_PADO 1
#define STAT_SEND_PADR 2
#define STAT_SESSION 3
#define STAT_TERMINATED 4

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

/* define etherent struct  which carry pppoe package */
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


int socket;                                                   /* used to set interface paramters   from rf.kill */

typedef struct PPPoEConnectionStruct {
    int discoveryState;                                 /* which state we are in */
    int discoverySocket;                              /* Raw socket for discovery frames */
    int pppSocket;                                        /* open system file device ppp */
    int pppConnSocket;                              /* used to connect  */
    
}PPPoEConnection;



#endif // PPPOE_H
