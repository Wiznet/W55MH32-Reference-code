#include "wol.h"
#include <stdio.h>
#include "socket.h"

/**
 * @brief  Wake on Lan run function
 * @param  sn: Socket number
 * @param  buf: ethernet buffer pointer
 * @param  local_port: Local port
 * @return none
 **/
void wol_run(uint8_t sn, uint8_t *buf, uint16_t local_port)
{
    switch (getSn_SR(sn))
    {
    case SOCK_UDP:
        if (getSn_IR(sn) & Sn_IR_RECV)
        {
            setSn_IR(sn, Sn_IR_RECV);
        }
        if (getIR() & IR_MP) // Received WOL Magic packet based on UDP.
        {
            printf("get wake on line\r\n");
            setIR(0x10);
        }
        break;
    case SOCK_CLOSED:
        setIMR(IM_IR4); // Open WOL magic pack interrupt
        setMR(MR_WOL);  // Receive WOL packets

        socket(sn, Sn_MR_UDP, local_port, 0);
        printf("%d:Opened, UDP WOL, port [%d]\r\n", sn, local_port);
        break;
    }
}
