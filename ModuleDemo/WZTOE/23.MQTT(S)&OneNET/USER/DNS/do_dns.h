#ifndef __DO_DNS_H__
#define __DO_DNS_H__

#include <stdint.h>

#define DNS_RET_FAIL    0
#define DNS_RET_SUCCESS 1
#define DNS_RETRY       3 /* 3 times */

/**
 * @brief   DNS domain name resolution
 * @param   ethernet_buff: ethernet buffer
 * @param   domain_name:Domain name to be resolved
 * @param   domain_ip:Resolved Internet Protocol Address
 * @return  0:success;-1:failed
 */
int do_dns(uint8_t *buf, uint8_t *domain_name, uint8_t *domain_ip);

#endif
