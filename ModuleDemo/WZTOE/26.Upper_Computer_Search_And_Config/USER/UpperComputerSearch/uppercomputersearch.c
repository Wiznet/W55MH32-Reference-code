#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "wiz_interface.h"
#include "uppercomputersearch.h"

// Define two variables.
CONFIG_MSG ConfigMsg, RecvMsg;

// Define network inforamtion.
wiz_NetInfo netinfo = {
    0,
};

/**
 * @brief   Run receive UpperComputer command and precess.
 * @param   sn: socket number
 * @return  none.
 */
void do_udp_config(uint8_t sn)
{
    uint16_t i;
    uint16_t len = 0;
    uint8_t  rip[4];
    uint16_t rport;
    uint16_t local_port    = 1460;
    uint8_t  sw_version[2] = {1, 0};

    memset(RecvMsg.op, 0, sizeof(RecvMsg)); // clear RecvMsg

    switch (getSn_SR(sn))
    {
    case SOCK_UDP:
        if ((len = getSn_RX_RSR(sn)) > 0)
        {
            len = recvfrom(sn, (uint8_t *)&RecvMsg, len, rip, &rport);
            if (len > sizeof(ConfigMsg))
                break;
            {
                // FIND: searching, SETT: setting,
                if ((RecvMsg.op[0] == 'F') && (RecvMsg.op[1] == 'I') && (RecvMsg.op[2] == 'N') && (RecvMsg.op[3] == 'D'))
                {
                    wizchip_getnetinfo(&netinfo); // get config network infomation.

                    memcpy(ConfigMsg.mac, netinfo.mac, 6);
                    memcpy(ConfigMsg.lip, netinfo.ip, 4);
                    memcpy(ConfigMsg.gw, netinfo.gw, 4);
                    memcpy(ConfigMsg.dns, netinfo.dns, 4);
                    memcpy(ConfigMsg.sub, netinfo.sn, 4);
                    memcpy(ConfigMsg.sw_ver, sw_version, 2);

                    RecvMsg = ConfigMsg;
                    memcpy(RecvMsg.op, "FIND", 4);
                    sendto(sn, (uint8_t *)&RecvMsg, sizeof(RecvMsg), rip, rport); // return network info to uppercomputer.
                    printf("Find\r\n");
                }
                else if ((RecvMsg.op[0] == 'S') && (RecvMsg.op[1] == 'E') && (RecvMsg.op[2] == 'T') && (RecvMsg.op[3] == 'T'))
                {
                    printf("Sett\r\n");
                    if ((RecvMsg.mac[0] == ConfigMsg.mac[0]) && (RecvMsg.mac[1] == ConfigMsg.mac[1]) && (RecvMsg.mac[2] == ConfigMsg.mac[2]) && (RecvMsg.mac[3] == ConfigMsg.mac[3]) && (RecvMsg.mac[4] == ConfigMsg.mac[4]) && (RecvMsg.mac[5] == ConfigMsg.mac[5]))
                    {
                        for (i = 0; i < 4; i++) // recv uppercomputer config network info
                        {
                            memcpy(netinfo.ip, RecvMsg.lip, 4);
                            memcpy(netinfo.sn, RecvMsg.sub, 4);
                            memcpy(netinfo.gw, RecvMsg.gw, 4);
                            memcpy(netinfo.dns, RecvMsg.dns, 4);
                            netinfo.dhcp = NETINFO_STATIC;
                        }
                        wizchip_setnetinfo(&netinfo); // write chip
                        printf("From the UpperComputer Config network information :\r\n");
                        print_network_information();  // readback and print
                    }
                }
            }
        }

        break;
    case SOCK_CLOSED:
        socket(sn, Sn_MR_UDP, local_port, 0x00);
        break;
    }
}
