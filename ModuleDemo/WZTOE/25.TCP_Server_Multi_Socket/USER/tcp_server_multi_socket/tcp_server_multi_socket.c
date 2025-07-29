#include "tcp_server_multi_socket.h"
#include "socket.h"
#include "stdio.h"
#include "wizchip_conf.h"
#define _LOOPBACK_DEBUG_

uint8_t socket_sn = 0, socket_flag = 0;

/**
 * @brief multi socket loopback function
 * @param buf :buffer pointer
 * @param destport: local port number
 *
 * @return Returns the operation result, 1 for success and error code for failure
 */
int32_t multi_tcps_socket(uint8_t *buf, uint16_t localport)
{
    int32_t  ret;
    uint16_t size = 0, sentsize = 0;

#ifdef _LOOPBACK_DEBUG_
    uint8_t  destip[4];
    uint16_t destport;
#endif

    switch (getSn_SR(socket_sn))
    {
    case SOCK_ESTABLISHED:
        if (getSn_IR(socket_sn) & Sn_IR_CON)
        {
#ifdef _LOOPBACK_DEBUG_
            getSn_DIPR(socket_sn, destip);
            destport = getSn_DPORT(socket_sn);

            printf("%d:Connected - %d.%d.%d.%d : %d\r\n", socket_sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(socket_sn, Sn_IR_CON);
        }
        if ((size = getSn_RX_RSR(socket_sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
        {
            if (size > DATA_BUF_SIZE)
                size = DATA_BUF_SIZE;
            ret = recv(socket_sn, buf, size);

            if (ret <= 0)
                return ret; // check SOCKERR_BUSY & SOCKERR_XXX. For showing the occurrence of SOCKERR_BUSY.
            size      = (uint16_t)ret;
            sentsize  = 0;
            buf[size] = 0x00;
            printf("%d:rece data:%s\r\n", socket_sn, buf);
            while (size != sentsize)
            {
                ret = send(socket_sn, buf + sentsize, size - sentsize);
                if (ret < 0)
                {
                    close(socket_sn);
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        break;
    case SOCK_CLOSE_WAIT:
#ifdef _LOOPBACK_DEBUG_
        printf("%d:CloseWait\r\n", socket_sn);
#endif
        if ((ret = disconnect(socket_sn)) != SOCK_OK)
            return ret;
#ifdef _LOOPBACK_DEBUG_
        printf("%d:Socket Closed\r\n", socket_sn);
#endif
        break;
    case SOCK_INIT:
#ifdef _LOOPBACK_DEBUG_
        printf("%d:Listen, TCP server loopback, port [%d]\r\n", socket_sn, localport);
#endif
        if ((ret = listen(socket_sn)) != SOCK_OK)
            return ret;
        break;
    case SOCK_CLOSED:
#ifdef _LOOPBACK_DEBUG_
        printf("%d:TCP server loopback start\r\n", socket_sn);
#endif
        if ((ret = socket(socket_sn, Sn_MR_TCP, localport, 0x00)) != socket_sn)
            return ret;
#ifdef _LOOPBACK_DEBUG_
        printf("%d:Socket opened\r\n", socket_sn);
#endif
        break;
    default:
        break;
    }

    if (socket_sn < _WIZCHIP_SOCK_NUM_)
    {
        socket_sn++;
    }
    else
    {
        socket_sn = 0;
    }
    return 1;
}
