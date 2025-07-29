#include "w25qxx.h"
#include "delay.h"
#include "mass_mal.h"

u16 W25QXX_TYPE = 0; //The default is W25Q128.

uint16_t wq25xx_Flash_Init(void);
uint16_t wq25xx_WriteMultiBlocks(uint32_t Memory_Offset, uint8_t *Writebuff, uint16_t Transfer_Length);
uint16_t wq25xx_ReadMultiBlocks(uint32_t Memory_Offset, uint8_t *Readbuff, uint16_t Transfer_Length);
uint16_t wq25xx_Flash_GetStatus(uint8_t lun);

uint8_t wq25xx_Flash_Inquiry_Data[] =
    {
        0x00, /* Direct Access Device */
        0x80, /* RMB = 1: Removable Medium */
        0x02, /* Version: No conformance claim to standard */
        0x02,

        36 - 4, /* Additional Length */
        0x00,   /* SCCS = 1: Storage Controller Component */
        0x00,
        0x00,
        /* Vendor Identification */
        'W', 'I', 'Z', 'n', 'e', 't', ' ', ' ',
        /* Product Identification */
        'w', 'q', '2', '5', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
        'D', 'i', 's', 'k',
        /* Product Revision Level */
        '1', '.', '0', ' '};

t_Mal_Lun w25qxx_Flash =
    {
        wq25xx_Flash_Init,
        wq25xx_WriteMultiBlocks,
        wq25xx_ReadMultiBlocks,
        wq25xx_Flash_GetStatus,
        wq25xx_Flash_Inquiry_Data,
};

//SPIx reads and writes a byte
//TxData: Bytes to write
//Return value: bytes read
u8 SPI_ReadWriteByte(u8 TxData)
{
    u8 retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //Check whether the specified SPI flag is set or not: Send cache null flag
    {
        retry++;
        if (retry > 200) return 0;
    }
    SPI_I2S_SendData(SPI1, TxData); //Send a data via peripheral SPIx
    retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //Check whether the specified SPI flag is set or not: Accept cache non-empty flags
    {
        retry++;
        if (retry > 200) return 0;
    }
    return SPI_I2S_ReceiveData(SPI1); //Returns the most recently received data via SPIx
}

void SPI_InitTest(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial     = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    SPI_Cmd(SPI1, ENABLE);

    SPI_ReadWriteByte(0xff);
}
//SPI speed setting function
//SpeedSet:
//SPI_BaudRatePrescaler_2 divided by 2
//SPI_BaudRatePrescaler_8 8 divided frequency
//SPI_BaudRatePrescaler_16 16 divided frequency
//SPI_BaudRatePrescaler_256 256 divided frequency


void SPI_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    SPI1->CR1 &= 0XFFC7;
    SPI1->CR1 |= SPI_BaudRatePrescaler;
    SPI_Cmd(SPI1, ENABLE);
}


//4Kbytes for a Sector
//16 sectors for 1 Block
//W25Q128
//16M bytes, 128 Blocks, 4096 Sectors

//Initialize the IO port of SPI FLASH
void W25QXX_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_4);

    W25QXX_CS = 1;                 //SPI FLASH not selected
    SPI_InitTest();                //Initialize SPI
    SPI_SetSpeed(SPI_BaudRatePrescaler_4);
    W25QXX_TYPE = W25QXX_ReadID(); //Read flash ID.
}

