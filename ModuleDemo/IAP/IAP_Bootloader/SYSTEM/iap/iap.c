#include "iap.h"

uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
    return *(vu16 *)faddr;
}

//Read the data of the specified length from the specified address
//ReadAddr: starting address
//pBuffer: data pointer
//NumToWrite: Half-word (16-bit) number
void FLASH_Read(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead)
{
    uint16_t i;
    for (i = 0; i < NumToRead; i++)
    {
        pBuffer[i]  = STMFLASH_ReadHalfWord(ReadAddr); //Read 2 bytes.
        ReadAddr   += 2;                               //Offset by 2 bytes.
    }
}

uint16_t FLASH_ReadHalfWord(uint32_t faddr)
{
    return *(vu16 *)faddr;
}

//unchecked write
//WriteAddr: starting address
//pBuffer: data pointer
//NumToWrite: Half-word (16-bit) number
void FLASH_Write_NoCheck(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
    uint16_t i;
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
uint16_t FLASH_BUF[FLASH_SECTOR_SIZE / 2]; //2K bytes at most
void     FLASH_Write(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite)
{
    uint32_t secpos;                                                                                             //sector address
    uint16_t secoff;                                                                                             //Intra-sector offset address (16-bit word calculation)
    uint16_t secremain;                                                                                          //Remaining address in sector (16-bit word calculation)
    uint16_t i;
    uint32_t offaddr;                                                                                            //Remove the address after 0X08000000.
    if (WriteAddr < W55MH32_FLASH_BASE || (WriteAddr >= (W55MH32_FLASH_BASE + 128 * FLASH_SECTOR_SIZE))) return; //Illegal address
    FLASH_Unlock();                                                                                              //Unlock
    offaddr   = WriteAddr - W55MH32_FLASH_BASE;                                                                  //Actual offset address.
    secpos    = offaddr / FLASH_SECTOR_SIZE;                                                                     //sector address
    secoff    = (offaddr % FLASH_SECTOR_SIZE) / 2;                                                               //Offsets within sectors (2)
    secremain = FLASH_SECTOR_SIZE / 2 - secoff;                                                                  //Sector free space size
    if (NumToWrite <= secremain) secremain = NumToWrite;                                                         //No greater than the sector range
    while (1)
    {
        FLASH_Read(secpos * FLASH_SECTOR_SIZE + W55MH32_FLASH_BASE, FLASH_BUF, FLASH_SECTOR_SIZE / 2); //Read the content of the entire sector
        for (i = 0; i < secremain; i++)                                                                //validation data
        {
            if (FLASH_BUF[secoff + i] != 0XFFFF) break;                                                //Need to erase
        }
        if (i < secremain)                                                                             //Need to erase
        {
            FLASH_ErasePage(secpos * FLASH_SECTOR_SIZE + W55MH32_FLASH_BASE);                          //Erase this sector
            for (i = 0; i < secremain; i++)                                                            //copy
            {
                FLASH_BUF[i + secoff] = pBuffer[i];
            }
            FLASH_Write_NoCheck(secpos * FLASH_SECTOR_SIZE + W55MH32_FLASH_BASE, FLASH_BUF, FLASH_SECTOR_SIZE / 2); //Write entire sector
        }
        else
            FLASH_Write_NoCheck(WriteAddr, pBuffer, secremain); //Write what has been erased, directly write the remaining section of the sector.
        if (NumToWrite == secremain)
            break;                                              //Write is over
        else                                                    //Write not finished
        {
            secpos++;                                           //The sector address is incremented by 1.
            secoff      = 0;                                    //The offset position is 0.
            pBuffer    += secremain;                            //pointer offset
            WriteAddr  += secremain;                            //write address offset
            NumToWrite -= secremain;                            //Diminishing number of bytes (16 bits)
            if (NumToWrite > (FLASH_SECTOR_SIZE / 2))
                secremain = FLASH_SECTOR_SIZE / 2;              //I can't finish writing the next sector.
            else
                secremain = NumToWrite;                         //The next sector can be written
        }
    };
    FLASH_Lock(); //Lock
}

iapfun   jump2app;
uint16_t iapbuf[1024];
void     IAP_Write_Appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize)
{
    uint16_t t;
    uint16_t i = 0;
    uint16_t temp;
    uint32_t fwaddr = appxaddr; //The address currently written to
    uint8_t *dfu    = appbuf;
    for (t = 0; t < appsize; t += 2)
    {
        temp         = (u16)dfu[1] << 8;
        temp        += (u16)dfu[0];
        dfu         += 2; //Offset by 2 bytes
        iapbuf[i++]  = temp;
        if (i == 1024)
        {
            i = 0;
            FLASH_Write(fwaddr, iapbuf, 1024);
            fwaddr += 2048; //Offset 2048 16 = 2 * 8. So multiply by 2.
        }
    }
    if (i) FLASH_Write(fwaddr, iapbuf, i); //Write in the last few bytes of content.
}

//Set the top of the stack address
//addr: top of stack address
__asm void MSR_MSP(uint32_t addr)
{
    MSR          MSP, r0 //set Main Stack value
                 BX r14
}
//jump to the application section
//appxaddr: the starting address of the user code.
void IAP_Load_App(uint32_t appxaddr)
{
    if (((*(vu32 *)appxaddr) & 0x2FFE0000) == 0x20000000) //Check if the top of the stack address is valid.
    {
        jump2app = (iapfun) * (vu32 *)(appxaddr + 4);     //The second word in the user code area is the program start address (reset address).
        MSR_MSP(*(vu32 *)appxaddr);                       //Initialize the APP stack pointer (the first word of the user code area is used to store the top address of the stack)
        jump2app();                                       //Go to the APP.
    }
}

