/**
******************************************************************************
* @file    			utility.c
* @version 			V1.2
* @date    			2017-11-01
*
******************************************************************************
*/

#include "wizchip_conf.h"
#include "utility.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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
uint16 atoi16(char *str, uint16 base)
{
    unsigned int num = 0;
    while (*str != 0)
        num = num * base + c2d(*str++);
    return num;
}

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
uint32 atoi32(char *str, uint16 base)
{
    uint32 num = 0;
    while (*str != 0)
        num = num * base + c2d(*str++);
    return num;
}

/**
 * @brief Convert unsigned 16-bit integers to strings
 *
 * Converts a given unsigned 16-bit integer to string form and stores it in a specified array of characters.
 *
 * @param n :An unsigned 16-bit integer to be converted
 * @param str :An array of characters that stores the result of the transformation
 * @param len :The length of the character array
 */
void itoa(uint16 n, uint8 str[5], uint8 len)
{
    uint8 i = len - 1;

    memset(str, 0x20, len);
    do
    {
        str[i--] = n % 10 + '0';

    } while ((n /= 10) > 0);

    return;
}

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
int validatoi(char *str, int base, int *ret)
{
    int   c;
    char *tstr = str;
    if (str == 0 || *str == '\0')
        return 0;
    while (*tstr != '\0')
    {
        c = c2d(*tstr);
        if (c >= 0 && c < base)
            tstr++;
        else
            return 0;
    }

    *ret = atoi16(str, base);
    return 1;
}

/**
 * @brief Replaces the specified character in the string with another character
 *
 * In a given string, replace all occurrences of the specified character with another character.
 *
 * @param str :string pointer
 * @param oldchar :characters that need to be replaced
 * @param newchar :Replaced characters

 */
void replacetochar(char *str, char oldchar, char newchar)
{
    int x;
    for (x = 0; str[x]; x++)
        if (str[x] == oldchar)
            str[x] = newchar;
}

/**
 * @brief Convert hexadecimal characters to decimal numbers
 *
 * Convert a given hexadecimal character to the corresponding decimal number.
 *
 * @param c :hexadecimal characters
 *
 * @return Converted decimal digits, if the conversion fails, the original characters will be returned
 */
char c2d(uint8 c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';

    return (char)c;
}

/**
 * @brief Swap endianness
 *
 * Swap the upper 8 bits and the lower 8 bits of the input 16-bit unsigned integer and return the result after the swap.
 *
 * @param i :A 16-bit unsigned integer entered
 *
 * @return 16-bit unsigned integer after swapping
 */
uint16 swaps(uint16 i)
{
    uint16 ret  = 0;
    ret         = (i & 0xFF) << 8;
    ret        |= ((i >> 8) & 0xFF);
    return ret;
}

/**
 * @brief Byte order for swapping 32-bit unsigned integers
 *
 * Byte order the given 32-bit unsigned integer is swapped and the result after the swap is returned.
 *
 * @param l:32-bit unsigned integers to be swapped for byte order
 *
 * @return 32-bit unsigned integer after swapping
 */
uint32 swapl(uint32 l)
{
    uint32 ret  = 0;
    ret         = (l & 0xFF) << 24;
    ret        |= ((l >> 8) & 0xFF) << 16;
    ret        |= ((l >> 16) & 0xFF) << 8;
    ret        |= ((l >> 24) & 0xFF);
    return ret;
}

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
void mid(int8 *src, int8 *s1, int8 *s2, int8 *sub)
{
    int8  *sub1;
    int8  *sub2;
    uint16 n;

    sub1  = strstr(src, s1);
    sub1 += strlen(s1);
    sub2  = strstr(sub1, s2);
    n     = sub2 - sub1;
    strncpy(sub, sub1, n);
    sub[n] = 0;
}

/**
 * @brief Converts the IP address represented by the string into a byte array
 *
 * Converts the IP address represented by a given string into a byte array and stores it in the specified array.
 *
 * @param addr :IP address represented by the
 * @param ip :stores an array of bytes of the transformation result
 */
void inet_addr_(unsigned char *addr, unsigned char *ip)
{
    int   i;
    char  taddr[30];
    char *nexttok;
    char  num;
    strcpy(taddr, (char *)addr);

    nexttok = taddr;
    for (i = 0; i < 4; i++)
    {
        nexttok = strtok(nexttok, ".");
        if (nexttok[0] == '0' && nexttok[1] == 'x')
            num = atoi16(nexttok + 2, 0x10);
        else
            num = atoi16(nexttok, 10);

        ip[i]   = num;
        nexttok = NULL;
    }
}

