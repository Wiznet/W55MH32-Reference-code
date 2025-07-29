#include "do_dns.h"
#include <stdio.h>
#include "dns.h"
#include "wizchip_conf.h"
#include "wiz_interface.h"

/**
 * @brief   DNS domain name resolution
 * @param   ethernet_buff: ethernet buffer
 * @param   domain_name:Domain name to be resolved
 * @param   domain_ip:Resolved Internet Protocol Address
 * @return  0:success;-1:failed
 */
int do_dns(uint8_t *buf, uint8_t *domain_name, uint8_t *domain_ip)
{
    int         dns_ok_flag  = 0;
    int         dns_run_flag = 1;
    wiz_NetInfo net_info;
    uint8_t     dns_retry_cnt = 0;
    DNS_init(0, buf); // DNS client init
    wizchip_getnetinfo(&net_info);
    while (1)
    {
        switch (DNS_run(net_info.dns, domain_name, domain_ip)) // Read the DNS_run return value
        {
        case DNS_RET_FAIL:                                     // The DNS domain name is successfully resolved
        {
            if (dns_retry_cnt < DNS_RETRY)                     // Determine whether the parsing is successful or whether the parsing exceeds the number of times
            {
                dns_retry_cnt++;
            }
            else
            {
                printf("> DNS Failed\r\n");
                dns_ok_flag  = -1;
                dns_run_flag = 0;
            }
            break;
        }
        case DNS_RET_SUCCESS: {
            printf("> Translated %s to %d.%d.%d.%d\r\n", domain_name, domain_ip[0], domain_ip[1], domain_ip[2], domain_ip[3]);
            dns_ok_flag  = 0;
            dns_run_flag = 0;
            break;
        }
        }
        if (dns_run_flag != 1)
        {
            return dns_ok_flag;
        }
    }
}
