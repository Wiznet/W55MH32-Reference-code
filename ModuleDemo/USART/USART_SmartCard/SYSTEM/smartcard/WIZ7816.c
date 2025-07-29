#include "WIZ7816.h"
#include "string.h"
#include "delay.h"


/* Reset pin */
#define WIZ_RST_H GPIO_SetBits(WIZ_RST_GPIOx, WIZ_RST_Pinx)
#define WIZ_RST_L GPIO_ResetBits(WIZ_RST_GPIOx, WIZ_RST_Pinx)


uint8_t WIZ_ATR[40];          //Store an ATR.
uint8_t ATR_TA1 = 0x00;       //The value of TA1 in the card ATR, TA1 contains FD
uint8_t WIZ_T1  = 0;          //Is the card T = 1?

uint32_t WIZ_WT = 9600;       //Communication Timeout Time WT

uint8_t  WIZ_F     = 1;       //F
uint8_t  WIZ_D     = 1;       //D
uint32_t WIZ_ClkHz = 4000000; //Frequency 4MHz

uint16_t WIZ_DelayMS = 0;     //timeout count

                              //FD table
static const uint16_t F_Table[16] = {372, 372, 558, 744, 1116, 1488, 1860, 372, 372, 512, 768, 1024, 1536, 2048, 372, 372};
static const uint8_t  D_Table[16] = {1, 1, 2, 4, 8, 16, 32, 64, 12, 20, 1, 1, 1, 1, 1, 1};

