#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wiz_interface.h"
#include "wizchip_conf.h"
#include "dhcp.h"
#include <rtthread.h>

/**
 * @brief Print PHY information
 */
void wiz_print_phy_info(void)
{
    uint8_t get_phy_conf;
    get_phy_conf = getPHYCFGR();
    rt_kprintf("The current Mbtis speed : %dMbps\n", get_phy_conf & 0x02 ? 100 : 10);
    rt_kprintf("The current Duplex Mode : %s\n", get_phy_conf & 0x04 ? "Full-Duplex" : "Half-Duplex");
}

/**
 * @brief Ethernet Link Detection
 */
void wiz_phy_link_check(void)
{
    uint8_t phy_link_status;
    do
    {
        rt_thread_mdelay(1000);
        ctlwizchip(CW_GET_PHYLINK, (void *)&phy_link_status);
        if (phy_link_status == PHY_LINK_ON)
        {
            rt_kprintf("PHY link\n");
            wiz_print_phy_info();
        }
        else
        {
            rt_kprintf("PHY no link\n");
        }
    } while (phy_link_status == PHY_LINK_OFF);
}

/**
 * @brief   print network information
 * @param   none
 * @return  none
 */
void print_network_information(void)
{
    wiz_NetInfo net_info;
    wizchip_getnetinfo(&net_info); // Get chip configuration information

    if (net_info.dhcp == NETINFO_DHCP)
    {
        rt_kprintf("====================================================================================================\n");
        rt_kprintf(" %s network configuration : DHCP\n\n", _WIZCHIP_ID_);
    }
    else
    {
        rt_kprintf("====================================================================================================\n");
        rt_kprintf(" %s network configuration : static\n\n", _WIZCHIP_ID_);
    }

    rt_kprintf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    rt_kprintf(" IP          : %d.%d.%d.%d\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    rt_kprintf(" Subnet Mask : %d.%d.%d.%d\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    rt_kprintf(" Gateway     : %d.%d.%d.%d\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    rt_kprintf(" DNS         : %d.%d.%d.%d\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    rt_kprintf("====================================================================================================\n\n");
}

/**
 * @brief DHCP process
 * @param sn :socket number
 * @param buffer :socket buffer
 */
static uint8_t wiz_dhcp_process(uint8_t sn, uint8_t *buffer)
{
    wiz_NetInfo conf_info;
    uint8_t     dhcp_run_flag = 1;
    uint8_t     dhcp_ok_flag  = 0;
    /* Registration DHCP_time_handler to 1 second timer */
    DHCP_init(sn, buffer);
    rt_kprintf("DHCP running\n");
    while (1)
    {
        switch (DHCP_run())  // Do the DHCP client
        {
        case DHCP_IP_LEASED: // DHCP Acquiring network information successfully

            if (dhcp_ok_flag == 0)
            {
                dhcp_ok_flag  = 1;
                dhcp_run_flag = 0;
            }
            break;

        case DHCP_FAILED:
            dhcp_run_flag = 0;
            break;
        }
        if (dhcp_run_flag == 0)
        {
            rt_kprintf("DHCP %s!\n", dhcp_ok_flag ? "success" : "fail");
            DHCP_stop();

            if (dhcp_ok_flag)
            {
                getIPfromDHCP(conf_info.ip);
                getGWfromDHCP(conf_info.gw);
                getSNfromDHCP(conf_info.sn);
                getDNSfromDHCP(conf_info.dns);
                conf_info.dhcp = NETINFO_DHCP;
                getSHAR(conf_info.mac);
                wizchip_setnetinfo(&conf_info); // Update network information to network information obtained by DHCP
                return 1;
            }
            return 0;
        }
    }
}

/**
 * @brief   set network information
 *
 * First determine whether to use DHCP. If DHCP is used, first obtain the Internet Protocol Address through DHCP.
 * When DHCP fails, use static IP to configure network information. If static IP is used, configure network information directly
 *
 * @param   sn: socketid
 * @param   ethernet_buff:
 * @param   net_info: network information struct
 * @return  none
 */
void network_init(uint8_t *ethernet_buff, wiz_NetInfo *conf_info)
{
    int ret;
    wizchip_setnetinfo(conf_info); // Configuring Network Information
    if (conf_info->dhcp == NETINFO_DHCP)
    {
        ret = wiz_dhcp_process(0, ethernet_buff);
        if (ret == 0)
        {
            conf_info->dhcp = NETINFO_STATIC;
            wizchip_setnetinfo(conf_info);
        }
    }
    print_network_information();
}
