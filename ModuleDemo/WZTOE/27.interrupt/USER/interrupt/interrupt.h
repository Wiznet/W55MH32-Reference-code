#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include "stdint.h"

/**
 * @brief   Initialize the INT pin
 * @param   none
 * @return  none
 */
void wizchip_int_init(void);

/**
 * @brief   Determine the interrupt type and store the value in I STATUS
 * @param   none
 * @return  none
 */
void wizchip_ISR(void);

/**
 * @brief   TCP server interrupt mode loopback
 * @param   none
 * @return  none
 */
void loopback_tcps_interrupt(uint8_t sn, uint8_t *buf, uint16_t port);

#endif