/**
 * @brief Converts network addresses into dotted decimal strings
 *
 * Converts a given network address into a dotted decimal string form.
 *
 * @param addr:network address
 *
 * @return The pointer to the converted dotted decimal string
 */
char *inet_ntoa(unsigned long addr)
{
    static char addr_str[32];
    memset(addr_str, 0, 32);
    sprintf(addr_str, "%d.%d.%d.%d", (int)(addr >> 24 & 0xFF), (int)(addr >> 16 & 0xFF), (int)(addr >> 8 & 0xFF), (int)(addr & 0xFF));
    return addr_str;
}

/**
 * @brief Translates an IP address into a string with leading zeros
 *
 * Converts a given unsigned long integer IP address to a string format with leading zeros and returns the converted string.
 *
 * @param addr:An unsigned, long, integer representation of the addr IP address
 *
 * @return Converted string pointer
 */
char *inet_ntoa_pad(unsigned long addr)
{
    static char addr_str[16];
    memset(addr_str, 0, 16);
    printf(addr_str, "%03d.%03d.%03d.%03d", (int)(addr >> 24 & 0xFF), (int)(addr >> 16 & 0xFF), (int)(addr >> 8 & 0xFF), (int)(addr & 0xFF));
    return addr_str;
}

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
char verify_ip_address(char *src, uint8 *ip)
{
    int   i;
    int   tnum;
    char  tsrc[50];
    char *tok = tsrc;

    strcpy(tsrc, src);

    for (i = 0; i < 4; i++)
    {
        tok = strtok(tok, ".");
        if (!tok)
            return 0;
        if (tok[0] == '0' && tok[1] == 'x')
        {
            if (!validatoi(tok + 2, 0x10, &tnum))
                return 0;
        }
        else if (!validatoi(tok, 10, &tnum))
            return 0;

        ip[i] = tnum;

        if (tnum < 0 || tnum > 255)
            return 0;
        tok = NULL;
    }
    return 1;
}

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
uint16 htons(uint16 hostshort) //< A 16-bit number in host byte order.
{
#if (SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
    return swaps(hostshort);
#else
    return hostshort;
#endif
}

/**
 * @brief Converts 32-bit host endian-order integers to network endian-order integers
 *
 * Converts a given 32-bit host endian-order integer to a network endian-order integer.
 *
 * @param hostlong: host endian-order integer
 *
 * @return Converted network endian-order integers
 */
unsigned long htonl(unsigned long hostlong) //< hostshort  - A 32-bit number in host byte order.
{
#if (SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
    return swapl(hostlong);
#else
    return hostlong;
#endif
}

/**
 * @brief Converts 16-bit unsigned integers of network endianness to host endianness
 *
 * Converts the 16-bit unsigned integer of a given network endianness to the corresponding host endian.
 *
 * @param netshort:16-bit unsigned integer of the netshort network endian
 *
 * @return A 16-bit unsigned integer in the converted host endianism
 */
unsigned long ntohs(unsigned short netshort) //< netshort - network odering 16bit value
{
#if (SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
    return htons(netshort);
#else
    return netshort;
#endif
}

/**
 * @brief Converts the long integer number of network endenies to host endians
 *
 * Converts the long integer number of network endenians to the corresponding host endianness according to the endianness of the current system.
 *
 * @param netlong:long integer number of the netlong network endeny
 *
 * @return The long integer number of the converted host endians
 */
unsigned long ntohl(unsigned long netlong)
{
#if (SYSTEM_ENDIAN == _ENDIAN_LITTLE_)
    return htonl(netlong);
#else
    return netlong;
#endif
}

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
unsigned short checksum(unsigned char *src, unsigned int len)
{
    u_int  sum, tsum, i, j;
    u_long lsum;

    j = len >> 1;

    lsum = 0;

    for (i = 0; i < j; i++)
    {
        tsum  = src[i * 2];
        tsum  = tsum << 8;
        tsum += src[i * 2 + 1];
        lsum += tsum;
    }

    if (len % 2)
    {
        tsum  = src[i * 2];
        lsum += (tsum << 8);
    }

    sum = lsum;
    sum = ~(sum + (lsum >> 16));
    return (u_short)sum;
}
