/**
  ******************************************************************************
  * @file    GPIO/IOToggle/w55mh32_conf.h 
  * @author  none
  * @version V3.5.0
  * @date    17-Oct-2024
  * @brief   Library configuration file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WIZnet SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2024 WIZnet</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W55MH32_CONF_H
#define __W55MH32_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment/Comment the line below to enable/disable peripheral header file inclusion */
#include "w55mh32_adc.h"
#include "w55mh32_bkp.h"
#include "w55mh32_can.h"
#include "w55mh32_crc.h"
#include "w55mh32_dac.h"
#include "w55mh32_dbgmcu.h"
#include "w55mh32_dma.h"
#include "w55mh32_exti.h"
#include "w55mh32_flash.h"
#include "w55mh32_gpio.h"
#include "w55mh32_i2c.h"
#include "w55mh32_iwdg.h"
#include "w55mh32_pwr.h"
#include "w55mh32_rcc.h"
#include "w55mh32_rtc.h"
#include "w55mh32_sdio.h"
#include "w55mh32_spi.h"
#include "w55mh32_tim.h"
#include "w55mh32_usart.h"
#include "w55mh32_wwdg.h"
#include "misc.h" /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

/* Exported macro ------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT

/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
void assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __w55mh32_CONF_H */

/******************* (C) COPYRIGHT 2024 WIZnet *****END OF FILE****/