void TIM_ON(void)
{
    TIM6->CNT = 0;
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    TIM_Cmd(TIM6, ENABLE);
}
/*
Function: WIZ7816 port initialization
Parameters: None
Return: none
*/
void WIZ7816_Init(void)
{
    GPIO_InitTypeDef       GPIO_InitStructure;
    USART_InitTypeDef      USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStructure;

    WIZ_RCC_APBxPeriphClockCmd;

    GPIO_InitStructure.GPIO_Pin   = WIZ_VCC_Pinx; //Power output
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WIZ_VCC_GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = WIZ_RST_Pinx; //Reset output
    GPIO_Init(WIZ_RST_GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = WIZ_5V3VN_Pinx; //5V3VN
    GPIO_Init(WIZ_5V3VN_GPIOx, &GPIO_InitStructure);

    GPIO_ResetBits(WIZ_5V3VN_GPIOx, WIZ_5V3VN_Pinx); //Power supply is switched to 3.3V
    GPIO_SetBits(WIZ_5V3VN_GPIOx, WIZ_5V3VN_Pinx);   //The power supply is switched to 5V.
    WIZ7816_SetVCC(0);                               //power failure

    GPIO_InitStructure.GPIO_Pin   = WIZ_CLK_Pinx;    //CLK multiplexing
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WIZ_CLK_GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = WIZ_IO_Pinx; //IO multiplexing
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(WIZ_IO_GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = WIZ_OFFN_Pinx; //OFFN
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(WIZ_OFFN_GPIOx, &GPIO_InitStructure);

    USART_SetGuardTime(WIZ_USARTx, 12); //protection time

    USART_ClockInitStructure.USART_Clock   = USART_Clock_Enable;
    USART_ClockInitStructure.USART_CPOL    = USART_CPOL_Low;
    USART_ClockInitStructure.USART_CPHA    = USART_CPHA_1Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
    USART_ClockInit(WIZ_USARTx, &USART_ClockInitStructure);

    WIZ7816_SetClkHz(WIZ_ClkHz); //Set the CLK frequency

    USART_InitStructure.USART_BaudRate            = 10752;
    USART_InitStructure.USART_WordLength          = USART_WordLength_9b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1_5;
    USART_InitStructure.USART_Parity              = USART_Parity_Even;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(WIZ_USARTx, &USART_InitStructure);

    WIZ7816_SetFD(WIZ_F, WIZ_D); //Set FD

    USART_Cmd(WIZ_USARTx, ENABLE);
    USART_SmartCardNACKCmd(WIZ_USARTx, ENABLE);
    USART_SmartCardCmd(WIZ_USARTx, ENABLE);
}

/*
Function: Timeout count timing interrupt (1MS interrupt)
Parameters: None
Return: none
*/
void WIZ7816_TIMxInt(void)
{
    if (WIZ_DelayMS > 0) //Count time>0
    {
        WIZ_DelayMS--;   //
    }
}

/*
Function: WIZ7816 port to set the frequency
Parameter: Frequency, = 0 is off clock
Return: none
*/
void WIZ7816_SetClkHz(uint32_t hz)
{
    uint32_t          apbclock   = 0x00;
    uint32_t          usartxbase = 0;
    RCC_ClocksTypeDef RCC_ClocksStatus;

    if (hz == 0)
    {
        WIZ_USARTx->CR2 &= ~0x00000800; //Off clock
        WIZ_USARTx->CR1 &= ~0x0000000C;
        return;
    }

    usartxbase = (uint32_t)WIZ_USARTx;
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    if (usartxbase == USART1_BASE)
    {
        apbclock = RCC_ClocksStatus.PCLK2_Frequency;
    }
    else
    {
        apbclock = RCC_ClocksStatus.PCLK1_Frequency;
    }

    apbclock /= hz; //Calculate the frequency division ratio according to the frequency of the serial port
    apbclock /= 2;
    if (apbclock < 1) apbclock = 1;

    USART_SetPrescaler(WIZ_USARTx, apbclock); //Set frequency division
    WIZ_USARTx->CR2 |= 0x00000800;            //Turn on the clock
    WIZ_USARTx->CR1 |= 0x0000000C;
}


/*
Function: WIZ7816 port setting FD
Parameter: Table index value of FD
Return: none
*/
void WIZ7816_SetFD(uint8_t F, uint8_t D)
{
    uint32_t etudiv;

    etudiv = WIZ_USARTx->GTPR & 0x0000001F;        //Get the clock division number
    etudiv = 2 * etudiv * F_Table[F] / D_Table[D]; //Porter ratio = ((clock division)*2)*F)/D

    WIZ_USARTx->BRR = etudiv;
}

/*
Function: WIZ7816 interface to set communication timeout time
Parameter: Timeout time (in ETU)
Return: none
*/
void WIZ7816_SetWT(uint32_t wt)
{
    WIZ_WT = wt;
}

/*
Function: Serial port receives one byte of data
Parameters: Data, Timeout (in MS)
Returns: 1 timeout error, 0 success
*/
static uint8_t USART_RecvByte(uint8_t *dat, uint16_t overMs)
{
    WIZ_DelayMS = overMs + 1; //Set timeout time, + 1 to avoid 1ms bias
    TIM_ON();
    while (WIZ_DelayMS)       //within time
    {
        //Received data
        if (RESET != USART_GetFlagStatus(WIZ_USARTx, USART_FLAG_RXNE))
        {
            *dat = (uint8_t)USART_ReceiveData(WIZ_USARTx);
            break;
        }
    }
    overMs      = WIZ_DelayMS == 0; //timeout
    WIZ_DelayMS = 0;

    return overMs; //return
}

/*
Function: Serial port sends a byte of data
Parameter: Data
Return: none
*/
static void USART_SendByte(uint8_t dat)
{
    USART_ClearFlag(WIZ_USARTx, USART_FLAG_TC); //clear send identifier
    USART_SendData(WIZ_USARTx, dat);            //send
    while (USART_GetFlagStatus(WIZ_USARTx, USART_FLAG_TC) == RESET);
    //while(USART_GetFlagStatus(WIZ_USARTx,USART_FLAG_RXNE)==RESET);            //When you don't have a card, you will keep waiting.
    (void)USART_ReceiveData(WIZ_USARTx);
}
/*
Functions: CLK to MS conversion, internal functions
Parameters: CLK, 1 in/0 rounded
Return: The converted MS value
*/
static uint16_t CLKToMS(uint32_t clk, uint8_t half)
{
    uint16_t temp;

    temp = clk / (WIZ_ClkHz / 1000);
    clk  = clk % (WIZ_ClkHz / 1000);

    if (half && clk)
    {
        temp += 1;
    }
    return temp;
}
/*
Functions: CLK to US conversion, internal functions
Parameters: CLK, 1 in/0 rounded
Returns: US value after conversion
*/
static uint16_t CLKToUS(uint32_t clk, uint8_t half)
{
    uint16_t temp;

    clk  *= 1000;
    temp  = clk / (WIZ_ClkHz / 1000);
    clk   = clk % (WIZ_ClkHz / 1000);

    if (half && clk)
    {
        temp += 1;
    }
    return temp;
}
/*
Functions: ETU to MS conversion, internal functions
Parameters: ETU, 1 in/0 rounded
Return: The converted MS value
*/
static uint16_t ETUToMS(uint32_t etu, uint8_t half)
{
    uint16_t temp;

    etu  *= (F_Table[WIZ_F] / D_Table[WIZ_D]);
    temp  = etu / (WIZ_ClkHz / 1000);
    etu   = etu % (WIZ_ClkHz / 1000);

    if (half && etu)
    {
        temp += 1;
    }
    return temp;
}

/*
Function: Calculate the number of 1 (binary) in a number, internal function
Parameter: Data
Returns the number of 1
*/
static uint8_t NumberOf1_Solution1(uint32_t num)
{
    uint8_t count = 0;
    while (num)
    {
        if (num & 0x01)
        {
            count++;
        }
        num = num >> 1;
    }
    return count;
}

/*
Function: Predict the true length of the ATR, internal function
Parameters: Known ATR data and length
Returns the true length of the ATR
*/
uint8_t foreATRLen(uint8_t *atr, uint8_t len)
{
    uint8_t len1 = 2, len2 = 2, next = 1, temp = 1, TD2;

    WIZ_T1  = 0;    //default T=0
    ATR_TA1 = 0;    //TA1 clear
    if (len < len2) //The amount of data is too small
    {
        return 0xFF;
    }
    while (next)
    {
        next = 0;
        if ((atr[len2 - 1] & 0x80) == 0x80) //TDi exists
        {
            next = 1;                       //Enter the next cycle
            len1++;
        }
        if ((atr[len2 - 1] & 0x40) == 0x40) //TCi exists
        {
            len1++;
        }
        if ((atr[len2 - 1] & 0x20) == 0x20) //TBi exists
        {
            len1++;
        }
        if ((atr[len2 - 1] & 0x10) == 0x10) //TAI exists
        {
            len1++;

            if (len2 == 2)        //TA1 exists
            {
                ATR_TA1 = atr[2]; //Record TA1
            }
        }
        len2 = len1;
        if (len < len2) //The amount of data is too small
        {
            return 0xFF;
        }
    }
    len1 += (atr[1] & 0x0F);                        //Add history byte length

    if (atr[1] & 0x80)                              //TD1 exists
    {
        temp += NumberOf1_Solution1(atr[1] & 0xF0); //Find the index of TD1 by judging the number of digits of 1
        if ((atr[temp] & 0x0F) == 0x01)             //TD1 low is 1, then T = 1 card
        {
            WIZ_T1 = 1;
        }
        if (atr[temp] & 0x80)                                //Determine whether TD2 exists
        {
            WIZ_T1  = 1;                                     //Existence must be T = 1
            temp   += NumberOf1_Solution1(atr[temp] & 0xF0); //Find the index of TD2 by judging the number of digits of 1
            TD2     = temp;
            if ((atr[TD2] & 0x30) == 0x30)
            {
                WIZ_T1 = 1;
            }
            else if ((atr[TD2] & 0xF0) == 0x00 && (atr[TD2] & 0x0F) == 0x01)
            {
                WIZ_T1 = 1;
            }
        }
    }
    if (WIZ_T1) //T = 1ATR has a 1-byte check at the end
    {
        len1++;
    }
    return len1; //Returns the predicted ATR length value
}

/*
Function: Thermal Reset
Parameters: ATR, ATR length
Returns: 1 timeout, 0 success
*/
uint8_t WarmReset(uint8_t *atr, uint16_t *len)
{
    uint8_t  i, err;
    uint16_t overTim;

    USART_RecvByte(&i, 1); //Clear away useless data

    WIZ_WT = 9600;         //Restore initial value
    WIZ_F  = 1;
    WIZ_D  = 1;
    *len   = 0;

    WIZ7816_SetFD(WIZ_F, WIZ_D);          //recovery baud rate

    WIZ_RST_L;                            //Reset foot pull low
    overTim = CLKToUS(500, 1);            //CLK to US, further guarantee greater than
    delay_us(overTim);                    //Wait for at least 400 clocks

    WIZ_RST_H;                            //Reset foot lift
    overTim = CLKToUS(300, 0);            //CLK to US, no further guarantee less than
    delay_us(overTim);                    //Wait up to 400 clocks

    for (i = 0; i < sizeof(WIZ_ATR); i++) //Receive ATR Cyclic
    {
        if (i == 0)
        {
            overTim = CLKToMS(40000, 1); //The return time of the first byte is within 400-40,000 clocks
        }
        else
        {
            overTim = ETUToMS(WIZ_WT * D_Table[WIZ_D], 1); //The remaining bytes are timed out
        }
        err = USART_RecvByte(WIZ_ATR + i, overTim);        //Receive a byte within the timeout
        if (!err)                                          //received
        {
            uint8_t atrLen;

            (*len)++;
            atrLen = foreATRLen(WIZ_ATR, *len);
            if (atrLen == *len)
            {
                break;
            }
        }
        else //timeout
        {
            err = i == 0;
            break;
        }
    }
    memcpy(atr, WIZ_ATR, *len); //Copy ATR data


    return err;
}

/*
Function: Set IO port status, internal functions
Parameters: 1 on/0 off
Return: none
*/
static void setIOState(uint8_t on)
{
    //    GPIO_InitTypeDef GPIO_InitStructure;

    //    if(on)
    //    {
    //        GPIO_InitStructure.GPIO_Pin = WIZ_IO_Pinx;
    //        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    //        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //        GPIO_Init(WIZ_IO_GPIOx, &GPIO_InitStructure);
    //    }
    //    else
    //    {
    //        GPIO_InitStructure.GPIO_Pin = WIZ_IO_Pinx;
    //        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    //        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    //        GPIO_Init(WIZ_IO_GPIOx, &GPIO_InitStructure);

    //        GPIO_ResetBits(WIZ_IO_GPIOx, WIZ_IO_Pinx);
    //    }
}

/*
Function: Set up VCC
Parameters: 1 on/0 off
Return: none
*/
void WIZ7816_SetVCC(uint8_t off)
{
    if (!off)
    {
        GPIO_SetBits(WIZ_VCC_GPIOx, WIZ_VCC_Pinx);
    }
    else
    {
        GPIO_ResetBits(WIZ_VCC_GPIOx, WIZ_VCC_Pinx);
    }
}

/*
Function: cold reset
Parameters: ATR, ATR length
Returns: 1 timeout, 0 success
*/
uint8_t ColdReset(uint8_t *atr, uint16_t *len)
{
    uint8_t  i, err;
    uint16_t overTim;

    USART_RecvByte(&i, 1); //Clear away useless data

    WIZ_WT = 9600;         //Restore initial value
    WIZ_F  = 1;
    WIZ_D  = 1;
    *len   = 0;

    WIZ7816_SetFD(WIZ_F, WIZ_D); //recovery baud rate

    WIZ7816_SetClkHz(0);
    setIOState(0);
    WIZ_RST_L;

    WIZ7816_SetVCC(0);           //power down
    delay_ms(50);                //latency

    WIZ7816_SetVCC(1);           //power up

    WIZ7816_SetClkHz(WIZ_ClkHz); //Give the clock
    delay_ms(50);                //delay

    setIOState(1);               //IO is raised within 200 clocks, here it is directly raised after giving the clock
    USART_RecvByte(&i, 1);       //Clear away useless data

    overTim = CLKToUS(500, 1);   //CLK to US, further guarantee greater than
    delay_us(overTim);           //Wait for at least 400 clocks
    WIZ_RST_H;                   //Reset foot lift
    overTim = CLKToUS(300, 0);   //CLK to US, no further guarantee less than
    delay_us(overTim);

    for (i = 0; i < sizeof(WIZ_ATR); i++) //Receive ATR Cyclic
    {
        if (i == 0)
        {
            overTim = CLKToMS(40000, 1); //The return time of the first byte is within 400-40,000 clocks
        }
        else
        {
            overTim = ETUToMS(WIZ_WT * D_Table[WIZ_D], 1); //The remaining bytes are timed out
        }
        err = USART_RecvByte(WIZ_ATR + i, overTim);        //Receive a byte within the timeout
        if (!err)                                          //Received
        {
            uint8_t atrLen;

            (*len)++;
            atrLen = foreATRLen(WIZ_ATR, *len);
            if (atrLen == *len)
            {
                break;
            }
        }
        else //timeout
        {
            err = i == 0;
            break;
        }
    }
    memcpy(atr, WIZ_ATR, *len); //Copy ATR data


    return err;
}

/*
Function: Send APDU and receive return data
Parameters: send data, length, receive data, length
Returns: 0 success, 1 timeout, 2 APDU format error, 3 communication error
*/
uint8_t ExchangeTPDU(uint8_t *sData, uint16_t len_sData, uint8_t *rData, uint16_t *len_rData)
{
    uint8_t err, wait, recvFlag;
    uint8_t i, lc, le;

    uint8_t pc;
    uint8_t INS = sData[1]; //Record the INS to prevent it from being overwritten when using it after receiving the data.

    uint16_t overTim;
    overTim = ETUToMS(WIZ_WT * D_Table[WIZ_D], 1); //timeout

    if (len_sData == 4)                            //Length 4, CASE 1
    {
        sData[4] = 0x00;
        lc = le = 0;
    }
    else if (len_sData == 5) //Length 5, CASE 2
    {
        lc = 0;
        le = sData[4];
    }
    else
    {
        if (len_sData == sData[4] + 5) //Lc+5,CASE3
        {
            lc = sData[4];
            le = 0;
        }
        else if (len_sData == sData[4] + 6) //Lc+5+1(Le),CASE4
        {
            lc = sData[4];
            le = sData[len_sData - 1];
        }
        else
        {
            return 2; //Unable to parse APDU
        }
    }

    USART_RecvByte(&i, 1);  //Clear away useless data
    for (i = 0; i < 5; i++) //Send 5 APDU headers
    {
        USART_SendByte(sData[i]);
    }

    wait       = 1;
    len_sData  = 0;
    *len_rData = 0;
    recvFlag   = 0;
    while (wait) //You need to wait.
    {
        wait = 0;
        err  = USART_RecvByte(&pc, overTim); //Receive one byte of data
        if (err)
        {
            return err; //error
        }
        else
        {
            if ((pc >= 0x90 && pc <= 0x9F) || (pc >= 0x60 && pc <= 0x6F)) //Between 90-9F/60-6F
            {
                switch (pc)
                {
                case 0x60: //Keep waiting.
                    wait = 1;
                    break;
                default:                    //Status word SW
                    rData[*len_rData] = pc; //SW1
                    (*len_rData)++;
                    USART_RecvByte(&pc, overTim);
                    rData[*len_rData] = pc; //SW2
                    (*len_rData)++;
                    break;
                }
            }
            else                                             //ACK
            {
                pc ^= INS;                                   //Process word Exclusive or INS
                if (pc == 0)                                 //Return value = INS, indicating that all data to be sent should be sent next, or ready to receive all data
                {
                    if (recvFlag == 0 && lc > len_sData)     //Sent status and there is data to send
                    {
                        for (i = 0; i < lc - len_sData; i++) //Send all the data to be sent
                        {
                            USART_SendByte(sData[i + 5 + len_sData]);
                        }
                        len_sData = lc;
                        recvFlag  = 1;                                 //receiving status
                    }
                    if ((recvFlag == 1 || lc == 0) && le > *len_rData) //(Received status or no data to send) and there is data to receive
                    {
                        for (i = 0; i < le - *len_rData; i++)
                        {
                            err = USART_RecvByte(rData + *len_rData + i, overTim);
                            if (err && i < le - *len_rData)
                            {
                                *len_rData = i;
                                return err; //error
                            }
                        }
                        *len_rData = le;
                    }
                    wait = 1;
                }
                else if (pc == 0xFF)                          //Return value =~ INS, identifying the next byte of data to be sent, or ready to receive 1 byte of data
                {
                    if (recvFlag == 0 && lc > len_sData)      //Sent status and there is data to send
                    {
                        USART_SendByte(sData[5 + len_sData]); //Send the next byte of data
                        len_sData++;
                        if (len_sData == lc)                  //Sent complete
                        {
                            recvFlag = 1;                     //receiving status
                        }
                    }
                    if ((recvFlag == 1 || lc == 0) && le > *len_rData)     //(Received status or no data to send) and there is data to receive
                    {
                        err = USART_RecvByte(rData + *len_rData, overTim); //Receive one byte of data
                        if (err)
                        {
                            break;
                        }
                        (*len_rData)++; //Receive Length Accumulation
                    }
                    wait = 1;
                }
                else //other
                {
                    return 3;
                }
            }
        }
    }
    return 0;
}

/*
Function: PPS
Parameters: Table index values of F, D
Returns: 0 success, 1 failure
*/
uint8_t PPS(uint8_t F, uint8_t D)
{
    uint8_t  i, err;
    uint16_t overTim;
    uint8_t  pps_cmd[4] = {0xFF, 0x10, 0xFD, 0x00};
    uint8_t  pps_res[4] = {0x00, 0x00, 0x00, 0x00};

    overTim = ETUToMS(WIZ_WT * D_Table[WIZ_D], 1); //timeout

    pps_cmd[2] = ((F << 4) & 0xF0) | (D & 0x0F);   //Assignment FD
    for (i = 0; i < 3; i++)
    {
        pps_cmd[3] ^= pps_cmd[i]; //XOR
    }

    for (i = 0; i < 4; i++) //Send PPS
    {
        USART_SendByte(pps_cmd[i]);
    }

    for (i = 0; i < 4; i++)
    {
        err = USART_RecvByte(&pps_res[i], overTim);
        if (err)
        {
            break; //error
        }
    }
    if (i == 4) //By agreement, four bytes should be returned
    {
        if (pps_res[0] == 0xFF && (pps_res[1] & 0x10) == 0x10 && pps_res[2] == pps_cmd[2])
        {
            WIZ7816_SetFD(F, D);
            return 0;
        }
    }
    else //Not according to the agreement
    {
        //
        //if(((pps_res[0]&0x10)==0x10&&pps_res[1]==pps_cmd[2]))
        //{
        //WIZ7816_SetFD(F,D);
        //return 0;
        //}
    }

    return 1;
}


