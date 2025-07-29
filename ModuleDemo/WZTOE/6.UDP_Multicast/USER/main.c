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
#include "udp_multicast.h"

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

uint16_t Multicast_port                      = 30000;                                // Multicast port
uint8_t  Multicast_mac[6]                    = {0x01, 0x00, 0x5e, 0x01, 0x01, 0x0b}; // Multicast MAC
uint8_t  Multicast_IP[4]                     = {224, 1, 1, 11};                      // Multicast IP
uint8_t  ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

int main(void)
{
    /* hardware initialization */
    rcc_clk_config();
    delay_init();

    console_usart_init(115200);

    tim3_init();

    printf("%s UDP multicast example\r\n", _WIZCHIP_ID_);

    /* wiztoe init */
    wiz_toe_init();

    wiz_phy_link_check();

    network_init(ethernet_buf, &default_net_info);

    while (1)
    {
        udp_multicast(SOCKET_ID, ethernet_buf, Multicast_mac, Multicast_IP, Multicast_port);
    }
}
