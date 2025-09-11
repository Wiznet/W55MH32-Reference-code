#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "wiz_interface.h"
#include "udp_config.h"
#include "config.h"

CONFIG_MSG RecvMsg;
extern CONFIG_MSG system_cfg;
extern uint8_t reboot_flag;

/**
 * @brief   Run receive UpperComputer command and precess.
 * @param   sn: socket number
 * @return  none.
 */
void do_udp_config(uint8_t sn)
{
    uint16_t len = 0;
    uint8_t rip[4];
    uint16_t rport;
    uint16_t local_port = 1460;

    memset(RecvMsg.op, 0, sizeof(RecvMsg)); // clear RecvMsg

    switch (getSn_SR(sn))
    {
    case SOCK_UDP:
        if ((len = getSn_RX_RSR(sn)) > 0)
        {
            len = recvfrom(sn, (uint8_t *)&RecvMsg, len, rip, &rport);
            if (len > sizeof(CONFIG_MSG))
                break;
            {
                // FIND: searching, SETT: setting,
                if ((RecvMsg.op[0] == 'F') && (RecvMsg.op[1] == 'I') && (RecvMsg.op[2] == 'N') && (RecvMsg.op[3] == 'D'))
                {
                    printf("Find from %d.%d.%d.%d\r\n", rip[0], rip[1], rip[2], rip[3]);
                    memcpy(public_buff, &system_cfg, sizeof(CONFIG_MSG));
                    memcpy(public_buff, "FIND", 4);

                    sendto(sn, public_buff, sizeof(CONFIG_MSG), rip, rport); // return network info to uppercomputer.
                }
                else if ((RecvMsg.op[0] == 'S') && (RecvMsg.op[1] == 'E') && (RecvMsg.op[2] == 'T') && (RecvMsg.op[3] == 'T'))
                {
                    printf("Sett\r\n");

                    if ((RecvMsg.mac[0] == system_cfg.mac[0]) && (RecvMsg.mac[1] == system_cfg.mac[1]) && (RecvMsg.mac[2] == system_cfg.mac[2]) && (RecvMsg.mac[3] == system_cfg.mac[3]) && (system_cfg.mac[4] == system_cfg.mac[4]) && (RecvMsg.mac[5] == system_cfg.mac[5]))
                    {
                        memcpy(system_cfg.lip, RecvMsg.lip, 4);
                        memcpy(system_cfg.sub, RecvMsg.sub, 4);
                        memcpy(system_cfg.gw, RecvMsg.gw, 4);
                        memcpy(system_cfg.dns, RecvMsg.dns, 4);
                        system_config_param_save();
                        reboot_flag = 1;
                        sendto(sn, (uint8_t *)&RecvMsg, sizeof(CONFIG_MSG), rip, rport);
                    }
                }
                else if ((RecvMsg.op[0] == 'R') && (RecvMsg.op[1] == 'S') && (RecvMsg.op[2] == 'T') && (RecvMsg.op[3] == 'T'))
                {
                    printf("Reset\r\n");
                    reboot_flag = 1;
                }
                else if ((RecvMsg.op[0] == 'F') && (RecvMsg.op[1] == 'I') && (RecvMsg.op[2] == 'R') && (RecvMsg.op[3] == 'M'))
                {
                    printf("Fire\r\n");
                    if ((RecvMsg.mac[0] == system_cfg.mac[0]) && (RecvMsg.mac[1] == system_cfg.mac[1]) && (RecvMsg.mac[2] == system_cfg.mac[2]) && (RecvMsg.mac[3] == system_cfg.mac[3]) && (system_cfg.mac[4] == system_cfg.mac[4]) && (RecvMsg.mac[5] == system_cfg.mac[5]))
                    {
                        system_cfg.state = FW_UPDATE_CONFIG;
                        sendto(sn, (uint8_t *)&RecvMsg, sizeof(CONFIG_MSG), rip, rport);
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
