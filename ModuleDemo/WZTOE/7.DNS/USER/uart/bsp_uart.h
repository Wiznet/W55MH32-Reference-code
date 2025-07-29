#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include <stdint.h>
/**
 * @brief   debug usart initialization
 * @param   none
 * @return  none
 */
void console_usart_init(uint32_t baudrate);
#endif
