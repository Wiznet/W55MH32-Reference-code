#include "eeprom.h"
#include "delay.h"

//Read the half word (16-bit data) of the specified address
//faddr: read address (this address must be a multiple of 2!!)
//Return value: corresponding data.
u16 WIZFLASH_ReadHalfWord(u32 faddr)
{
    return *(vu16 *)faddr;
}
#if WIZ_FLASH_WREN //if enabled to write
//unchecked write
//WriteAddr: starting address
//pBuffer: data pointer
//NumToWrite: Half-word (16-bit) number
void WIZFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
    u16 i;
    for (i = 0; i < NumToWrite; i++)
    {
        FLASH_ProgramHalfWord(WriteAddr, pBuffer[i]);
        WriteAddr += 2; //Address is increased by 2.
    }
}
//Write data of the specified length starting from the specified address
//WriteAddr: Start address (this address must be a multiple of 2!!)
//pBuffer: data pointer
//NumToWrite: halfword (16-bit) number (that is, the number of 16-bit data to be written.)

#define WIZ_SECTOR_SIZE 2048

u16  WIZFLASH_BUF[WIZ_SECTOR_SIZE / 2]; //at most 2 k bytes
void WIZFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
    u32 secpos;                                                                                        //sector address
    u16 secoff;                                                                                        //Intra-sector offset address (16-bit word calculation)
    u16 secremain;                                                                                     //Remaining address in sector (16-bit word calculation)
    u16 i;
    u32 offaddr;                                                                                       //Remove the address after 0X08000000.
    if (WriteAddr < WIZ_FLASH_BASE || (WriteAddr >= (WIZ_FLASH_BASE + 1024 * WIZ_FLASH_SIZE))) return; //Illegal address
    FLASH_Unlock();                                                                                    //Unlock
    offaddr   = WriteAddr - WIZ_FLASH_BASE;                                                            //Actual offset address.
    secpos    = offaddr / WIZ_SECTOR_SIZE;                                                             //sector address
    secoff    = (offaddr % WIZ_SECTOR_SIZE) / 2;                                                       //Offset within a sector (2 bytes in base units.)
    secremain = WIZ_SECTOR_SIZE / 2 - secoff;                                                          //Sector free space size
    if (NumToWrite <= secremain) secremain = NumToWrite;                                               //No greater than the sector range
    while (1)
    {
        WIZFLASH_Read(secpos * WIZ_SECTOR_SIZE + WIZ_FLASH_BASE, WIZFLASH_BUF, WIZ_SECTOR_SIZE / 2); //Read the content of the entire sector
        for (i = 0; i < secremain; i++)                                                              //validation data
        {
            if (WIZFLASH_BUF[secoff + i] != 0XFFFF) break;                                           //Need to erase
        }
        if (i < secremain)                                                                           //Need to erase
        {
            FLASH_ErasePage(secpos * WIZ_SECTOR_SIZE + WIZ_FLASH_BASE);                              //Erase this sector
            for (i = 0; i < secremain; i++)                                                          //copy
            {
                WIZFLASH_BUF[i + secoff] = pBuffer[i];
            }
            WIZFLASH_Write_NoCheck(secpos * WIZ_SECTOR_SIZE + WIZ_FLASH_BASE, WIZFLASH_BUF, WIZ_SECTOR_SIZE / 2); //Write entire sector
        }
        else
            WIZFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain); //Write what has been erased, directly write the remaining section of the sector.
        if (NumToWrite == secremain)
            break;                                                 //Write is over
        else                                                       //Write not finished
        {
            secpos++;                                              //The sector address is incremented by 1.
            secoff      = 0;                                       //The offset position is 0.
            pBuffer    += secremain;                               //pointer offset
            WriteAddr  += secremain * 2;                           //write address offset
            NumToWrite -= secremain * 2;                           //Diminishing number of bytes (16 bits)
            if (NumToWrite > (WIZ_SECTOR_SIZE / 2))
                secremain = WIZ_SECTOR_SIZE / 2;                   //I can't finish writing the next sector.
            else
                secremain = NumToWrite;                            //The next sector can be written
        }
    };
    FLASH_Lock(); //Lock
}
#endif

//Read the data of the specified length from the specified address
//ReadAddr: starting address
//pBuffer: data pointer
//NumToWrite: Half-word (16-bit) number
void WIZFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
    u16 i;
    for (i = 0; i < NumToRead; i++)
    {
        pBuffer[i]  = WIZFLASH_ReadHalfWord(ReadAddr); //Read 2 bytes.
        ReadAddr   += 2;                               //Offset by 2 bytes.
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr: starting address
//WriteData: The data to be written
void Test_Write(u32 WriteAddr, u16 WriteData)
{
    WIZFLASH_Write(WriteAddr, &WriteData, 1); //Write a word
}


