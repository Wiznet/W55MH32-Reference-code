/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __WIZ_H
#define __WIZ_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "w55mh32.h"

uint32_t WIZ_RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul, uint8_t Latency);
uint32_t WIZ_LSE_DriveConfig(uint8_t gear);
uint32_t WIZ_PLL_SelLockConfig(uint8_t mode);
uint32_t WIZ_PLL_CpConfig(uint8_t mode);

#endif

