#include "eeprom.h"

static void i2c_Delay(void)
{
    delay_us(1);
}

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
* Function name: i2c_CheckDevice
* Function description: Detect the I2C bus device, the CPU sends the address of the sending device, and then reads the device reply to determine whether the device exists
* Parameters: _Address: I2C bus address of the device
* Return value: Return value 0 means correct, return 1 means not detected
*********************************************************************************************************
*/
uint8_t ee_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;

    i2c_Start(); /* Send start signal */

    /* Send device address + read and write control bits (0 = w, 1 = r) bit7 first */
    i2c_SendByte(_Address | EEPROM_I2C_WR);
    ucAck = i2c_WaitAck(); /* Detect the ACK response of the device */

    i2c_Stop();            /* Send stop signal */

    i2c_NAck();            /*If the input is a read address, a negative-acknowledgement signal needs to be generated*/

    return ucAck;
}


/*
*********************************************************************************************************
* Function name: ee_WaitStandby
* Function description: Wait for the EEPROM to be ready. After writing data, this function must be called

During the write operation, after transferring the data to the EEPROM using I2C,
EEPROM will take a certain amount of time to write data to the internal space,
When the internal write of the EEPROM is completed, it will respond to the addressing of the I2C device.
Call this function to wait until the EEPROM internal timing is written
* Parameters: None
* Return value: 0 means normal, 1 means wait timeout
*********************************************************************************************************
*/
uint8_t ee_WaitStandby(void)
{
    uint32_t wait_count = 0;

    while (ee_CheckDevice(EEPROM_DEV_ADDR))
    {
        //If the number of detections exceeds, exit the loop
        if (wait_count++ > 0xFFFF)
        {
            //wait timeout
            return 1;
        }
    }
    //Wait for completion
    return 0;
}

/*
*********************************************************************************************************
* Function name: ee_ReadBytes
* Function description: Start reading several data from the address specified by the serial EEPROM
* Parameter: _usAddress: Start address
* _usSize: Data length in bytes
* _pReadBuf: Buffer pointer to hold read data
Return value: 0 indicates failure, 1 indicates success
*********************************************************************************************************
*/
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i;

    /* Use serial EEPROM to read the instruction sequence randomly, continuously reading several bytes */

    /* Step 1: Initiate the I2C bus start signal */
    i2c_Start();

    /* Step 2: Initiate the control byte, the high 7bit is the address, bit0 is the read and write control bit, 0 means write, 1 means read */
    i2c_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_WR); /* Here is the write instruction */

    /* Step 3: Wait for ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail; /* EEPROM device does not respond */
    }

    /* Step 4: Send the byte address, 24C02 is only 256 bytes, so 1 byte is enough. If it is above 24C04, then multiple addresses need to be sent in a row here */
    i2c_SendByte((uint8_t)_usAddress);

    /* Step 5: Wait for ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail; /* EEPROM device does not respond */
    }

    /* Step 6: Restart the I2C bus. The purpose of the previous code is to transfer the address to the EEPROM, and the following starts reading the data */
    i2c_Start();

    /* Step 7: Initiate the control byte, the high 7bit is the address, bit0 is the read and write control bit, 0 means write, 1 means read */
    i2c_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_RD); /* Here is the read command */

    /* Step 8: Send ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail; /* EEPROM device does not respond */
    }

    /* Step 9: Loop through the data */
    for (i = 0; i < _usSize; i++)
    {
        _pReadBuf[i] = i2c_ReadByte(); /* Read 1 byte */

        /* After reading 1 byte, you need to send Ack, the last byte does not need Ack, send Nack */
        if (i != _usSize - 1)
        {
            i2c_Ack(); /* After the intermediate byte is read, the CPU generates an ACK signal (driver SDA = 0) */
        }
        else
        {
            i2c_NAck(); /* After the last byte is read, the CPU generates a NACK signal (driver SDA = 1) */
        }
    }
    /* Send I2C bus stop signal */
    i2c_Stop();
    return 1; /* Execution successful */

cmd_fail:     /* After the command execution fails, remember to send a stop signal to avoid affecting other devices on the I2C bus */
    /* Send I2C bus stop signal */
    i2c_Stop();
    return 0;
}

