

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "usb_type.h"
extern uint8_t  Receive_Buffer_port[64];
extern uint32_t Receive_length_port;
extern int      packet_sent;
extern uint32_t CDC_Send_DATA(uint8_t *ptrBuffer, uint8_t Send_length);
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void USB_Cable_Config(FunctionalState NewState);
void GPIO_Configuration(void);
void EXTI_Configuration(void);
void ADC_Configuration(void);
void ADC30x_Configuration(void);
void Get_SerialNum(void);
void Delay(__IO uint32_t nCount);


enum leds
{
    LED1 = 0,
    LED2,
    LED3,
    LED4,
};

enum button
{
    KEY1 = 0,
    KEY2,
};

void    WIZ_EVAL_LEDOn(uint8_t Led);
void    WIZ_EVAL_LEDOff(uint8_t Led);
uint8_t WIZ_EVAL_PBGetState(uint8_t Key);


#endif /*__HW_CONFIG_H*/
