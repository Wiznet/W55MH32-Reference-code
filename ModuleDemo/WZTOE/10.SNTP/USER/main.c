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
#include "sntp.h"
#include "bsp_rtc.h"

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

uint8_t              ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};
uint8_t              sntp_server_name[]                  = "cn.pool.ntp.org";
uint8_t              sntp_server_ip[4]                   = {162, 159, 200, 1};
uint8_t              timezone                            = 39; // china
datetime             date                                = {0};
uint8_t              week_name[7][20]                    = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
extern _calendar_obj calendar; //时钟结构体
int main(void)
{
    /* hardware initialization */
    rcc_clk_config();
    delay_init();
    console_usart_init(115200);
    tim3_init();
    RTC_Init();

    printf("%s SNTP example\r\n", _WIZCHIP_ID_);

    /* wiztoe init */
    wiz_toe_init();
    wiz_phy_link_check();
    network_init(ethernet_buf, &default_net_info);

    if (do_dns(ethernet_buf, sntp_server_name, sntp_server_ip))
    {
        while (1)
        {
        }
    }
    SNTP_init(SOCKET_ID, sntp_server_ip, timezone, ethernet_buf);
    while (1) //上电自动获取时间
    {
        if (SNTP_run(&date))
        {
            RTC_Set(date.yy, date.mo, date.dd, date.hh, date.mm, date.ss);
            break;
        }
    }
    while (1)
    {
        printf("Beijing time now: %04d-%02d-%02d  %s  %02d:%02d:%02d\r\n", calendar.w_year, calendar.w_month, calendar.w_date, week_name[calendar.week], calendar.hour, calendar.min, calendar.sec);
        delay_ms(1000);
    }
}

