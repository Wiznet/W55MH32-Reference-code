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
#include "httpServer.h"
#include "webpage.h"
#include "eeprom.h"


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

static uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE]         = {0};
static uint8_t http_tx_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};
static uint8_t http_rx_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};
static uint8_t socknumlist[1]                              = {SOCKET_ID};
int            main(void)
{
    char        page[2048], mac[20], ip[20], sn[20], gw[20], dns[20];
    wiz_NetInfo net_info;
    /* hardware initialization */
    rcc_clk_config();
    delay_init();
    console_usart_init(115200);

    tim3_init();
    i2c_init();

    printf("%s HTTP Server example\r\n", _WIZCHIP_ID_);

    /* wiztoe init */
    wiz_toe_init();

    wiz_phy_link_check();

    check_eeprom_network_info(&default_net_info);
    network_init(ethernet_buf, &default_net_info);

    wizchip_getnetinfo(&net_info);
    printf("Please enter% d.% d.% d.% d in your browser to access the %s HTTP server\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3], _WIZCHIP_ID_);

    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    sprintf(ip, "%d.%d.%d.%d", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    sprintf(sn, "%d.%d.%d.%d", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    sprintf(gw, "%d.%d.%d.%d", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    sprintf(dns, "%d.%d.%d.%d", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    sprintf(page, (char *)index_page, mac, ip, sn, gw, dns);

    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)page);         // Build HTTP server web pages
    httpServer_init(http_tx_ethernet_buf, http_rx_ethernet_buf, 1, socknumlist); // Initializing the HTTP server
    while (1)
    {
        httpServer_run(SOCKET_ID);
    }
}
