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
#include "user_gpio.h"
#include "aht20.h"
#include "do_mqtt.h"

#include "w55mh32_wztoe.h"

#define DEFAULT_MAC_EN        1                   //1: Use the default MAC address, 0: Use the user - defined MAC address
#define SOCKET_MQTT_ID         1
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

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
static uint8_t mqtt_send_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};
static uint8_t mqtt_recv_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

wiz_tls_context tlsContext;
extern mqttconn mqtt_params;
extern MYI2C_Struct SENx;

int main(void)
{
    /* hardware initialization */
    rcc_clk_config();
    delay_init();
    console_usart_init(115200);
    user_gpio_init();
    MYI2C_Init(&SENx, 1000, 0x38);
		printf("W55MH32 MQTTS Example\r\n");

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

    uint32_t ver = wiz_crypt_version();
    printf("WIZnet CARD Secure Test Demo V1.0, secure lib version is V%02x.%02x.%02x.%02x\n", ver >> 24, (ver >> 16) & 0xFF, (ver >> 8) & 0xFF, ver & 0xFF);
    printf("WIZnet CARD Crypt Test V1.0 start......\r\n");

    network_init(mqtt_send_ethernet_buf, &default_net_info);
    mqtt_init(SOCKET_MQTT_ID, mqtt_send_ethernet_buf, mqtt_recv_ethernet_buf);

    while(1)
    {
			do_mqtt();
    }
}
