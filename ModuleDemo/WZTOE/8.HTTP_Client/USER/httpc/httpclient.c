#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "httpclient.h"
#include "delay.h"

/**
 * @brief   HTTP GET :  Request package combination package.
 * @param   pkt:    Array cache for grouping packages
 * @return  pkt:    Package length
 */
uint32_t http_get_pkt(uint8_t *pkt)
{
    *pkt = 0;
    // request type URL HTTP protocol version
    strcat((char *)pkt, "GET /get?username=admin&password=admin HTTP/1.1\r\n");

    // Host address, which can be a domain name or a specific IP address.
    strcat((char *)pkt, "Host: httpbin.org\r\n");
    // end
    strcat((char *)pkt, "\r\n");

    return strlen((char *)pkt);
}

/**
 * @brief   HTTP POST :  Request package combination package.
 * @param   pkt:    Array cache for grouping packages
 * @return  pkt:    Package length
 */
uint32_t http_post_pkt(uint8_t *pkt)
{
    *pkt = 0;
    // request type URL HTTP protocol version
    strcat((char *)pkt, "POST /post HTTP/1.1\r\n");

    // Host address, which can be a domain name or a specific IP address.
    strcat((char *)pkt, "Host: httpbin.org\r\n");

    // Main content format
    strcat((char *)pkt, "Content-Type:application/x-www-form-urlencode\r\n");

    // Main content lenght
    strcat((char *)pkt, "Content-Length:29\r\n");

    // separator
    strcat((char *)pkt, "\r\n");

    // main content
    strcat((char *)pkt, "username=admin&password=admin");
    return strlen((char *)pkt);
}

/**
 * @brief   HTTP Client get data stream test.
 * @param   sn:         socket number
 * @param   buf:        request message content
 * @param		len					request message length
 * @param   destip:     destion ip
 * @param   destport:   destion port
 * @return  0:timeout,1:Received response..
 */

uint8_t do_http_request(uint8_t sn, uint8_t *buf, uint16_t len, uint8_t *destip, uint16_t destport)
{
    uint16_t local_port   = 50000;
    uint16_t recv_timeout = 0;
    uint8_t  send_flag    = 0;
    while (1)
    {
        switch (getSn_SR(sn))
        {
        case SOCK_INIT:
            // Connect to http server.
            connect(sn, destip, destport);
            break;
        case SOCK_ESTABLISHED:
            if (send_flag == 0)
            {
                // send request
                send(sn, buf, len);
                send_flag = 1;
                printf("send request:\r\n");
                for (uint16_t i = 0; i < len; i++)
                {
                    printf("%c", *(buf + i));
                }
                printf("\r\n");
            }
            // Response content processing
            len = getSn_RX_RSR(sn);
            if (len > 0)
            {
                printf("Receive response:\r\n");
                while (len > 0)
                {
                    len = recv(sn, buf, len);
                    for (uint16_t i = 0; i < len; i++)
                    {
                        printf("%c", *(buf + i));
                    }
                    len = getSn_RX_RSR(sn);
                }
                printf("\r\n");
                disconnect(sn);
                close(sn);
                return 1;
            }
            else
            {
                recv_timeout++;
                delay_ms(1000);
            }
            // timeout handling
            if (recv_timeout > 10)
            {
                printf("request fail!\r\n");
                disconnect(sn);
                close(sn);
                return 0;
            }
            break;
        case SOCK_CLOSE_WAIT:
            // If there is a request error, the server will immediately send a close request,
            // so the error response content needs to be processed here.
            len = getSn_RX_RSR(sn);
            if (len > 0)
            {
                printf("Receive response:\r\n");
                while (len > 0)
                {
                    len = recv(sn, buf, len);
                    for (uint16_t i = 0; i < len; i++)
                    {
                        printf("%c", *(buf + i));
                    }
                    len = getSn_RX_RSR(sn);
                }
                printf("\r\n");
                disconnect(sn);
                close(sn);
                return 1;
            }
            close(sn);
            break;
        case SOCK_CLOSED:
            // close socket
            close(sn);
            // open socket
            socket(sn, Sn_MR_TCP, local_port, 0x00);
            break;
        default:
            break;
        }
    }
}
