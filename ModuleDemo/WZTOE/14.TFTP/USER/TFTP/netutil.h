
#ifndef __NETUTIL_H__
#define __NETUTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SYSTEM_LITTLE_ENDIAN

/**
 * Convert a 32bit Address into a Dotted Decimal Format string.
 *
 * @param addr 32bit address.
 * @return Dotted Decimal Format string.
 */
int8_t *inet_ntoa(uint32_t addr);

/**
 * Convert a 32bit Address into a Dotted Decimal Format string.
 * This is differ from inet_ntoa in fixed length.
 *
 * @param addr 32bit address.
 * @return Dotted Decimal Format string.
 */
int8_t *inet_ntoa_pad(uint32_t addr);

/**
 * Converts a string containing an (Ipv4) Internet Protocol decimal dotted address into a 32bit address.
 *
 * @param addr Dotted Decimal Format string.
 * @return 32bit address.
 */
uint32_t inet_addr(uint8_t *addr);

/**
 * Swap the byte order of 16bit(short) wide variable.
 *
 * @param i 16bit value to swap
 * @return Swapped value
 */
uint16_t swaps(uint16_t i);

/**
 * Swap the byte order of 32bit(long) wide variable.
 *
 * @param l 32bit value to convert
 * @return Swapped value
 */
uint32_t swapl(uint32_t l);

/**
 * htons function converts a unsigned short from host to TCP/IP network byte order (which is big-endian).
 *
 * @param hostshort The value to convert.
 * @return The value in TCP/IP network byte order.
 */
uint16_t htons(uint16_t hostshort);

/**
 * htonl function converts a unsigned long from host to TCP/IP network byte order (which is big-endian).
 *
 * @param hostlong The value to convert.
 * @return The value in TCP/IP network byte order.
 */
uint32_t htonl(uint32_t hostlong);

/**
 * ntohs function converts a unsigned short from TCP/IP network byte order
 * to host byte order (which is little-endian on Intel processors).
 *
 * @param netshort The value to convert.
 * @return A 16-bit number in host byte order
 */
uint32_t ntohs(uint16_t netshort);

/**
 * converts a unsigned long from TCP/IP network byte order to host byte order
 * (which is little-endian on Intel processors).
 *
 * @param netlong The value to convert.
 * @return A 16-bit number in host byte order
 */
uint32_t ntohl(uint32_t netlong);

#ifdef __cplusplus
}
#endif

#endif
