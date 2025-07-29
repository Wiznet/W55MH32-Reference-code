#include "can.h"
#include "delay.h"

// CAN initialization
// tsjw:resynchronize jump time units.range:CAN_SJW_1tq~ CAN_SJW_4tq
// tbs2:time unit of time period 2.   range:CAN_BS2_1tq~CAN_BS2_8tq;
// tbs1:time unit of time period 1.   range:CAN_BS1_1tq ~CAN_BS1_16tq
// brp :baud rate divider.range:1~1024;  tq=(brp)*tpclk1
// baud rate=Fpclk1/((tbs1+1+tbs2+1+1)*brp);
// mode:CAN_Mode_Normal,normal mode;CAN_Mode_LoopBack,loopback mode;
// the clock of fpclk1 is set to 36 m during initialization if set CAN_Mode_Init(CAN_SJW_1tq,CAN_BS2_8tq,CAN_BS1_9tq,4,CAN_Mode_LoopBack);
// the baud rate is:36M/((8+9+1)*4)=500Kbps
// return:0,OK;
//     other,Fail;
uint8_t CAN_Mode_Init(uint8_t tsjw, uint8_t tbs2, uint8_t tbs1, uint16_t brp, uint8_t mode)
{
    GPIO_InitTypeDef      GPIO_InitStructure;
    CAN_InitTypeDef       CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE
    NVIC_InitTypeDef NVIC_InitStructure;
#endif

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP; //reuse push pull
    GPIO_Init(GPIOB, &GPIO_InitStructure);           //initialize io

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //pull up input
    GPIO_Init(GPIOB, &GPIO_InitStructure);        //initialize io

    //CAN unit settings
    CAN_InitStructure.CAN_TTCM = DISABLE; //non time triggered communication mode
    CAN_InitStructure.CAN_ABOM = DISABLE; //software automatic offline management
    CAN_InitStructure.CAN_AWUM = DISABLE; //Sleep mode Wake up via software (clear the SLEEP bit of CAN- > MCR)
    CAN_InitStructure.CAN_NART = ENABLE;  //prohibit automatic transmission of messages
    CAN_InitStructure.CAN_RFLM = DISABLE; //message not locked new overwrite old
    CAN_InitStructure.CAN_TXFP = DISABLE; //priority is determined by the message identifier
    CAN_InitStructure.CAN_Mode = mode;    //Mode settings: mode: 0, normal mode; 1, loop mode;
    //set baud rate
    CAN_InitStructure.CAN_SJW       = tsjw;                                   //Resynchronize jump width (Tsjw) to tsjw + 1 time unit  	 CAN_SJW_2tq CAN_SJW_3tq CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1       = tbs1;                                   //Tbs1=tbs1+1 unit of timeCAN_BS1_1tq ~CAN_BS1_
    CAN_InitStructure.CAN_BS2       = tbs2;                                   //Tbs2=tbs2+1 unit of timeCAN_BS2_1tq ~	CAN_
    CAN_InitStructure.CAN_Prescaler = brp;                                    //The frequency division factor (Fdiv) is brp + 1
    CAN_Init(CAN1, &CAN_InitStructure);                                       //Initialize CAN1

    CAN_FilterInitStructure.CAN_FilterNumber         = 0;                     //filter0
    CAN_FilterInitStructure.CAN_FilterMode           = CAN_FilterMode_IdMask; //shielding mode
    CAN_FilterInitStructure.CAN_FilterScale          = CAN_FilterScale_32bit; //32 bit width
    CAN_FilterInitStructure.CAN_FilterIdHigh         = 0x0000;                //32 bit id
    CAN_FilterInitStructure.CAN_FilterIdLow          = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh     = 0x0000;                //32 bit mask
    CAN_FilterInitStructure.CAN_FilterMaskIdLow      = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;      //filter 0 is associated with fifo0
    CAN_FilterInitStructure.CAN_FilterActivation     = ENABLE;                //activate filter 0

    CAN_FilterInit(&CAN_FilterInitStructure);                                 //filter initialization

#if CAN_RX0_INT_ENABLE
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE); //FIFO0 message registration interruption allowed.

    NVIC_InitStructure.NVIC_IRQChannel                   = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // the primary priority is 1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0; // the secondary priority is 0
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
    return 0;
}

#if CAN_RX0_INT_ENABLE //Enable RX0 interrupt
//interrupt service function
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    int      i = 0;
    CAN_Receive(CAN1, 0, &RxMessage);
    for (i = 0; i < 8; i++)
        printf("rxbuf[%d]:%d\r\n", i, RxMessage.Data[i]);
}
#endif

//can send a set of data (fixed format: ID is 0X12, standard frame, data frame)
//len: data length (up to 8)
//Msg: Data pointer, up to 8 bytes.
//return value: 0, success;
//Other, failed.
uint8_t Can_Send_Msg(uint8_t *msg, uint8_t len)
{
    uint8_t  mbox;
    uint16_t i = 0;
    CanTxMsg TxMessage;
    TxMessage.StdId = 0x12;            // standard identifier
    TxMessage.ExtId = 0x12;            // set extension identifier
    TxMessage.IDE   = CAN_Id_Standard; // standard frame
    TxMessage.RTR   = CAN_RTR_Data;    // data frame
    TxMessage.DLC   = len;             // the length of the data to be sent
    for (i = 0; i < len; i++)
        TxMessage.Data[i] = msg[i];
    mbox = CAN_Transmit(CAN1, &TxMessage);
    i    = 0;
    while ((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i < 0XFFF)) i++; //waiting for the end of sending
    if (i >= 0XFFF) return 1;
    return 0;
}
//can receive data query
//buf:data cache area;
//return value:0,no data received;
//		 other,received data length;
uint8_t Can_Receive_Msg(u8 *buf)
{
    uint32_t i;
    CanRxMsg RxMessage;
    if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0) return 0; //no data received exit directly
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);               //read data
    for (i = 0; i < 8; i++)
        buf[i] = RxMessage.Data[i];
    return RxMessage.DLC;
}


