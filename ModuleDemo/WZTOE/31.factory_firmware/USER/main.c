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
#include "delay.h"
#include "httpServer.h"
#include "webpage.h"
#include "eeprom.h"
#include "system.h"
#include "user_gpio.h"
#include "w55mh32.h"
#include "aht20.h"
#include "user_rcc.h"
#include "user_usb.h"


extern MYI2C_Struct SENx;
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
uint8_t        reboot_flag                                 = 0;


int main(void)
{
    wiz_NetInfo net_info;

    /* hardware initialization */
    rcc_clk_config();
    delay_init();
    console_usart_init(115200);

    printf("%s factory firmware\r\n", _WIZCHIP_ID_);
    tim3_init();
    user_gpio_init();
    i2c_CfgGpio();
    delay_ms(1000);
    usb_init();
    MYI2C_Init(&SENx, 1000, 0x38);

    /* wiztoe init */
    wiz_toe_init();

    wiz_phy_link_check();
    check_eeprom_network_info(&default_net_info);
    network_init(ethernet_buf, &default_net_info);

    wizchip_getnetinfo(&net_info);
    printf("Please enter% d.% d.% d.% d in your browser to access the %s HTTP server\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3], _WIZCHIP_ID_);

    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)HTML_PAGE);
    httpServer_init(http_tx_ethernet_buf, http_rx_ethernet_buf, _WIZCHIP_SOCK_NUM_, socknumlist); // Initializing the HTTP server
    while (1)
    {
        httpServer_run(SOCKET_ID);
        if (reboot_flag)
        {
            NVIC_SystemReset();
        }
        usb_run();
    }
}


