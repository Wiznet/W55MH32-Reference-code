#ifndef __CDC_USART_H_
#define __CDC_USART_H_


#include "stdint.h"
#include "hw_config.h"
#include "ring_buf.h"

enum
{
    CDC_USART1 = 0,
    CDC_USART2,
};

typedef struct
{
    uint32_t bitrate;
    uint8_t  format;
    uint8_t  paritytype;
    uint8_t  datatype;
} LINE_CODING;

typedef struct
{
    uint8_t  ENDP;
    uint8_t  max_data_size;
    uint16_t data_addr;
} ENDP_t;

typedef struct
{
    USART_TypeDef *USARTx;
    LINE_CODING    coding;
    ring_buf_t     in_buf;
    uint8_t        out_buf[0x40];
    uint8_t        out_len;
    uint8_t        out_pos;
    uint8_t        in_len;
    uint8_t        in_busy;
    ENDP_t         out_endp;
    ENDP_t         in_endp;
} CDC_USART_t;


void CDC_USART_Init(void);
void CDC_USART_Config(CDC_USART_t *usartx);

extern CDC_USART_t CDC_USART[2];

#endif
