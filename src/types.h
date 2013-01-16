#ifndef TYPES_H
#define TYPES_H

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

#define uchar uint8_t

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))


#define null NULL
#endif // TYPES_H
