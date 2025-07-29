#ifndef _BSP_I2C_GPIO_H
#define _BSP_I2C_GPIO_H

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

void    i2c_CfgGpio(void);
void    i2c_Start(void);
void    i2c_Stop(void);
void    i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(void);
uint8_t i2c_WaitAck(void);
void    i2c_Ack(void);
void    i2c_NAck(void);


#endif

