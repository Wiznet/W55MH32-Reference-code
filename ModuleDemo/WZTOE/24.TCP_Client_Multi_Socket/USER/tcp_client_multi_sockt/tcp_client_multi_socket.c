#include "tcp_client_multi_socket.h"
#include "socket.h"
#include "stdio.h"
#include "wizchip_conf.h"

#define _LOOPBACK_DEBUG_

uint8_t  socket_sn = 0, socket_flag = 0;
uint16_t any_port = 50000;

/**
 * @brief Multi-TCP connection socket operation
 *
 * Perform multi-TCP connection socket operations based on the given buffer, destination IP address, and destination port number.
 *
 * @param buf buffer pointer
 * @param destip array of destination IP addresses
 * @param destport destination port number
 *
 * @return Returns the operation result, 1 for success and error code for failure
 */
int32_t multi_tcpc_socket(uint8_t *buf, uint8_t *destip, uint16_t destport)
{
    int32_t  ret; // return value for SOCK_ERRORs
    uint16_t size = 0, sentsize = 0;
    // Socket Status Transitions
    // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
    switch (getSn_SR(socket_sn))
    {
    case SOCK_ESTABLISHED:
        if (getSn_IR(socket_sn) & Sn_IR_CON) // Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
        {
#ifdef _LOOPBACK_DEBUG_
            printf("%d:Connected to - %d.%d.%d.%d : %d\r\n", socket_sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(socket_sn, Sn_IR_CON); // this interrupt should be write the bit cleared to '1'
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        // Data Transaction Parts; Handle the [data receive and send] process
        //////////////////////////////////////////////////////////////////////////////////////////////
        if ((size = getSn_RX_RSR(socket_sn)) > 0) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
        {
            if (size > DATA_BUF_SIZE)
                size = DATA_BUF_SIZE;               // DATA_BUF_SIZE means user defined buffer size (array)
            ret       = recv(socket_sn, buf, size); // Data Receive process (H/W Rx socket buffer -> User's buffer)
            buf[size] = 0x00;
            printf("%d:rece from %d.%d.%d.%d:%d data:%s\r\n", socket_sn, destip[0], destip[1], destip[2], destip[3], destport, buf);
            if (ret <= 0)
                return ret; // If the received data length <= 0, receive failed and process end
            size     = (uint16_t)ret;
            sentsize = 0;

            // Data sentsize control
            while (size != sentsize)
            {
                ret = send(socket_sn, buf + sentsize, size - sentsize); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
                if (ret < 0)                                            // Send Error occurred (sent data length < 0)
                {
                    close(socket_sn);                                   // socket close
                    return ret;
                }
                sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
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
        printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", socket_sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
        if ((ret = connect(socket_sn, destip, destport)) != SOCK_OK)
            return ret; //	Try to TCP connect to the TCP server (destination)
        break;
    case SOCK_CLOSED:
        close(socket_sn);
        if ((ret = socket(socket_sn, Sn_MR_TCP, any_port++, 0x00)) != socket_sn)
        {
            if (any_port == 0xffff)
                any_port = 50000;
            return ret; // TCP socket open with 'any_port' port number
        }
#ifdef _LOOPBACK_DEBUG_
        printf("%d:TCP client loopback start\r\n", socket_sn);
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
