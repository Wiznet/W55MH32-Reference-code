#include "delay.h"
#include "w55mh32.h"
#include "rs485.h"

#ifdef EN_USART3_RX

//receive buffer
uint8_t RS485_RX_BUF[64]; //Receive buffer, maximum 64 bytes.
//Length of data received
uint8_t RS485_RX_CNT = 0;

void USART3_IRQHandler(void)
{
    uint8_t res;
    if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //Received data
    {
        res = USART_ReceiveData(USART3);                   //Read the received data
        if (RS485_RX_CNT < 64)
        {
            RS485_RX_BUF[RS485_RX_CNT] = res; //Record the received value
            RS485_RX_CNT++;                   //Received data is increased by 1.
        }
    }
}
#endif

//Initialize IO serial port 2
//pclk1: PCLK1 clock frequency (MHz)
//bound: baud rate
void RS485_Init(uint32_t bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef  NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE);

    USART_InitStructure.USART_BaudRate            = bound;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART3, &USART_InitStructure);
    ;

    NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART3, ENABLE);

    RS485_TX_EN = 0;
}

//RS485 sends len bytes.
//buf: sending address
//len: the number of bytes sent (in order to match the reception of this code, it is recommended not to exceed 64 bytes)
void RS485_Send_Data(uint8_t *buf, uint8_t len)
{
    uint8_t t;
    RS485_TX_EN = 1;          //Set to send mode
    for (t = 0; t < len; t++) //loop data
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        USART_SendData(USART3, buf[t]);
    }

    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    RS485_RX_CNT = 0;
    RS485_TX_EN  = 0; //Set to receive mode
}
//RS485 queries the received data
//buf: receive cache first address
//len: length of data read
void RS485_Receive_Data(uint8_t *buf, uint8_t *len)
{
    uint8_t rxlen = RS485_RX_CNT;
    uint8_t i     = 0;
    *len          = 0;                  //Default is 0
    delay_ms(10);                       //Wait for 10ms, if no data is received for more than 10ms in a row, the reception is considered to be over.
    if (rxlen == RS485_RX_CNT && rxlen) //The data has been received and the reception is complete
    {
        for (i = 0; i < rxlen; i++)
        {
            buf[i] = RS485_RX_BUF[i];
        }
        *len         = RS485_RX_CNT; //Record the length of this data
        RS485_RX_CNT = 0;            //clear
    }
}