/*
*********************************************************************************************************
* Function name: ee_WriteBytes
* Function description: Write some data to the specified address of the serial EEPROM, and use the page write operation to improve the writing efficiency
* Parameter: _usAddress: Start address
* _usSize: Data length in bytes
* _pWriteBuf: Buffer pointer to hold read data
Return value: 0 indicates failure, 1 indicates success
*********************************************************************************************************
*/
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize)
{
    uint16_t i, m;
    uint16_t usAddr;

    /*
	Unlike read operations, which can read many bytes in a row, write serial EEPROMs can only be written on the same page each time.
	For 24xx02, page size = 8
	The simple processing method is: according to the byte write operation mode, every byte written, the address is sent
	In order to improve the efficiency of continuous writing: This function uses the page wirte operation.
	*/

    usAddr = _usAddress;
    for (i = 0; i < _usSize; i++)
    {
        /* When sending the first byte or the first page address, you need to restart the start signal and address */
        if ((i == 0) || (usAddr & (EEPROM_PAGE_SIZE - 1)) == 0)
        {
            /* Step 0: Send a stop signal to start the internal write operation */
            i2c_Stop();

            /* Determine whether the internal write operation is completed by checking the response of the device, generally less than 10ms 	
			
			  When the CLK frequency is 200KHz, the number of queries is about 30 times
			The principle is the same as ee_WaitStandby function, but the function will generate a stop signal after the check is completed, which does not apply here
			*/
            for (m = 0; m < 1000; m++)
            {
                /* Step 1: Initiate the I2C bus start signal */
                i2c_Start();

                /* Step 2: Initiate the control byte, the high 7bit is the address, bit0 is the read and write control bit, 0 means write, 1 means read */
                i2c_SendByte(EEPROM_DEV_ADDR | EEPROM_I2C_WR); /* Here are the write instructions */

                /* Step 3: Send a clock to determine if the device is responding correctly */
                if (i2c_WaitAck() == 0)
                {
                    break;
                }
            }
            if (m == 1000)
            {
                goto cmd_fail; /* EEPROM device write timeout */
            }

            /* Step 4: Send the byte address, 24C02 is only 256 bytes, so 1 byte is enough. If it is above 24C04, then multiple addresses need to be sent in a row here */
            i2c_SendByte((uint8_t)usAddr);

            /* Step 5: Wait for ACK */
            if (i2c_WaitAck() != 0)
            {
                goto cmd_fail; /* EEPROM device does not respond */
            }
        }

        /* Step 6: Start writing data */
        i2c_SendByte(_pWriteBuf[i]);

        /* Step 7: Send ACK */
        if (i2c_WaitAck() != 0)
        {
            goto cmd_fail; /* EEPROM device does not respond */
        }

        usAddr++; /* Add 1 to the address */
    }

    /* The command is executed successfully, and the I2C bus stop signal is sent */
    i2c_Stop();

    //Wait for the last internal EEPROM write to complete
    if (ee_WaitStandby() == 1) //Equal to 1 means timeout
        goto cmd_fail;

    return 1;

cmd_fail: /* After the command execution fails, remember to send a stop signal to avoid affecting other devices on the I2C bus */
    /* Send I2C bus stop signal */
    i2c_Stop();
    return 0;
}


void ee_Erase(void)
{
    uint16_t i;
    uint8_t  buf[EEPROM_SIZE];

    /* fill buffer */
    for (i = 0; i < EEPROM_SIZE; i++)
    {
        buf[i] = 0xFF;
    }

    /* Write EEPROM, start address = 0, data length is 256 */
    if (ee_WriteBytes(buf, 0, EEPROM_SIZE) == 0)
    {
        printf("Error erasing eeprom!\r\n");
        return;
    }
    else
    {
        printf("Erased eeprom successfully!\r\n");
    }
}


uint8_t check_eeprom_network_info(wiz_NetInfo *net_info)
{
    wiz_NetInfo eeprom_net_info = {0};

    /*-----------------------------------------------------------------------------------*/
    if (ee_CheckDevice(EEPROM_DEV_ADDR) == 1)
    {
        /* No EEPROM detected */
        printf("No serial EEPROM detected!\r\n");

        return 0;
    }
    ee_ReadBytes((uint8_t *)&eeprom_net_info, 0, sizeof(eeprom_net_info));
    if (eeprom_net_info.mac[0] == 0x00 && eeprom_net_info.mac[1] == 0x08 && eeprom_net_info.mac[2] == 0xdc)
    {
        memcpy(net_info, &eeprom_net_info, sizeof(wiz_NetInfo));
        return 1;
    }
    return 0;
}
/*********************************************END OF FILE**********************/
