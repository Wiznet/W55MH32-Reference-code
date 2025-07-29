#include "jperf.h"
#include "stdio.h"
#include "stdint.h"
#include "socket.h"
#include "string.h"

/**
 * @brief Perform speed measurements
 *
 * Perform speed measurement operations based on the specified sequence number, send buffer, destination IP address, and destination port number.
 *
 * @param SN serial number
 * @param buffer_txst Send buffer pointer
 * @param des_ip Array of destination IP addresses
 * @param des_port Destination port number
 */
void do_jperf(uint8_t sn, uint8_t *buffer_txst, uint8_t *des_ip, uint16_t des_port)
{
    uint32_t len;
    uint16_t port = 50000;
    switch (getSn_SR(sn))
    {
    case SOCK_INIT:
        connect(sn, des_ip, des_port);
        printf("%d:Try to connect to the %d.%d.%d.%d : %d\r\n", sn, des_ip[0], des_ip[1], des_ip[2], des_ip[3], des_port);
        break;
    case SOCK_ESTABLISHED:
        if (getSn_IR(sn) & Sn_IR_CON) //// Socket n interrupt register mask; TCP CON interrupt = connection with peer is successful
        {
            setSn_IR(sn, Sn_IR_CON);  // this interrupt should be write the bit cleared to '1'
            printf("%d:Connected to - %d.%d.%d.%d : %d\r\n", sn, des_ip[0], des_ip[1], des_ip[2], des_ip[3], des_port);
        }
        send(sn, buffer_txst, strlen((char *)buffer_txst)); // Data send process (User's buffer -> Destination through H/W Tx socket buffer)
        len = getSn_RX_RSR(sn);

        if (len >= sizeof(buffer_txst)) // Sn_RX_RSR: Socket n Received Size Register, Receiving data length
        {
            recv(sn, buffer_txst, len); // Data Receive process (H/W Rx socket buffer -> User's buffer)
            send(0, buffer_txst, len);
        }
        else if (len > 0)
        {
            len = recv(sn, buffer_txst, len);
            send(sn, buffer_txst, len);
        }

        break;
    case SOCK_CLOSE_WAIT:
        disconnect(sn);
        break;
    case SOCK_CLOSED:
        socket(sn, Sn_MR_TCP, port++, Sn_MR_ND);
        if (port > 60000)
        {
            port = 50000;
        }
        break;
    }
}
