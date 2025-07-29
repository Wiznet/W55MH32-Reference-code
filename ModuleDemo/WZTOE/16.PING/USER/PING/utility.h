#ifndef _UTILITY_H
#define _UTILITY_H
#include "stdio.h"
#include "types.h"

/**
 * @brief Convert strings to 16-bit unsigned integers
 *
 * Converts a given string to a 16-bit unsigned integer according to the specified cardinality.
 *
 * @param str :String pointer
 * @param base :radix
 *
 * @return A converted 16-bit unsigned integer
 */
uint16 atoi16(char *str, uint16 base);

/**
 * @brief Convert strings to 32-bit unsigned integers
 *
 * Converts a given string to a 32-bit unsigned integer in the specified base system and returns the result.
 *
 * @param str :String pointer
 * @param base :The base number can be 2~36
 *
 * @return A converted 32-bit unsigned integer
 */
uint32 atoi32(char *str, uint16 base);

/**
 * @brief Convert unsigned 16-bit integers to strings
 *
 * Converts a given unsigned 16-bit integer to string form and stores it in a specified array of characters.
 *
 * @param n :An unsigned 16-bit integer to be converted
 * @param str :An array of characters that stores the result of the transformation
 * @param len :The length of the character array
 */
void itoa(uint16 n, uint8 *str, uint8 len);

/**
 * @brief Converts a string to an integer in the specified base system
 *
 * Converts a given string to an integer in the specified base and stores the result in a specified variable.
 *
 * @param str :String pointer
 * @param base :The base number
 * @param ret :The converted integer stores the pointer
 *
 * @return If the conversion is successful, return 1; Otherwise, 0 is returned
 */
int validatoi(char *str, int base, int *ret);

/**
 * @brief Convert hexadecimal characters to decimal numbers
 *
 * Convert a given hexadecimal character to the corresponding decimal number.
 *
 * @param c :hexadecimal characters
 *
 * @return Converted decimal digits, if the conversion fails, the original characters will be returned
 */
char c2d(u_char c);

/**
 * @brief Swap endianness
 *
 * Swap the upper 8 bits and the lower 8 bits of the input 16-bit unsigned integer and return the result after the swap.
 *
 * @param i :A 16-bit unsigned integer entered
 *
 * @return 16-bit unsigned integer after swapping
 */
uint16 swaps(uint16 i);

/**
 * @brief Byte order for swapping 32-bit unsigned integers
 *
 * Byte order the given 32-bit unsigned integer is swapped and the result after the swap is returned.
 *
 * @param l:32-bit unsigned integers to be swapped for byte order
 *
 * @return 32-bit unsigned integer after swapping
 */
uint32 swapl(uint32 l);

/**
 * @brief Replaces the specified character in the string with another character
 *
 * In a given string, replace all occurrences of the specified character with another character.
 *
 * @param str :string pointer
 * @param oldchar :characters that need to be replaced
 * @param newchar :Replaced characters

 */
void replacetochar(char *str, char oldchar, char newchar);

/**
 * @brief Gets a substring between two strings
 *
 * In a given source string, find two specified substrings and get the substrings between them.
 *
 * @param src :source string pointer
 * @param s1 :first substring pointer
 * @param s2 :second substring pointer
 * @param sub :stores the pointer to the substring
 */
void mid(int8 *src, int8 *s1, int8 *s2, int8 *sub);

/**
 * @brief Converts the IP address represented by the string into a byte array
 *
 * Converts the IP address represented by a given string into a byte array and stores it in the specified array.
 *
 * @param addr :IP address represented by the
 * @param ip :stores an array of bytes of the transformation result
 */
void inet_addr_(unsigned char *addr, unsigned char *ip);

/**
 * @brief Converts network addresses into dotted decimal strings
 *
 * Converts a given network address into a dotted decimal string form.
 *
 * @param addr:network address
 *
 * @return The pointer to the converted dotted decimal string
 */
char *inet_ntoa(unsigned long addr);

/**
 * @brief Translates an IP address into a string with leading zeros
 *
 * Converts a given unsigned long integer IP address to a string format with leading zeros and returns the converted string.
 *
 * @param addr:An unsigned, long, integer representation of the addr IP address
 *
 * @return Converted string pointer
 */
char *inet_ntoa_pad(unsigned long addr);

/**
 * @brief Verify IP address
 *
 * Based on the given IP address string, verify that it is valid and store the result in a specified IP address array.
 *
 * @param src : IP address string
 * @param ip : is used to store an array of verified IP addresses
 *
 * @return Verification result, 1 is returned for success and 0 is returned for failure
 */
char verify_ip_address(char *src, uint8 *ip); /* Verify dotted notation IP address string */

/**
 * @brief Converts 16-bit unsigned integers from host endian-order to network endian-order
 *
 * Converts 16-bit unsigned integers under host endianness to network endians. If the host endianism is little-endian order, the bytes are swapped;
 * Otherwise, the original value will be returned.
 *
 * @param hostshort:16-bit unsigned integer in host endianness
 *
 * @return A 16-bit unsigned integer in the converted network endianness
 */
uint16 htons(unsigned short hostshort);

/**
 * @brief Converts 32-bit host endian-order integers to network endian-order integers
 *
 * Converts a given 32-bit host endian-order integer to a network endian-order integer.
 *
 * @param hostlong: host endian-order integer
 *
 * @return Converted network endian-order integers
 */
uint32 htonl(unsigned long hostlong);

/**
 * @brief Converts 16-bit unsigned integers of network endianness to host endianness
 *
 * Converts the 16-bit unsigned integer of a given network endianness to the corresponding host endian.
 *
 * @param netshort:16-bit unsigned integer of the netshort network endian
 *
 * @return A 16-bit unsigned integer in the converted host endianism
 */
uint32 ntohs(unsigned short netshort);

/**
 * @brief Converts the long integer number of network endenies to host endians
 *
 * Converts the long integer number of network endenians to the corresponding host endianness according to the endianness of the current system.
 *
 * @param netlong:long integer number of the netlong network endeny
 *
 * @return The long integer number of the converted host endians
 */
uint32 ntohl(unsigned long netlong); /* ntohl function converts a u_long from TCP/IP network order to host byte order (which is little-endian on Intel processors). */

/**
 * @brief Calculate the checksum
 *
 * Calculate checksums based on a given data stream.
 *
 * @param src:stream pointer
 * @param len:stream size
 *
 * @return Checksum
 */
uint16 checksum(unsigned char *src, unsigned int len);
#endif
