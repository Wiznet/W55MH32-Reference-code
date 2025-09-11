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
#include "udp_config.h"
#include "config.h"
#include "fw_update.h"

#define ETHERNET_BUF_MAX_SIZE 2048

extern CONFIG_MSG system_cfg;

/* network information */
wiz_NetInfo default_net_info = {
    .mac  = {0x00, 0x08, 0xdc, 0x12, 0x22, 0x12}, 
    .ip   = {192, 168, 1, 30},
    .gw   = {192, 168, 1, 1},
    .sn   = {255, 255, 255, 0},
    .dns  = {8, 8, 8, 8},
    .dhcp = NETINFO_STATIC
};

uint8_t ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

int main(void)
{
	/* hardware initialization */
	rcc_clk_config();
	delay_init();

	console_usart_init(115200);

	tim3_init();

	/* wiztoe init */
	wiz_toe_init();
	
	printf("boot\r\n");

	system_config_param_get();
	system_first_run_check();
	if (system_cfg.fw_len > 0 && system_cfg.fw_len != 0xFFFFFFFF)
	{
		if (app_copy(system_cfg.fw_len, system_cfg.fw_checksum) < 0)
		{
			printf("Copy fw error\r\n");
		}
		else
		{
			system_cfg.fw_len = 0;
			system_cfg.fw_checksum = 0;
			system_config_param_save();
			reboot_app();
		}
	}
	if (app_is_inside())
	{
		printf("reboot app\r\n");
		reboot_app();
	}
	else
	{
		printf("not app\r\n");
	}

	memcpy(&default_net_info.mac, &system_cfg.mac, 6);
	memcpy(&default_net_info.ip, &system_cfg.lip, 4);
	memcpy(&default_net_info.gw, &system_cfg.gw, 4);
	memcpy(&default_net_info.sn, &system_cfg.sub, 4);
	memcpy(&default_net_info.dns, &system_cfg.dns, 4);

	

	wiz_phy_link_check();
	network_init(ethernet_buf, &default_net_info);

	while (1)
	{
		do_udp_config(SOCK_UDP_CONFIG); // Run and precess UpperComputer command.
		fw_update_process();
		if (reboot_flag)
		{
			system_reboot();
		}
	}
}
