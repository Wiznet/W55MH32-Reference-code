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
#include "do_dns.h"
#include "httpclient.h"

#define SOCKET_ID             0
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/* network information */
wiz_NetInfo default_net_info = {
    .mac  = {0x00, 0x08, 0xdc, 0x12, 0x22, 0x12},
    .ip   = {192, 168, 1, 30},
    .gw   = {192, 168, 1, 1},
    .sn   = {255, 255, 255, 0},
    .dns  = {8, 8, 8, 8},
    .dhcp = NETINFO_DHCP
};

uint8_t org_server_name[]                   = "httpbin.org";
uint8_t org_server_ip[4]                    = {0}; /*httpbin.org IP adress */
uint8_t org_port                            = 80;  /*httpbin.org port*/
uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

int main(void)
{
    uint16_t len;
    /* hardware initialization */
    rcc_clk_config();
    delay_init();

    console_usart_init(115200);

    tim3_init();

    printf("%s HTTP Client example\r\n", _WIZCHIP_ID_);

    /* wiztoe init */
    wiz_toe_init();

    wiz_phy_link_check();

    network_init(ethernet_buf, &default_net_info);

    if (do_dns(ethernet_buf, org_server_name, org_server_ip))
    {
        printf("DNS request failed.\r\n");
        while (1)
        {
        }
    }
    // Send a GET request.
    len = http_get_pkt(ethernet_buf);
    do_http_request(SOCKET_ID, ethernet_buf, len, org_server_ip, org_port);

    // Send a POST request.
    len = http_post_pkt(ethernet_buf);
    do_http_request(SOCKET_ID, ethernet_buf, len, org_server_ip, org_port);
    while (1)
    {
    }
}