//Read the status register of the W25QXX
//BIT7 6 5 4 3 2 1 0
//SPR RV TB BP2 BP1 BP0 WEL BUSY
//SPR: default 0, status register protection bit, used with WP
//TB, BP2, BP1, BP0: FLASH regional write protection settings
//WEL: Write enable lock
//BUSY: busy flag bit (1, busy; 0, idle)
//Default: 0x00
u8 W25QXX_ReadSR(void)
{
    u8 byte   = 0;
    W25QXX_CS = 0;                         //enable device
    SPI_ReadWriteByte(W25X_ReadStatusReg); //Send read status register command
    byte      = SPI_ReadWriteByte(0Xff);   //Read a byte
    W25QXX_CS = 1;                         //Cancel selection
    return byte;
}
//Write W25QXX Status Register
//Only SPR, TB, BP2, BP1, BP0 (bit 7, 5, 4, 3, 2) can be written!!!
void W25QXX_Write_SR(u8 sr)
{
    W25QXX_CS = 0;                          //enable device
    SPI_ReadWriteByte(W25X_WriteStatusReg); //Send write status register command
    SPI_ReadWriteByte(sr);                  //Write a byte
    W25QXX_CS = 1;                          //Cancel selection
}
//W25QXX write enable
//Set WEL
void W25QXX_Write_Enable(void)
{
    W25QXX_CS = 0;                       //enable device
    SPI_ReadWriteByte(W25X_WriteEnable); //Send write enable
    W25QXX_CS = 1;                       //Cancel selection
}
//W25QXX write ban
//Clear WEL
void W25QXX_Write_Disable(void)
{
    W25QXX_CS = 0;                        //enable device
    SPI_ReadWriteByte(W25X_WriteDisable); //Send write ban command
    W25QXX_CS = 1;                        //Cancel selection
}
//Read the chip ID
//The return value is as follows:
//0XEF13, indicating that the chip model is W25Q80.
//0XEF14, indicating that the chip model is W25Q16.
//0XEF15, indicating that the chip model is W25Q32
//0XEF16, indicating that the chip model is W25Q64.
//0XEF17, indicating that the chip model is W25Q128
u16 W25QXX_ReadID(void)
{
    u16 Temp  = 0;
    W25QXX_CS = 0;
    SPI_ReadWriteByte(0x90); //Send Read ID command
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    SPI_ReadWriteByte(0x00);
    Temp      |= SPI_ReadWriteByte(0xFF) << 8;
    Temp      |= SPI_ReadWriteByte(0xFF);
    W25QXX_CS  = 1;
    return Temp;
}
//read SPI FLASH
//Start reading data of the specified length at the specified address
//pBuffer: data store
//ReadAddr: Address to start reading (24bit)
//NumByteToRead: The number of bytes to read (max. 65535)
void W25QXX_Read(u8 *pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    u16 i;
    W25QXX_CS = 0;                             //enable device
    SPI_ReadWriteByte(W25X_ReadData);          //Send read command
    SPI_ReadWriteByte((u8)((ReadAddr) >> 16)); //Send 24bit address
    SPI_ReadWriteByte((u8)((ReadAddr) >> 8));
    SPI_ReadWriteByte((u8)ReadAddr);
    for (i = 0; i < NumByteToRead; i++)
    {
        pBuffer[i] = SPI_ReadWriteByte(0XFF); //cyclic reading
    }
    W25QXX_CS = 1;
}
//SPI writes less than 256 bytes of data in one page (0~ 65535)
//Start writing data up to 256 bytes at the specified address
//pBuffer: data store
//WriteAddr: Address to start writing (24bit)
//NumByteToWrite: The number of bytes to write (max 256), which should not exceed the number of bytes remaining on the page!!!
void W25QXX_Write_Page(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 i;
    W25QXX_Write_Enable();                      //SET WEL
    W25QXX_CS = 0;                              //enable device
    SPI_ReadWriteByte(W25X_PageProgram);        //Send page write command
    SPI_ReadWriteByte((u8)((WriteAddr) >> 16)); //Send 24bit address
    SPI_ReadWriteByte((u8)((WriteAddr) >> 8));
    SPI_ReadWriteByte((u8)WriteAddr);
    for (i = 0; i < NumByteToWrite; i++) SPI_ReadWriteByte(pBuffer[i]); //write loop
    W25QXX_CS = 1;                                                      //Cancel selection
    W25QXX_Wait_Busy();                                                 //Wait for write to finish
}
//Write SPI FLASH without test
//You must ensure that all data within the written address range is 0XFF, otherwise data written at non-0XFF will fail!
//with automatic page feed function
//Start writing data of the specified length at the specified address, but make sure the address does not exceed the limit!
//pBuffer: data store
//WriteAddr: Address to start writing (24bit)
//NumByteToWrite: The number of bytes to write (max. 65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain = 256 - WriteAddr % 256;                            //The number of bytes remaining on a single page
    if (NumByteToWrite <= pageremain) pageremain = NumByteToWrite; //No more than 256 bytes
    while (1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        if (NumByteToWrite == pageremain)
            break; //Write is over
        else       //NumByteToWrite>pageremain
        {
            pBuffer   += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;    //Subtract the number of bytes that have been written
            if (NumByteToWrite > 256)
                pageremain = 256;            //256 bytes can be written at a time
            else
                pageremain = NumByteToWrite; //Not enough 256 bytes
        }
    };
}
//write SPI FLASH
//Start writing data of the specified length at the specified address
//This function has an erase operation!
//pBuffer: data store
//WriteAddr: Address to start writing (24bit)
//NumByteToWrite: The number of bytes to write (max. 65535)
u8   W25QXX_BUFFER[4096];
void W25QXX_Write(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 *W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos     = WriteAddr / 4096; //sector address
    secoff     = WriteAddr % 4096; //Offset within a sector
    secremain  = 4096 - secoff;    //Sector free space size
    //printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//for testing
    if (NumByteToWrite <= secremain) secremain = NumByteToWrite; //No more than 4096 bytes
    while (1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);  //Read the content of the entire sector
        for (i = 0; i < secremain; i++)                //validation data
        {
            if (W25QXX_BUF[secoff + i] != 0XFF) break; //Need to erase
        }
        if (i < secremain)                             //Need to erase
        {
            W25QXX_Erase_Sector(secpos);               //Erase this sector
            for (i = 0; i < secremain; i++)            //copy
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096); //Write entire sector
        }
        else
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); //Write what has been erased, directly write the remaining section of the sector.
        if (NumByteToWrite == secremain)
            break;                                               //Write is over
        else                                                     //Write not finished
        {
            secpos++;                                            //The sector address is incremented by 1.
            secoff = 0;                                          //The offset position is 0.

            pBuffer        += secremain;                         //pointer offset
            WriteAddr      += secremain;                         //write address offset
            NumByteToWrite -= secremain;                         //Decrementing Bytes
            if (NumByteToWrite > 4096)
                secremain = 4096;                                //I can't finish writing the next sector.
            else
                secremain = NumByteToWrite;                      //The next sector can be written
        }
    };
}
//Erase the entire chip
//Waiting time is too long...
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();             //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS = 0;                     //enable device
    SPI_ReadWriteByte(W25X_ChipErase); //Send slice erase command
    W25QXX_CS = 1;                     //Cancel selection
    W25QXX_Wait_Busy();                //Wait for chip erase to finish
}
//erase a sector
//Dst_Addr: sector address, set according to actual capacity
//Minimum time to erase a mountain: 150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
    //Monitor falsh erasure, test
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();                     //SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS = 0;                             //enable device
    SPI_ReadWriteByte(W25X_SectorErase);       //Send sector erase command
    SPI_ReadWriteByte((u8)((Dst_Addr) >> 16)); //Send 24bit address
    SPI_ReadWriteByte((u8)((Dst_Addr) >> 8));
    SPI_ReadWriteByte((u8)Dst_Addr);
    W25QXX_CS = 1;      //Cancel selection
    W25QXX_Wait_Busy(); //Wait for erase to complete
}
//Wait for idle
void W25QXX_Wait_Busy(void)
{
    while ((W25QXX_ReadSR() & 0x01) == 0x01); // Wait for the BUSY bit to clear
}
//Enter power-down mode
void W25QXX_PowerDown(void)
{
    W25QXX_CS = 0;                     //enable device
    SPI_ReadWriteByte(W25X_PowerDown); //Send power down command
    W25QXX_CS = 1;                     //Cancel selection
    delay_us(3);                       //Waiting for TPD
}
//wake up
void W25QXX_WAKEUP(void)
{
    W25QXX_CS = 0;                            //enable device
    SPI_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB
    W25QXX_CS = 1;                            //Cancel selection
    delay_us(3);                              //Wait for TRES1
}

