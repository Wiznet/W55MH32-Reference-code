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
#include "bsp_led.h"
#include "UPnP.h"

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

static uint16_t tcps_port                           = 8000;
static uint16_t udps_port                           = 5000;
static uint8_t  tx_size[_WIZCHIP_SOCK_NUM_]         = {4, 4, 2, 1, 1, 1, 1, 2};
static uint8_t  rx_size[_WIZCHIP_SOCK_NUM_]         = {4, 4, 2, 1, 1, 1, 1, 2};
static uint8_t  ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

int main(void)
{
    /* hardware initialization */
    rcc_clk_config();
    delay_init();

    console_usart_init(115200);

    tim3_init();

    user_led_init();

    printf("%s UPnP example\r\n", _WIZCHIP_ID_);

    /* wiztoe init */
    wiz_toe_init();

    wiz_phy_link_check();

    network_init(ethernet_buf, &default_net_info);

    /* socket rx and tx buff init */
    wizchip_init(tx_size, rx_size);

    UserLED_Control_Init(set_user_led_status);

    do
    {
        printf("Send SSDP.. \r\n");
    } while (SSDPProcess(SOCKET_ID) != 0); // SSDP Search discovery

    if (GetDescriptionProcess(SOCKET_ID) == 0) // GET IGD description
    {
        printf("GetDescription Success!!\r\n");
    }
    else
    {
        printf("GetDescription Fail!!\r\n");
    }

    if (SetEventing(SOCKET_ID) == 0) // Subscribes IGD event messages
    {
        printf("SetEventing Success!!\r\n");
    }
    else
    {
        printf("SetEventing Fail!!\r\n");
    }

    Main_Menu(SOCKET_ID, SOCKET_ID + 1, SOCKET_ID + 2, ethernet_buf, tcps_port, udps_port); // Main menu
    while (1)
    {
    }
}
