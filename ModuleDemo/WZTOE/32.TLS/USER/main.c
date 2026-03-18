/**
******************************************************************************
* @file    			main.c
* @version 			V1.0
* @date    			2024-08-21
* @brief 		    Main function entry
*
* @company  		WIZnet
* @website  		https://wiznet.io/
* @gitee            https://gitee.com/wiznet-hk
* @github           https://github.com/Wiznet/
******************************************************************************
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wizchip_conf.h"
#include "wiz_interface.h"
#include "bsp_tim.h"
#include "bsp_uart.h"
#include "bsp_rcc.h"
#include "delay.h"
#include "w55mh32_trng.h"
#include "SSLInterface.h"
#include "wiz_crypt_version.h"
#include "socket.h"

#define DEFAULT_MAC_EN        1                   //1: Use the default MAC address, 0: Use the user - defined MAC address
#define SOCKET_DHCP_ID        0
#define SOCKET_SSL_ID         1
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)
#define LOCAL_PORT            5000
#define SSL_TARGET_PORT       443


/* network information */
wiz_NetInfo default_net_info = {
    .mac  = {0x00, 0x08, 0xdc, 0x12, 0x22, 0x12},
    .ip   = {10, 0, 1, 140},
    .gw   = {10, 0, 1, 254},
    .sn   = {255, 255, 255, 0},
    .dns  = {8, 8, 8, 8},
    .dhcp = NETINFO_DHCP
};

uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

uint8_t ssl_target_ip[4] = {182, 61, 244, 181}; // baidu

wiz_tls_context tlsContext;

int main(void)
{
    int recv_len = 0;

    /* hardware initialization */
    rcc_clk_config();
    delay_init();
    console_usart_init(115200);

	printf("W55MH32 TLS Example\r\n");

    /* hardware crypt initialization */
    *(uint32_t *)(0x400210F0) = 0x01;
    *(uint32_t *)(0x40016C00) = 0xCDED3526;
    *(uint32_t *)(0x40016CCC) = 0x07;

    /* hardware TRNG enable */
    TRNG_Out(ENABLE);

    tim3_init();

    /* wiztoe init */
    wiz_toe_init();
#if DEFAULT_MAC_EN == 1
    getSHAR(default_net_info.mac);
#endif
    wiz_phy_link_check();

    network_init(ethernet_buf, &default_net_info);

    uint32_t ver = wiz_crypt_version();
    printf("WIZnet CARD Secure Test Demo V1.0, secure lib version is V%02x.%02x.%02x.%02x\n", ver >> 24, (ver >> 16) & 0xFF, (ver >> 8) & 0xFF, ver & 0xFF);
    printf("WIZnet CARD Crypt Test V1.0 start......\r\n");

	/* open socket */
    wiz_tls_socket(&tlsContext, SOCKET_SSL_ID, LOCAL_PORT);

	/* tls init */
    wiz_tls_init(&tlsContext, 2000, (char *)ssl_target_ip, NULL, NULL, NULL);
    
	/* connect server*/
	connect(tlsContext.socket_fd, (uint8_t *)ssl_target_ip, SSL_TARGET_PORT);
    
	/* tls handshake */
	wiz_tls_connect(&tlsContext, (char *)ssl_target_ip, SSL_TARGET_PORT);

    printf("W55MH32 MbedTLS Transmission test start......\r\n");

    if (1)
    {
        printf("W55MH32 get baidu.com html content......\r\n");
        mbedtls_ssl_conf_read_timeout(tlsContext.conf, 200);
        sprintf(ethernet_buf, "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nUser-Agent: W55MH32/1.0\r\nAccept: text/html\r\nConnection: close\r\n\r\n");
        wiz_tls_write(&tlsContext, (unsigned char *)ethernet_buf, strlen(ethernet_buf));

        while (1)
        {
            recv_len = wiz_tls_read(&tlsContext, ethernet_buf, ETHERNET_BUF_MAX_SIZE - 1);

            if (recv_len > 0)
            {
                printf("recv_len:%d\r\n", recv_len);
                ethernet_buf[recv_len] = '\0';
                printf("Received: %s", ethernet_buf);
            }
        }
    }
}