uint16_t wq25xx_Flash_Init(void)
{
    W25QXX_Init();
    return MAL_OK;
}

uint16_t wq25xx_WriteMultiBlocks(uint32_t Memory_Offset, uint8_t *Writebuff, uint16_t Transfer_Length)
{
    W25QXX_Erase_Sector(Memory_Offset);
    W25QXX_Write_NoCheck(Writebuff, Memory_Offset, Transfer_Length);
    return MAL_OK;
}

uint16_t wq25xx_ReadMultiBlocks(uint32_t Memory_Offset, uint8_t *Readbuff, uint16_t Transfer_Length)
{
    W25QXX_Read(Readbuff, Memory_Offset, Transfer_Length);
    return MAL_OK;
}

uint16_t wq25xx_Flash_GetStatus(uint8_t lun)
{
    if ((W25QXX_TYPE & 0xFF) == 0x13)
        Mass_Block_Count[lun] = 0x100000 / 0x1000;
    else if ((W25QXX_TYPE & 0xFF) == 0x14)
        Mass_Block_Count[lun] = 0x200000 / 0x1000;
    else if ((W25QXX_TYPE & 0xFF) == 0x15)
        Mass_Block_Count[lun] = 0x400000 / 0x1000;
    else if ((W25QXX_TYPE & 0xFF) == 0x16)
        Mass_Block_Count[lun] = 0x800000 / 0x1000;
    else if ((W25QXX_TYPE & 0xFF) == 0x17)
        Mass_Block_Count[lun] = 0x1000000 / 0x1000;
    else
        return MAL_FAIL;
    Mass_Block_Size[lun] = 0x1000;
    return MAL_OK;
}
