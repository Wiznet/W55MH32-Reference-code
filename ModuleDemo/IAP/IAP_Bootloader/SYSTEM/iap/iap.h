#ifndef __IAP_H__
#define __IAP_H__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"

typedef void (*iapfun)(void);         //Defines a parameter of a function type.

#define FLASH_APP1_ADDR 0x08008000    //First application start address (stored in Flash) \
                                      //Reserve the space of 0X08000000~ 0X08007FFF for IAP use (32K)

#define W55MH32_FLASH_BASE 0x08000000 //FLASH starting address
#define FLASH_SECTOR_SIZE  (2048)     //Byte

void IAP_Write_Appbin(uint32_t appxaddr, uint8_t *appbuf, uint32_t appsize);
void IAP_Load_App(uint32_t appxaddr);
#endif
