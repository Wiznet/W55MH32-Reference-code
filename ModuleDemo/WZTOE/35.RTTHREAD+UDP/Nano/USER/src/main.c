#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"
#include <rtthread.h>
#include "wizchip_conf.h"
#include "wiz_interface.h"
#include "loopback.h"

#define DEFAULT_MAC_EN        1                   //1: Use the default MAC address, 0: Use the user - defined MAC address
#define SOCKET_UDPC             0
#define SOCKET_UDPS              1
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

#define TASK_UDPC_PRIO     1                      
#define TASK_UDPC_STK_SIZE 256                    
static rt_thread_t Task_UDPC_Handler = RT_NULL;   
void               Task_UDPC(void *pvParameters); 

#define TASK_UDPS_PRIO     1                      
#define TASK_UDPS_STK_SIZE 256                    
static rt_thread_t Task_UDPS_Handler = RT_NULL;   
void               Task_UDPS(void *pvParameters); 

/* network information */
wiz_NetInfo default_net_info = {
    .mac  = {0x00, 0x08, 0xdc, 0x12, 0x22, 0x12}, //User-defined MAC address
    .ip   = {192, 168, 1, 30},
    .gw   = {192, 168, 1, 1},
    .sn   = {255, 255, 255, 0},
    .dns  = {8, 8, 8, 8},
    .dhcp = NETINFO_DHCP
};

//tcp client
uint8_t  dest_ip[4]                          = {192, 168, 1, 20};
uint16_t dest_port                           = 8080;
uint8_t  tcpc_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};

//tcp server
uint16_t local_port = 8080;
uint8_t  tcps_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {0};
int main(void)
{
    while (1)
		{
			rt_thread_mdelay(1);
		}
}

int app_init(void)
{
		rt_kprintf("%s RTThread UDP example\n",_WIZCHIP_ID_);
		wiz_toe_init();
	  reg_wizchip_cris_cbfunc(rt_enter_critical,rt_exit_critical);
#if DEFAULT_MAC_EN == 1
    getSHAR(default_net_info.mac);
#endif

    wiz_phy_link_check();

    network_init(tcpc_ethernet_buf, &default_net_info);

    /* Enable keepalive,Parameter 2 is the keep alive time, with a unit of 5 seconds */
    setSn_KPALVTR(SOCKET_UDPC, 6); // 30s keepalive
	  setSn_KPALVTR(SOCKET_UDPS, 6); // 30s keepalive
	
	  Task_UDPC_Handler = rt_thread_create("Task TCP Client Handler", Task_UDPC, RT_NULL, TASK_UDPC_STK_SIZE, TASK_UDPC_PRIO, 20);
    Task_UDPS_Handler = rt_thread_create("Task TCP Server Handler", Task_UDPS, RT_NULL, TASK_UDPS_STK_SIZE, TASK_UDPS_PRIO, 20);
	  rt_thread_startup(Task_UDPC_Handler);
    rt_thread_startup(Task_UDPS_Handler);
	  return 0;
}
INIT_APP_EXPORT(app_init);

void Task_UDPC(void *parameter)
{
	  printf("UDP Client loopback\r\n");
	  printf("Only when the destination address and port number are consistent will it be returned.\r\n");
    while (1)
    {
			  loopback_udpc(SOCKET_UDPC, tcpc_ethernet_buf, dest_ip, dest_port);
        rt_thread_mdelay(1);			
    }
}

void Task_UDPS(void *parameter)
{
	  printf("UDP Server loopback\r\n");
	  printf("Data from any source will be transmitted back.\r\n");
    while (1)
    {
			  loopback_udps(SOCKET_UDPS, tcps_ethernet_buf,local_port);
			  rt_thread_mdelay(1);
    }
}
