#include "aht20.h"
#include "delay.h"

MYI2C_Struct SENx;


/**********************************************
//MYI2C_Delay_us
**********************************************/
void MYI2C_Delay_us(unsigned long nTim)
{
    unsigned int i;

    while (nTim--)
    {
        i = MYI2C_delay_us_cnt;
        while (i--);
    }
}
/*******************************************************************************
* Function Name  : MYI2C_GPIO_MODE
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MYI2C_GPIO_MODE(unsigned char TYP) //根据MCU修改相应的IO初始化
{
#ifdef ARM32
    GPIO_InitTypeDef GPIO_InitStruct;
#endif
    switch (TYP)
    {
    case SDA_OUT: //设置开漏输出,需要加外部上拉电阻
#ifdef ARM32
        GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_OD;
        GPIO_InitStruct.GPIO_Pin   = SDA_Pin;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
#else
        P0M0 |= 1 << 3;
        P0M1 |= 3 << 2;
#endif
        break;
    case SDA_IN: //设置输入,需要加外部上拉电阻
#ifdef ARM32
        GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IPU;
        GPIO_InitStruct.GPIO_Pin   = SDA_Pin;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(IIC_SDA_PORT, &GPIO_InitStruct);
#else
        P0M0 &= ~(1 << 3);
        P0M1 |= 3 << 2;
#endif
        break;
    case SCL_OUT: //设置开漏输出,需要加外部上拉电阻
#ifdef ARM32
        GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_OD;
        GPIO_InitStruct.GPIO_Pin   = SCL_Pin;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(IIC_SCL_PORT, &GPIO_InitStruct);
#else
        P0M0 |= 1 << 2;
        P0M1 |= 3 << 2;
#endif
        break;
    }
}
/*******************************************************************************
* Function Name  : MYI2C_GPIO_DATA
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char MYI2C_GPIO_DATA(unsigned char TYP) //根据MCU修改相应的IO操作
{
    unsigned int dat = 0;

    switch (TYP)
    {
    case SDA_H:
#ifdef ARM32
        GPIO_SetBits(IIC_SDA_PORT, SDA_Pin);
#else
        I2C_SDA_PIN = 1;
#endif
        break;
    case SDA_L:
#ifdef ARM32
        GPIO_ResetBits(IIC_SDA_PORT, SDA_Pin);
#else
        I2C_SDA_PIN = 0;
#endif
        break;
    case SCL_H:
#ifdef ARM32
        GPIO_SetBits(IIC_SCL_PORT, SCL_Pin);
#else
        I2C_SCL_PIN = 1;
#endif
        break;
    case SCL_L:
#ifdef ARM32
        GPIO_ResetBits(IIC_SCL_PORT, SCL_Pin);
#else
        I2C_SCL_PIN = 0;
#endif
        break;
    case SDA_R:
#ifdef ARM32
        dat = GPIO_ReadInputDataBit(IIC_SDA_PORT, SDA_Pin);
#else
        dat = I2C_SDA_PIN;
#endif
        break;
    }

    return dat;
}
/**********************************************
//IIC Start
**********************************************/
void MYI2C_IIC_Start(void)
{
    MYI2C_SCK_Set();
    MYI2C_SDA_Set();
    MYI2C_SDA_Clr();
    MYI2C_SCK_Clr();
}
/**********************************************
//IIC Stop
**********************************************/
void MYI2C_IIC_Stop(void)
{
    MYI2C_SCK_Clr();
    MYI2C_SDA_Clr();
    MYI2C_SCK_Set();
    MYI2C_SDA_Set();
}
/**********************************************
//IIC Ack
**********************************************/
void MYI2C_IIC_Ack(unsigned char ack)
{
    MYI2C_SCK_Clr();
    if (ack)
    {
        MYI2C_SDA_Clr();
    }
    else
    {
        MYI2C_SDA_Set();
    }
    MYI2C_SCK_Set();
}
/**********************************************
//IIC Wait_Ack
**********************************************/
unsigned char MYI2C_IIC_Wait_Ack(unsigned int wait_time)
{
    MYI2C_SCK_Clr();
    MYI2C_SDA_IN_Mode;
    MYI2C_SDA_Set();
    MYI2C_SCK_Set();
    while (wait_time)
    {
        if (MYI2C_GPIO_DATA(SDA_R) == 0) break;
        MYI2C_Delay_us(1);
        wait_time--; //=======================
    }

    MYI2C_SDA_OD_Mode;
    return wait_time; //是否应答=======================
}
/**********************************************
// IIC Write byte
**********************************************/
void MYI2C_Write_IIC_Byte(unsigned char dat)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        MYI2C_SCK_Clr();
        if (dat & 0x80)
        {
            MYI2C_SDA_Set();
        }
        else
        {
            MYI2C_SDA_Clr();
        }
        MYI2C_SCK_Set();
        dat = dat << 1;
    }
}
/**********************************************
// IIC Read byte
**********************************************/
unsigned char MYI2C_Read_IIC_Byte(void)
{
    unsigned char i, byt = 0;

    MYI2C_SCK_Clr();
    MYI2C_SDA_IN_Mode;
    MYI2C_SDA_Set();
    for (i = 0; i < 8; i++)
    {
        MYI2C_SCK_Clr();
        if (MYI2C_GPIO_DATA(SDA_R)) byt++;
        MYI2C_SCK_Set();
        if (i < 7) byt = byt << 1;
    }
    MYI2C_SDA_OD_Mode;

    return byt;
}

