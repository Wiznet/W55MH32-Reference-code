#ifndef __UPPERCOMPUTERSEARCH_H_
#define __UPPERCOMPUTERSEARCH_H_

#include <stdint.h>

// Define WIZnet chip main configuration of parameters.
#pragma pack(push, 4)

typedef struct _CONFIG_MSG
{
    uint8_t op[4];
    uint8_t mac[6];
    uint8_t sw_ver[2];
    uint8_t lip[4];
    uint8_t sub[4];
    uint8_t gw[4];
    uint8_t dns[4];

    uint8_t  dhcp;
    uint8_t  debug;
    uint16_t fw_len;
    uint8_t  state;
} CONFIG_MSG;

#pragma pack(pop)

/**
 * @brief   Run receive UpperComputer command and precess.
 * @param   sn: socket number
 * @return  none.
 */
void do_udp_config(uint8_t sn);

#endif
