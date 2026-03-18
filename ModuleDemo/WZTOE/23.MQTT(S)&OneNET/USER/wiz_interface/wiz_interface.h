#ifndef __WIZ_INTERFACE_H__
#define __WIZ_INTERFACE_H__

#include "wizchip_conf.h"
#include <stdint.h>

/**
 * @brief   print network information
 * @param   none
 * @return  none
 */
void print_network_information(void);

/**
 * @brief   set network information
 * @param   sn: socketid
 * @param   ethernet_buff:
 * @param   net_info: network information struct
 * @return  none
 */
void network_init(uint8_t *ethernet_buff, wiz_NetInfo *conf_info);

/**
 * @brief Ethernet Link Detection
 */
void wiz_phy_link_check(void);

/**
 * @brief Print PHY information
 */
void wiz_print_phy_info(void);

#endif
