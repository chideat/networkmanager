#ifndef PPPOE_H
#define PPPOE_H

//#include "../config.h"

int socket;                                               /* used to set interface paramters   from rf.kill */

typedef struct PPPoEConnectionStruct {
    int discoveryState;                                 /* which state we are in */
    int discoverySocket;                              /* Raw socket for discovery frames */
    int pppSocket;                                        /* open system file device ppp */
    int pppConnSocket;                              /* used to connect  */
    
}PPPoEConnection;



#endif // PPPOE_H
