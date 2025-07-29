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
#include "loopback.h"
#include "socket.h"

#define SOCKET_ID             0
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/* network information */
wiz_NetInfo default_net_info = {
    .mac  = {0x00, 0x08, 0xdc, 0x12, 0x22, 0x05},
    .ip   = {192, 168, 1, 30},
    .gw   = {192, 168, 1, 1},
    .sn   = {255, 255, 255, 0},
    .dns  = {8, 8, 8, 8},
    .dhcp = NETINFO_STATIC
};

uint16_t local_port                          = 5000;
uint8_t  ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};
uint8_t ip[4] = {192,168,1,20};

int main(void)
{
	  uint8_t ret;
	  uint16_t ptr=0;
	  uint32_t addrsel=0;
    /* hardware initialization */
    rcc_clk_config();
    delay_init();

    console_usart_init(115200);

    tim3_init();

    printf("%s UDP example\r\n", _WIZCHIP_ID_);

    /* wiztoe init */
    wiz_toe_init();
		setMR(0x80);
    wiz_phy_link_check();
		setMR(0x80);
		delay_ms(20);
    network_init(ethernet_buf, &default_net_info);
	setMR(0x02);
	setIMR(0xF0);
	setSIMR(0x0FF);
	setSn_IMR(0,0x1F);
	setSn_CR(0,0x10);
	setSn_IR(0,0xFF);
	setIMR(0x01);
	setSn_IMR(0,0xFF);
	setSn_MR(0,0x02);
	setSn_PORT(0,2000);
	setSn_CR(0,0x01);
	setSn_DIPR(0,ip);
	setSn_DPORT(0,5000);
//	ptr=getSn_TX_WR(0);
//	addrsel = ((uint32_t)ptr<<8)+(WIZCHIP_TXBUF_BLOCK(0)<<3);
//	WIZCHIP_WRITE_BUF(addrsel,(uint8_t *)"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890",257);
//	ptr += 257;
//	setSn_TX_WR(0,ptr);
//	setSn_CR(0,0x20);
	printf("getSn_IR%02x\r\n",getSn_IR(0));
	printf("getSn_IR%02x\r\n",getSn_IR(0));
	printf("getSn_IR%02x\r\n",getSn_IR(0));
	printf("getSn_IR%02x\r\n",getSn_IR(0));
	printf("getSn_IR%02x\r\n",getSn_IR(0));
	printf("getSn_IR%02x\r\n",getSn_IR(0));
	printf("getSn_IR%02x\r\n",getSn_IR(0));
	printf("getSn_IR%02x\r\n",getSn_IR(0));
//	 setMR(0x02);
    
//    ret = socket(SOCKET_ID, Sn_MR_UDP, local_port, 0x00);
//		printf("ret = %d\r\n",ret);
    while (1)
    {
			ptr=getSn_TX_WR(0);
	addrsel = ((uint32_t)ptr<<8)+(WIZCHIP_TXBUF_BLOCK(0)<<3);
	WIZCHIP_WRITE_BUF(addrsel,(uint8_t *)"123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890",257);
	ptr += 257;
	WIZCHIP_WRITE(((0x0024 << 8) + (WIZCHIP_SREG_BLOCK(0) << 3)),(unsigned char)((ptr&0xFF00U)>>8U));
	WIZCHIP_WRITE(((0x0025 << 8) + (WIZCHIP_SREG_BLOCK(0) << 3)),(unsigned char)((ptr&0x00FFU)));
	//setSn_TX_WR(0,ptr);
	setSn_CR(0,0x20);
			  //ret = sendto(SOCKET_ID, (uint8_t*)"123123", 6, ip, 5000);
			setSn_IR(0,0xff);
			setIR(0xff);
			printf("ret:%d\r\n",ret);
			printf("getIR:%02x\r\n",getIR());
			printf("getSn_IR:%02x\r\n",getSn_IR(SOCKET_ID));
			
			  delay_ms(1000);
        //loopback_udps(SOCKET_ID, ethernet_buf, local_port);
    }
}
