#ifndef __FLASH_H
#define __FLASH_H
#include "w55mh32.h"

//W25X series/Q series chip list
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16
//W25Q128 ID  0XEF17
#define W25Q80  0XEF13
#define W25Q16  0XEF14
#define W25Q32  0XEF15
#define W25Q64  0XEF16
#define W25Q128 0XEF17

extern u16 W25QXX_TYPE;    //Define the W25QXX chip model

#define W25QXX_CS PAout(4) //W25QXX chip selection signal

////////////////////////////////////////////////////////////////////////////
//IO port operation macro definition
#define BITBAND(addr, bitnum)  ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr)         *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))
//IO port address mapping
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) //0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) //0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) //0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) //0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) //0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) //0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) //0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8)  //0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8)  //0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8)  //0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8)  //0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8)  //0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8)  //0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8)  //0x40011E08

//IO port operation, only for a single IO port!
//Make sure the value of n is less than 16!
#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n)
#define PAin(n)  BIT_ADDR(GPIOA_IDR_Addr, n)

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n)  BIT_ADDR(GPIOB_IDR_Addr, n)

#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n)
#define PCin(n)  BIT_ADDR(GPIOC_IDR_Addr, n)

#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n)
#define PDin(n)  BIT_ADDR(GPIOD_IDR_Addr, n)

#define PEout(n) BIT_ADDR(GPIOE_ODR_Addr, n)
#define PEin(n)  BIT_ADDR(GPIOE_IDR_Addr, n)

#define PFout(n) BIT_ADDR(GPIOF_ODR_Addr, n)
#define PFin(n)  BIT_ADDR(GPIOF_IDR_Addr, n)

#define PGout(n) BIT_ADDR(GPIOG_ODR_Addr, n)
#define PGin(n)  BIT_ADDR(GPIOG_IDR_Addr, n)

//instruction list
#define W25X_WriteEnable      0x06
#define W25X_WriteDisable     0x04
#define W25X_ReadStatusReg    0x05
#define W25X_WriteStatusReg   0x01
#define W25X_ReadData         0x03
#define W25X_FastReadData     0x0B
#define W25X_FastReadDual     0x3B
#define W25X_PageProgram      0x02
#define W25X_BlockErase       0xD8
#define W25X_SectorErase      0x20
#define W25X_ChipErase        0xC7
#define W25X_PowerDown        0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID         0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID    0x9F

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);        //Read flash ID
u8   W25QXX_ReadSR(void);        //Read state register
void W25QXX_Write_SR(u8 sr);     //Write status register
void W25QXX_Write_Enable(void);  //write enable
void W25QXX_Write_Disable(void); //write protection
void W25QXX_Write_NoCheck(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void W25QXX_Read(u8 *pBuffer, u32 ReadAddr, u16 NumByteToRead);
void W25QXX_Write(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void W25QXX_Erase_Chip(void);           //Whole piece erase
void W25QXX_Erase_Sector(u32 Dst_Addr); //sector erase
void W25QXX_Wait_Busy(void);            //Wait for idle
void W25QXX_PowerDown(void);            //Enter power-down mode
void W25QXX_WAKEUP(void);               //wake up
#endif
