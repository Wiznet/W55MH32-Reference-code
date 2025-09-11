#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wiz_interface.h"
#include "wizchip_conf.h"
#include "dhcp.h"
#include "delay.h"

/**
 * @brief Print PHY information
 */
void wiz_print_phy_info(void)
{
    uint8_t get_phy_conf;
    get_phy_conf = getPHYCFGR();
    printf("The current Mbtis speed : %dMbps\r\n", get_phy_conf & 0x02 ? 100 : 10);
    printf("The current Duplex Mode : %s\r\n", get_phy_conf & 0x04 ? "Full-Duplex" : "Half-Duplex");
}

/**
 * @brief Ethernet Link Detection
 */
void wiz_phy_link_check(void)
{
    uint8_t phy_link_status;
    do
    {
        delay_ms(1000);
        ctlwizchip(CW_GET_PHYLINK, (void *)&phy_link_status);
        if (phy_link_status == PHY_LINK_ON)
        {
            printf("PHY link\r\n");
            wiz_print_phy_info();
        }
        else
        {
            printf("PHY no link\r\n");
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
        printf("====================================================================================================\r\n");
        printf(" %s network configuration : DHCP\r\n\r\n", _WIZCHIP_ID_);
    }
    else
    {
        printf("====================================================================================================\r\n");
        printf(" %s network configuration : static\r\n\r\n", _WIZCHIP_ID_);
    }

    printf(" MAC         : %02X:%02X:%02X:%02X:%02X:%02X\r\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf(" IP          : %d.%d.%d.%d\r\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf(" Subnet Mask : %d.%d.%d.%d\r\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf(" Gateway     : %d.%d.%d.%d\r\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf(" DNS         : %d.%d.%d.%d\r\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
    printf("====================================================================================================\r\n\r\n");
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
    printf("DHCP running\r\n");
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
            printf("DHCP %s!\r\n", dhcp_ok_flag ? "success" : "fail");
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
