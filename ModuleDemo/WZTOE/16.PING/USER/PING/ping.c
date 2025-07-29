/**
******************************************************************************
* @file   		ping.c
* @author  		WIZnet Software Team
* @version 		V1.0
* @date    		2015-12-12
* @brief   		ping Demo function
******************************************************************************
**/
#include "ping.h"
#include "stdio.h"
#include "utility.h"
#include "wiz_interface.h"
#include "delay.h"

#define PING_DEBUG
PINGMSGR PingRequest;
PINGMSGR PingReply;

static uint16_t RandomID            = 0x1234; // Random ID
static uint16_t RandomSeqNum        = 0x4321;
uint8_t         ping_reply_received = 0;
uint8_t         rep                 = 0; // rep is the number of ping reply received
uint8_t         req                 = 0; // request number

/**
 *@brief	Set the number of times to ping the public IP address
 *@param	s:socket number
 *@param	pCount:The number of pings
 *@param	addr:Public IP address
 *@return	none
 */
void ping_count(uint8_t s, uint16_t pCount, uint8_t *addr)
{
    uint16_t rlen, cnt, i;
    cnt = 0;

    for (i = 0; i < pCount + 1; i++)
    {
        if (i != 0)
        {
            // Output count number
            printf("No.%d  ", i);
        }
        switch (getSn_SR(s))
        {
        case SOCK_CLOSED:
            close(s);
            // Create Socket
            IINCHIP_WRITE(WZTOE_Sn_PROTO(s), IPPROTO_ICMP);
            if (socket(s, Sn_MR_IPRAW, 3000, 0) != 0)
            {
            }
            // Check socket register
            while (getSn_SR(s) != SOCK_IPRAW);
            break;
        case SOCK_IPRAW:
            ping_request(s, addr);
            req++;
            while (1)
            {
                if ((rlen = getSn_RX_RSR(s)) > 0)
                {
                    ping_reply(s, addr, rlen);
                    rep++;
                    if (ping_reply_received)
                        break;
                }
                if ((cnt > 1000))
                {
                    printf("Request Time out\r\n\r\n");
                    cnt = 0;
                    break;
                }
                else
                {
                    cnt++;
                    delay_ms(5);
                }
            }
            break;
        default:
            break;
        }
        if (req >= pCount)
        {
            printf("Ping Request = %d, Ping Reply = %d, Lost = %d\r\n", req, rep, req - rep);
        }
    }
}

/**
 * @brief ping request function
 * @param s:socket number
 * @param addr:Ping address
 * @return none
 */
void ping_request(uint8_t s, uint8_t *addr)
{
    uint16_t i;
    ping_reply_received = 0;
    PingRequest.Type    = PING_REQUEST;
    PingRequest.Code    = CODE_ZERO;
    PingRequest.ID      = htons(RandomID++);
    PingRequest.SeqNum  = htons(RandomSeqNum++);
    for (i = 0; i < BUF_LEN; i++)
    {
        PingRequest.Data[i] = (i) % 8;
    }
    PingRequest.CheckSum = 0;

    PingRequest.CheckSum = htons(checksum((uint8_t *)&PingRequest, sizeof(PingRequest)));

    if (sendto(s, (uint8_t *)&PingRequest, sizeof(PingRequest), addr, 3000) == 0)
    {
        printf("Fail to send ping-reply packet\r\n");
    }
    else
    {
        printf("Ping:%d.%d.%d.%d\r\n", (addr[0]), (addr[1]), (addr[2]), (addr[3]));
    }
}

/**
 * @brief Parse Ping replies
 * @param s:socket number
 * @param addr:Ping address
 * @return none
 */
void ping_reply(uint8_t s, uint8_t *addr, uint16_t rlen)
{
    uint16_t tmp_checksum;
    uint16_t len;
    uint16_t i;
    uint8_t  data_buf[136];

    uint16_t port = 3000;
    PINGMSGR PingReply;
    len = recvfrom(s, data_buf, rlen, addr, &port);

    if (data_buf[0] == PING_REPLY)
    {
        PingReply.Type     = data_buf[0];
        PingReply.Code     = data_buf[1];
        PingReply.CheckSum = (data_buf[3] << 8) + data_buf[2];
        PingReply.ID       = (data_buf[5] << 8) + data_buf[4];
        PingReply.SeqNum   = (data_buf[7] << 8) + data_buf[6];

        for (i = 0; i < len - 8; i++)
        {
            PingReply.Data[i] = data_buf[8 + i];
        }
        tmp_checksum = ~checksum(data_buf, len);
        if (tmp_checksum != 0xffff)
            printf("tmp_checksum = %x\r\n", tmp_checksum);

        else
        {
            printf("Reply from %3d.%3d.%3d.%3d ID=%x Byte=%d\r\n\r\n", (addr[0]), (addr[1]), (addr[2]), (addr[3]), htons(PingReply.ID), (rlen + 6));
            ping_reply_received = 1;
        }
    }
    else if (data_buf[0] == PING_REQUEST)
    {
        PingReply.Code     = data_buf[1];
        PingReply.Type     = data_buf[2];
        PingReply.CheckSum = (data_buf[3] << 8) + data_buf[2];
        PingReply.ID       = (data_buf[5] << 8) + data_buf[4];
        PingReply.SeqNum   = (data_buf[7] << 8) + data_buf[6];
        for (i = 0; i < len - 8; i++)
        {
            PingReply.Data[i] = data_buf[8 + i];
        }
        tmp_checksum       = PingReply.CheckSum;
        PingReply.CheckSum = 0;
        if (tmp_checksum != PingReply.CheckSum)
        {
            printf(" \n CheckSum is in correct %x shold be %x \n", (tmp_checksum), htons(PingReply.CheckSum));
        }
        else
        {
        }
        printf("	Request from %d.%d.%d.%d  ID:%x SeqNum:%x  :data size %d bytes\r\n",
               (addr[0]), (addr[1]), (addr[2]), (addr[3]), (PingReply.ID), (PingReply.SeqNum), (rlen + 6));
        ping_reply_received = 1;
    }
    else
    {
        printf(" Unkonwn msg. \n");
    }
}

/**
 * @brief Ping the operation
 *
 * Based on the given serial number, remote IP address, and number of requests, ping is performed.
 *
 * @param SN:socket number
 * @param remote_ip:Remote IP address pointer
 * @param req_num:Number of requests
 */
void do_ping(uint8_t sn, uint8_t *remote_ip, uint8_t req_num)
{
    if (req >= req_num)
    {
        close(sn);
        return;
    }
    else
    {
        ping_count(sn, req_num, remote_ip);
    }
}
