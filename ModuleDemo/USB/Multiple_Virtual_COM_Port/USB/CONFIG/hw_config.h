


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "usb_type.h"
#include "delay.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define MASS_MEMORY_START    0x04002000
#define BULK_MAX_PACKET_SIZE 0x00000040
#define LED_ON               0xF0
#define LED_OFF              0xFF
#define DOUBLE_BUF
//#define CTR_TEST
/* Exported functions ------------------------------------------------------- */
void     Set_System(void);
void     Set_USBClock(void);
void     Enter_LowPowerMode(void);
void     Leave_LowPowerMode(void);
void     USB_Interrupts_Config(void);
void     USB_Cable_Config(FunctionalState NewState);
void     Get_SerialNum(void);
void     LCD_Control(void);
uint32_t CDC_Send_DATA(uint8_t *ptrBuffer, uint8_t Send_length);
/* External variables --------------------------------------------------------*/

#endif /*__HW_CONFIG_H*/
