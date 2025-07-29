#ifndef _PING_H_
#define _PING_H_

#include "wizchip_conf.h"
#include "socket.h"

#define BUF_LEN      128
#define PING_REQUEST 8
#define PING_REPLY   0
#define CODE_ZERO    0

#define SOCKET_ERROR  1
#define TIMEOUT_ERROR 2
// #define SUCCESS 3
#define REPLY_ERROR 4
extern uint8_t req;

typedef struct pingmsg
{
    uint8_t  Type;          // 0 - Ping Reply, 8 - Ping Request
    uint8_t  Code;          // Always 0
    uint16_t CheckSum;      // Check sum
    uint16_t ID;            // Identification
    uint16_t SeqNum;        // Sequence Number
    int8_t   Data[BUF_LEN]; // Ping Data  : 1452 = IP RAW MTU - sizeof(Type+Code+CheckSum+ID+SeqNum)
} PINGMSGR;

/**
 *@brief	Set the number of times to ping the public IP address
 *@param	s:socket number
 *@param	pCount:The number of pings
 *@param	addr:Public IP address
 *@return	none
 */
void ping_count(uint8_t s, uint16_t pCount, uint8_t *addr);

/**
 * @brief ping request function
 * @param s:socket number
 * @param addr:Ping address
 * @return none
 */
void ping_request(uint8_t s, uint8_t *addr);

/**
 * @brief Parse Ping replies
 * @param s:socket number
 * @param addr:Ping address
 * @return none
 */
void ping_reply(uint8_t s, uint8_t *addr, uint16_t rlen);

/**
 * @brief Ping the operation
 *
 * Based on the given serial number, remote IP address, and number of requests, ping is performed.
 *
 * @param SN:socket number
 * @param remote_ip:Remote IP address pointer
 * @param req_num:Number of requests
 */
void do_ping(uint8_t sn, uint8_t *remote_ip, uint8_t req);
#endif
