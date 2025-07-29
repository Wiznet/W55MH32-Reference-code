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

uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

int main(void)
{
    wiz_PhyConf phy_conf, get_conf;
    /* hardware initialization */
    rcc_clk_config();
    delay_init();

    console_usart_init(115200);

    tim3_init();

    printf("%s Low Power example\r\n", _WIZCHIP_ID_);

    /* wiztoe init */
    wiz_toe_init();

    wiz_phy_link_check();

    network_init(ethernet_buf, &default_net_info);
    /* config init massage */
    phy_conf.by     = PHY_CONFBY_SW;   // Use software config
    phy_conf.mode   = PHY_MODE_MANUAL; // User config mode
    phy_conf.duplex = PHY_DUPLEX_FULL; // Full duplex
    phy_conf.speed  = PHY_SPEED_100;   // Speed

    /* setting phy 100M and full-duplex mode */
    printf("setting phy 100M and full-duplex mode\r\n");
    ctlwizchip(CW_SET_PHYCONF, &phy_conf);
    ctlwizchip(CW_GET_PHYCONF, &get_conf);

    printf("The current Mbtis speed : %d\r\n", get_conf.speed == PHY_SPEED_100 ? 100 : 10);
    printf("The current Duplex Mode : %s\r\n", get_conf.duplex == PHY_DUPLEX_HALF ? "Half-Duplex" : "Full-Duplex");
    delay_ms(10000);
    /* setting phy 10M and half-duplex mode */
    printf("setting phy 10M and half-duplex mode\r\n");
    phy_conf.speed  = PHY_SPEED_10;
    phy_conf.duplex = PHY_DUPLEX_HALF;
    ctlwizchip(CW_SET_PHYCONF, &phy_conf);
    ctlwizchip(CW_GET_PHYCONF, &get_conf);

    printf("The current Mbtis speed : %d\r\n", get_conf.speed == PHY_SPEED_100 ? 100 : 10);
    printf("The current Duplex Mode : %s\r\n", get_conf.duplex == PHY_DUPLEX_HALF ? "Half-Duplex" : "Full-Duplex");
    delay_ms(10000);
    /* setting phy low power mode */
    printf("setting phy low power mode\r\n");
    wizphy_setphypmode(PHY_POWER_DOWN);
    printf("The current phy is : %s\r\n", (getPHYCFGR() & PHYCFGR_OPMDC_PDOWN) ? "power down mode" : "normal mode");
    printf("PHY is in power down state and cannot be ping reply.\r\n");
    while (1)
    {
    }
}
