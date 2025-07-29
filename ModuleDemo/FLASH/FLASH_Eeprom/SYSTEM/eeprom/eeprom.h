#ifndef __EEPROM_H__
#define __EEPROM_H__
#include "w55mh32.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//Users can set it according to their needs
#define WIZ_FLASH_SIZE 512 //FLASH capacity of the selected mh (in K)
#define WIZ_FLASH_WREN 1   //enable flash write (0, not enable; 1, enable)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH starting address
#define WIZ_FLASH_BASE 0x08000000 //wiz FLASH
//FLASH unlock key


u16  WIZFLASH_ReadHalfWord(u32 faddr);                               //Read half word
void WIZFLASH_WriteLenByte(u32 WriteAddr, u32 DataToWrite, u16 Len); //The specified address starts writing data of the specified length
u32  WIZFLASH_ReadLenByte(u32 ReadAddr, u16 Len);                    //The specified address starts reading the specified length data
void WIZFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite);    //Write data of the specified length starting from the specified address
void WIZFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead);       //Read data of the specified length starting from the specified address

//test write
void Test_Write(u32 WriteAddr, u16 WriteData);
#endif


