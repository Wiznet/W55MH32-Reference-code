#ifndef __I2C_EE_H
#define __I2C_EE_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"
#include "wizchip_conf.h"
#include "wiz_interface.h"


#include <inttypes.h>

#define EEPROM_I2C_WR 0 /* write control bit */
#define EEPROM_I2C_RD 1 /* Read control bit */


/* Define the GPIO port of the I2C bus connection, the user only needs to modify the following 4 lines of code to arbitrarily change the pins of SCL and SDA */
#define EEPROM_GPIO_PORT_I2C GPIOB                                                    /* GPIO port */
#define EEPROM_RCC_I2C_PORT  RCC_APB2Periph_GPIOB                                     /* GPIO port clock */
#define EEPROM_I2C_SCL_PIN   GPIO_Pin_6                                               /* GPIO port clock */
#define EEPROM_I2C_SDA_PIN   GPIO_Pin_7                                               /* GPIO port clock */

#define EEPROM_I2C_SCL_1() EEPROM_GPIO_PORT_I2C->BSRR = EEPROM_I2C_SCL_PIN            /* SCL = 1 */
#define EEPROM_I2C_SCL_0() EEPROM_GPIO_PORT_I2C->BRR = EEPROM_I2C_SCL_PIN             /* SCL = 0 */

#define EEPROM_I2C_SDA_1() EEPROM_GPIO_PORT_I2C->BSRR = EEPROM_I2C_SDA_PIN            /* SDA = 1 */
#define EEPROM_I2C_SDA_0() EEPROM_GPIO_PORT_I2C->BRR = EEPROM_I2C_SDA_PIN             /* SDA = 0 */

#define EEPROM_I2C_SDA_READ() ((EEPROM_GPIO_PORT_I2C->IDR & EEPROM_I2C_SDA_PIN) != 0) /* Read SDA port status */

void    i2c_Start(void);
void    i2c_Stop(void);
void    i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void    i2c_Ack(void);
void    i2c_NAck(void);

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* AT24C01/02 Each page has eight bytes. 
 * AT24C04/08A/16A Each page has 16 bytes 
 */


#define EEPROM_DEV_ADDR  0xA0 /* 24xx02 device address */
#define EEPROM_PAGE_SIZE 8    /* 24xx02 page size */
#define EEPROM_SIZE      256  /* 24xx02 Total Capacity */


void    i2c_CfgGpio(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_CheckDevice(uint8_t _Address);
void    ee_Erase(void);
uint8_t ee_WaitStandby(void);
uint8_t check_eeprom_network_info(wiz_NetInfo *net_info);


#endif /* __I2C_EE_H */
