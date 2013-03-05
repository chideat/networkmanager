#ifndef TYPES_H
#define TYPES_H

#include <QList>
#include <QMap>
#include <QMetaType>
#include <QVariant>
/**
 * define the common used types
 */

#ifndef uint16_t
#define uint16_t unsigned short
#endif
#ifndef uint8_t
#define uint8_t unsigned char
#endif
#ifndef uint32_t
#define uint32_t unsigned int
#endif
#ifndef uint64_t
#define uint64_t unsigned long long
#endif

#define uchar uint8_t

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef QList<QString> Arr_Str;
Q_DECLARE_METATYPE(Arr_Str)

typedef QList<uint32_t> Arr_UInt32;
Q_DECLARE_METATYPE(Arr_UInt32)

typedef QList<QList<uint32_t> > Arr_Arr_UInt32;
Q_DECLARE_METATYPE(Arr_Arr_UInt32)

typedef struct {
    QByteArray addr; //ipv6 addr, with network byte order
    uint32_t prefox;   //ipv6 prefix
    QByteArray gateway;   //ipv6 gateway
} IPv6_Addr;

typedef struct  {
    QByteArray dest;   //dest ipv6 addr network byte order
    uint32_t prefix;     // dest prefix
    QByteArray nextHop;   // network byte order
    uint32_t routeMetric;
}IPv6_Route;

Q_DECLARE_METATYPE(IPv6_Addr)
Q_DECLARE_METATYPE(IPv6_Route)

typedef QList<QByteArray> Arr_Byte;
typedef QList<IPv6_Addr>  Arr_Byte_UInt32_Byte;
typedef QList<IPv6_Route>  Arr_Byte_UInt32_Byte_UInt32;
Q_DECLARE_METATYPE(Arr_Byte)
Q_DECLARE_METATYPE(Arr_Byte_UInt32_Byte)
Q_DECLARE_METATYPE(Arr_Byte_UInt32_Byte_UInt32)


typedef QMap<QString, QMap<QString, QVariant> > Json;
Q_DECLARE_METATYPE(Json)


#endif // TYPES_H
