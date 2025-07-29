#include "cdc_usart.h"
#include "usb_lib.h"

void CDC_Send_Data(CDC_USART_t *usart, uint8_t is_in_callback);

CDC_USART_t CDC_USART[2] =
    {
        {
         USART1,
         {115200, 0x00, 0x00, 0x08},
         {0},
         {0},
         0,
         0,
         0,
         0,
         {CDC1_ENDP, CDC1_RX_DATA_SIZE, CDC1_RX_ENDP_ADDR},
         {CDC1_ENDP, CDC1_TX_DATA_SIZE, CDC1_TX_ENDP_ADDR},
         },
        {
         USART2,
         {115200, 0x00, 0x00, 0x08},
         {0},
         {0},
         0,
         0,
         0,
         0,
         {CDC2_ENDP, CDC2_RX_DATA_SIZE, CDC2_RX_ENDP_ADDR},
         {CDC2_ENDP, CDC2_TX_DATA_SIZE, CDC2_TX_ENDP_ADDR},
         },
};

void CDC_USART_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStruct);


    CDC_USART_Config(&CDC_USART[0]);
    CDC_USART_Config(&CDC_USART[1]);

    NVIC_InitStruct.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 1;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_Init(&NVIC_InitStruct);
}

void CDC_USART_Config(CDC_USART_t *usartx)
{
    USART_InitTypeDef USART_InitStructure;

    /* set the Stop bit*/
    switch (usartx->coding.format)
    {
    case 0:
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        break;
    case 1:
        USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
        break;
    case 2:
        USART_InitStructure.USART_StopBits = USART_StopBits_2;
        break;
    default:
        USART_InitStructure.USART_StopBits = USART_StopBits_1;
        break;
    }
    /* set the parity bit*/
    switch (usartx->coding.paritytype)
    {
    case 0:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    case 1:
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        break;
    case 2:
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
        break;
    default:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    }
    /* set the parity bit*/
    switch (usartx->coding.paritytype)
    {
    case 0:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    case 1:
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        break;
    case 2:
        USART_InitStructure.USART_Parity = USART_Parity_Odd;
        break;
    default:
        USART_InitStructure.USART_Parity = USART_Parity_No;
        break;
    }
    /*set the data type : only 8bits and 9bits is supported */
    switch (usartx->coding.datatype)
    {
    case 0x07:
        /* With this configuration a parity (Even or Odd) should be set */
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        break;
    case 0x08:
        if (USART_InitStructure.USART_Parity == USART_Parity_No)
        {
            USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        }
        else
        {
            USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        }
        break;
    default:
        USART_InitStructure.USART_WordLength = USART_WordLength_8b;
        break;
    }

    USART_InitStructure.USART_BaudRate            = usartx->coding.bitrate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(usartx->USARTx, &USART_InitStructure);

    USART_Cmd(usartx->USARTx, ENABLE);

    USART_ITConfig(usartx->USARTx, USART_IT_RXNE, ENABLE);
}

void USART_Callback(CDC_USART_t *usartx)
{
    if (USART_GetITStatus(usartx->USARTx, USART_IT_RXNE))
    {
        if (Buf_Write_Byte(&usartx->in_buf, USART_ReceiveData(usartx->USARTx)) == 2)
            CDC_Send_Data(usartx, 0);
    }
    if (USART_GetITStatus(usartx->USARTx, USART_IT_TXE))
    {
        USART_SendData(usartx->USARTx, usartx->out_buf[usartx->out_pos++]);
        if (usartx->out_pos >= usartx->out_len)
        {
            SetEPRxValid(usartx->out_endp.ENDP);
            USART_ITConfig(usartx->USARTx, USART_IT_TXE, DISABLE);
        }
    }
}

void USART1_IRQHandler(void)
{
    USART_Callback(&CDC_USART[CDC_USART1]);
}

void USART2_IRQHandler(void)
{
    USART_Callback(&CDC_USART[CDC_USART2]);
}

