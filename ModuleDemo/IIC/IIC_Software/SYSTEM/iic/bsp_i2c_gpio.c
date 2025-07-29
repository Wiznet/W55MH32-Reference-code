#include "bsp_i2c_gpio.h"
#include "delay.h"
#include "w55mh32.h"

static void i2c_Delay(void)
{
    delay_us(1);
}

/*
*********************************************************************************************************
* Function name: i2c_Start
* Function description: CPU initiates I2C bus start signal
* Parameters: None
* Return value: none
*********************************************************************************************************
*/
void i2c_Start(void)
{
    /* When SCL is high, a down-hop edge appears in SDA indicating the I2C bus start signal */
    EEPROM_I2C_SDA_1();
    EEPROM_I2C_SCL_1();
    i2c_Delay();
    EEPROM_I2C_SDA_0();
    i2c_Delay();
    EEPROM_I2C_SCL_0();
    i2c_Delay();
}

/*
*********************************************************************************************************
* Function name: i2c_Start
* Function description: CPU initiates I2C bus stop signal
* Parameters: None
* Return value: none
*********************************************************************************************************
*/
void i2c_Stop(void)
{
    /* When SCL is high, a jump edge appears on SDA indicating an I2C bus stop signal */
    EEPROM_I2C_SDA_0();
    EEPROM_I2C_SCL_1();
    i2c_Delay();
    EEPROM_I2C_SDA_1();
}

/*
*********************************************************************************************************
* Function name: i2c_SendByte
* Function description: CPU sends 8bit data to I2C bus device
* Parameter: _ucByte: Bytes waiting to be sent
* Return value: none
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
    uint8_t i;

    /* Send the high bit bit7 of the byte first */
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            EEPROM_I2C_SDA_1();
        }
        else
        {
            EEPROM_I2C_SDA_0();
        }
        i2c_Delay();
        EEPROM_I2C_SCL_1();
        i2c_Delay();
        EEPROM_I2C_SCL_0();
        if (i == 7)
        {
            EEPROM_I2C_SDA_1(); // Release the bus
        }
        _ucByte <<= 1;          /* One bit to the left */
        i2c_Delay();
    }
}

/*
*********************************************************************************************************
* Function name: i2c_ReadByte
* Function description: CPU reads 8bit data from I2C bus device
* Parameters: None
* Return value: read data
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(void)
{
    uint8_t i;
    uint8_t value;

    /* Read the first bit as bit7 of the data */
    value = 0;
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        EEPROM_I2C_SCL_1();
        i2c_Delay();
        if (EEPROM_I2C_SDA_READ())
        {
            value++;
        }
        EEPROM_I2C_SCL_0();
        i2c_Delay();
    }
    return value;
}

/*
*********************************************************************************************************
* Function name: i2c_WaitAck
Function description: CPU generates a clock and reads the device's ACK response signal
* Parameters: None
Return value: 0 means correct response, 1 means no device response
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
    uint8_t re;

    EEPROM_I2C_SDA_1();        /* CPU releases SDA bus */
    i2c_Delay();
    EEPROM_I2C_SCL_1();        /* CPU driver SCL = 1, the device will return an ACK response at this time */
    i2c_Delay();
    if (EEPROM_I2C_SDA_READ()) /* CPU reads SDA port status */
    {
        re = 1;
    }
    else
    {
        re = 0;
    }
    EEPROM_I2C_SCL_0();
    i2c_Delay();
    return re;
}

/*
*********************************************************************************************************
* Function name: i2c_Ack
* Function description: CPU generates an ACK signal
* Parameters: None
* Return value: none
*********************************************************************************************************
*/
void i2c_Ack(void)
{
    EEPROM_I2C_SDA_0(); /* CPU driver SDA = 0 */
    i2c_Delay();
    EEPROM_I2C_SCL_1(); /* CPU generates 1 clock */
    i2c_Delay();
    EEPROM_I2C_SCL_0();
    i2c_Delay();
    EEPROM_I2C_SDA_1(); /* CPU releases SDA bus */
}

/*
*********************************************************************************************************
* Function name: i2c_NAck
* Function description: CPU generates 1 NACK signal
* Parameters: None
* Return value: none
*********************************************************************************************************
*/
void i2c_NAck(void)
{
    EEPROM_I2C_SDA_1(); /* CPU driver SDA = 1 */
    i2c_Delay();
    EEPROM_I2C_SCL_1(); /* CPU generates 1 clock */
    i2c_Delay();
    EEPROM_I2C_SCL_0();
    i2c_Delay();
}

/*
*********************************************************************************************************
* Function name: i2c_CfgGpio
* Function description: Configure the GPIO of the I2C bus and implement it in an analog IO manner
* Parameters: None
* Return value: none
*********************************************************************************************************
*/
void i2c_CfgGpio(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(EEPROM_RCC_I2C_PORT, ENABLE); /* Turn on the GPIO clock */
    GPIO_InitStructure.GPIO_Pin   = EEPROM_I2C_SCL_PIN | EEPROM_I2C_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD; /* Open drain output */
    GPIO_Init(EEPROM_GPIO_PORT_I2C, &GPIO_InitStructure);

    /* Give a stop signal, reset all devices on the I2C bus to standby mode */
    i2c_Stop();
}


