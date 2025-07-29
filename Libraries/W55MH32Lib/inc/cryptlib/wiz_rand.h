#ifndef __WIZ_RAND_H
#define __WIZ_RAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wiz_crypt.h"


#define WIZ_RET_RAND_INIT                         (0x562F79AA)
#define WIZ_RET_RAND_PARAM_ERR                    (WIZ_RET_RAND_INIT + 1)
#define WIZ_RET_RAND_FAILURE                      (WIZ_RET_RAND_INIT + 2)
#define WIZ_RET_RAND_LEN_ERR                      (WIZ_RET_RAND_INIT + 3)
#define WIZ_RET_RAND_TRNG_EMPTY                   (WIZ_RET_RAND_INIT + 4)
#define WIZ_RET_RAND_SUCCESS                      (WIZ_RET_RAND_INIT + 0X0100)

#define RAND_PARAM_ERRNO                         (-5500)
#define RAND_CHECK_ERRNO                         (-5501)

#define WIZ_RAND_BUFFER_OK               ((uint16_t)(0xA5A5UL))

uint32_t WIZRAND_Init(void *pu32Buff, uint32_t u32BuffSize);

uint32_t WIZRAND_Trand(void *pvDst, uint32_t u32Len, uint32_t *pu32CrcInit, uint32_t *pu32CrcEnd);

int32_t WIZRAND_TrandNoBlk(void *pvDst, uint32_t u32Len, uint32_t *pu32CrcInit, uint32_t *pu32CrcEnd);

uint32_t WIZRAND_Prand(void *pvDst, uint32_t u32Len);

uint32_t WIZCRC_CalcBuff(uint16_t u16CrcInit, void *pvData, uint32_t u32Size);

uint32_t WIZRAND_UpdatePSeed(void);

#ifdef __cplusplus
}
#endif

#endif