/*******************************************************************************
* Function Name  : MYI2C_Init
* Description    : 初始化MYI2C
* Input          :  None
* Output         : None
* Return         :None
*******************************************************************************/
void MYI2C_Init(MYI2C_Struct *pst, unsigned int ReadTimMS, unsigned char xAddr)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能SDA、SCL端口时钟，如是51单片机不需要则注释掉该行。

    pst->Adrr = xAddr;
    pst->Step = SENSOR_IDLE;
    if (ReadTimMS > MinReadTim)
        pst->SetRTim = ReadTimMS;
    else
        pst->SetRTim = MinReadTim;

    MYI2C_SCK_OD_Mode;
    MYI2C_SDA_OD_Mode;

    MYI2C_SCK_Set();
    MYI2C_SDA_Set();
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          :None
* Output         :None
* Return         :None
*******************************************************************************/
unsigned char MYI2C_READ_FUNC(MYI2C_Struct *pst, unsigned char device_addr, unsigned char register_addr, unsigned char *pDat, unsigned char len)
{
    unsigned char NoAck = 0;

    if (register_addr)
    {
        /* Send STRAT condition a second time */
        MYI2C_IIC_Start();
        /* Send slave address for Write Regsiter */
        MYI2C_Write_IIC_Byte((device_addr << 1) + 0);
        /* Ack */
        if (MYI2C_IIC_Wait_Ack(Wait_Ack_time) == 0) NoAck++;
        /*Send register_addr*/
        MYI2C_Write_IIC_Byte((register_addr));
        /* Ack */
        if (MYI2C_IIC_Wait_Ack(Wait_Ack_time) == 0) NoAck++;

        //   MYI2C_IIC_Stop(pst);
        MYI2C_SCK_Clr();
        MYI2C_SCK_Set();
    }
    /* Send STRAT condition a second time */
    MYI2C_IIC_Start();
    /* Send slave address for Read */
    MYI2C_Write_IIC_Byte((device_addr << 1) + 1);
    /* Ack */
    if (MYI2C_IIC_Wait_Ack(Wait_Ack_time) == 0) NoAck++;
    /* While there is data to be read */
    while (len && NoAck == 0 && len < MYI2C_Buffer_Size) //
    {
        *pDat = MYI2C_Read_IIC_Byte();
        /*  Ack */
        MYI2C_IIC_Ack(len - 1); //len = 1 NoAck
        pDat++;
        len--;
    }
    /* Send STOP Condition */
    MYI2C_IIC_Stop();

    pst->ErrFlag = NoAck;

    return NoAck;
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          :  None
* Output         : None
* Return         :None
*******************************************************************************/
unsigned char MYI2C_WRITE_FUNC(MYI2C_Struct *pst, unsigned char device_addr, unsigned char register_addr, unsigned char *pDat, unsigned char len)
{
    unsigned int NoAck = 0;

    /* Send STRAT condition */
    MYI2C_IIC_Start();
    /* Send slave address for write */
    MYI2C_Write_IIC_Byte((device_addr << 1) + 0);
    /* ACK */
    if (MYI2C_IIC_Wait_Ack(Wait_Ack_time) == 0) NoAck++;
    /* Send register_addr for read */
    MYI2C_Write_IIC_Byte((register_addr));
    /* ACK */
    if (MYI2C_IIC_Wait_Ack(Wait_Ack_time) == 0) NoAck++;
    while (NoAck == 0 && len && len < MYI2C_Buffer_Size) //
    {
        /* Send the byte to be written */
        MYI2C_Write_IIC_Byte(*pDat);
        /*  Acknowledgement */
        MYI2C_IIC_Wait_Ack(Wait_Ack_time);
        pDat++;
        len--;
    }
    /* Send STOP condition */
    MYI2C_IIC_Stop();

    pst->ErrFlag = NoAck;

    return NoAck;
}
/*******************************************************************************
* Function Name  : CheckCrc
* Description    :  
* Input          :  None
* Output         : None
* Return         :None
*******************************************************************************/
unsigned char CheckCrc8(unsigned char *pDat, unsigned char Lenth)
{
    unsigned char crc = 0xff, i, j;

    for (i = 0; i < Lenth; i++)
    {
        crc = crc ^ *pDat;
        for (j = 0; j < 8; j++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc <<= 1;
        }
        pDat++;
    }
    return crc;
}
/*******************************************************************************
* Function Name  :  
* Description    :  
* Input          :None
* Output         :None
* Return         :None
*******************************************************************************/
void MYI2C_Handle(MYI2C_Struct *pst)
{
    unsigned long s32x;

    pst->timcnt += MYI2C_Tick;
    if (pst->timcnt > PowerOnTim && pst->Step == SENSOR_IDLE)
    {
        pst->Step        = SENSOR_MEASURE;
        pst->SendByte[0] = 0x33;
        pst->SendByte[1] = 0x00;
        MYI2C_WRITE_FUNC(pst, pst->Adrr, 0xAC, &pst->SendByte[0], 2);
    }
    else if (pst->timcnt > MeasureTim && pst->Step == SENSOR_MEASURE)
    {
        pst->Step = SENSOR_COMPLETE;
        MYI2C_READ_FUNC(pst, pst->Adrr, 0, &pst->ReadByte[0], 7);
        if (pst->ErrFlag == 0)
        {
            if ((CheckCrc8(&pst->ReadByte[0], 6) == pst->ReadByte[6]) && ((pst->ReadByte[0] & 0x98) == 0x18))
            {
                s32x     = pst->ReadByte[1];
                s32x     = s32x << 8;
                s32x    += pst->ReadByte[2];
                s32x     = s32x << 8;
                s32x    += pst->ReadByte[3];
                s32x     = s32x >> 4;
                pst->RH  = s32x;
                pst->RH  = pst->RH * 100 / 1048576;
                s32x     = pst->ReadByte[3] & 0x0F;
                s32x     = s32x << 8;
                s32x    += pst->ReadByte[4];
                s32x     = s32x << 8;
                s32x    += pst->ReadByte[5];
                pst->T   = s32x;
                pst->T   = pst->T * 200 / 1048576 - 50;
            }
        }
        else
        {
            pst->RH = 0;
            pst->T  = 0;
        }
    }
    else if (pst->timcnt > pst->SetRTim)
    {
        pst->Step   = SENSOR_IDLE;
        pst->timcnt = 0;
    }
}

