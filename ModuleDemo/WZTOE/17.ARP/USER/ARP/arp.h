#ifndef _ARP_H_
#define _ARP_H_

#include <stdint.h>

#define ARP_TYPE    0x0806
#define ARP_TYPE_HI 0x08
#define ARP_TYPE_LO 0x06

#define ETHER_TYPE  0x0001
#define PRO_TYPE    0x0800
#define HW_SIZE     6
#define PRO_SIZE    4
#define ARP_REQUEST 0x0001
#define ARP_REPLY   0x0002

extern uint8_t target_ip_addr[4];

typedef struct _ARPMSG
{
    uint8_t  dst_mac[6]; // ff.ff.ff.ff.ff.ff
    uint8_t  src_mac[6];
    uint16_t msg_type;   // ARP (0x0806)
    uint16_t hw_type;    // Ethernet (0x0001)
    uint16_t pro_type;   // IP  (0x0800)
    uint8_t  hw_size;    // 6
    uint8_t  pro_size;   // 4
    uint16_t opcode;     // request (0x0001), reply(0x0002)
    uint8_t  sender_mac[6];
    uint8_t  sender_ip[4];
    uint8_t  tgt_mac[6];  // 00.00.00.00.00.00
    uint8_t  tgt_ip[4];
    uint8_t  trailer[22]; // All zeros
} ARPMSG;

/**
 * @brief   Send arp request
 * @param   sn: socket number
 * @param   port: local socket port
 * @param   dest_ip: ARP Destination IP address
 * @return  none
 */
void arp_request(uint8_t sn, uint16_t port, uint8_t *dest_ip);

/**
 * @brief   ARP reply process
 * @param   sn: socket number
 * @param   buff: The cache that accepts data
 * @param   rlen: The length of the received data
 * @return  none
 */
void arp_reply(uint8_t sn, uint8_t *buff, uint16_t rlen);

/**
 * @brief   Run arp
 * @param   sn: socket number
 * @param   buff: The cache that accepts data
 * @param   dest_ip: ARP Destination IP address
 * @return  none
 */
void do_arp(uint8_t sn, uint8_t *buf, uint8_t *dest_ip);

#endif /* _ARP_H_ */
