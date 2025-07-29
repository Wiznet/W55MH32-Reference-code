
#ifndef __WIZ_SMRAND_H
#define __WIZ_SMRAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wiz_crypt.h"

#define WIZ_RET_SMRAND_INIT                       (0x5D6c39AA)
#define WIZ_RET_SMRAND_PARAM_ERR                  (WIZ_RET_SMRAND_INIT + 1)
#define WIZ_RET_SMRAND_CHECK_FAILURE              (WIZ_RET_SMRAND_INIT + 2)
#define WIZ_RET_SMRAND_CHECK_SUCCESS              (WIZ_RET_SMRAND_INIT + 0X0100)

uint32_t Rand_Check(uint8_t *pu8Src, uint32_t u32Len, uint32_t u32Mask);

#ifdef __cplusplus
}
#endif

#endif /*  __WIZ_SMRAND_H  */
