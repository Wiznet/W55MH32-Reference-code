#ifndef _WOL_H
#define _WOL_H
#include <stdint.h>

/**
 * @brief  Wake on Lan run function
 * @param  sn: Socket number
 * @param  buf: ethernet buffer pointer
 * @param  local_port: Local port
 * @return none
 **/
void wol_run(uint8_t sn, uint8_t *buf, uint16_t local_port);
#endif
