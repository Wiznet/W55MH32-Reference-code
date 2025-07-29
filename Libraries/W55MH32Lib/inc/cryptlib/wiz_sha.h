#ifndef __WIZ_SHA_H
#define __WIZ_SHA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wiz_crypt.h"


#define WIZ_RET_SHA_INIT                         (0x576C39AA)
#define WIZ_RET_SHA_PARAM_ERR                    ((WIZ_RET_SHA_INIT + 0x01))
#define WIZ_RET_SHA_BUSY                         ((WIZ_RET_SHA_INIT + 0x02))
#define WIZ_RET_SHA_MODE_ERR                     ((WIZ_RET_SHA_INIT + 0x03))
#define WIZ_RET_SHA_SUCCESS                      ((WIZ_RET_SHA_INIT + 0x0100))


typedef struct
{
    uint32_t au32Total[2];
    uint8_t au8Buffer[64];
    uint32_t au32State[5];
}WIZSHA1_Context;

typedef struct
{
    uint32_t au32Total[2];
    uint8_t  au8Buffer[64];
    uint32_t au32State[8];
}WIZSHA2_Context;

typedef enum
{
    WIZSHA1 = 0,
    WIZSHA224 = 1,
    WIZSHA256 = 2,
//    WIZSHA384 = 3,
//    WIZSHA512 = 4
}WIZSHA_MOD_TYPE;

uint32_t WIZSHA1_Starts(WIZSHA1_Context *pCtx);
uint32_t WIZSHA1_Update(WIZSHA1_Context *pCtx, const uint8_t *pu8Input, uint32_t u32InputLen);
uint32_t WIZSHA1_Finish(WIZSHA1_Context *pCtx, uint8_t au8Digest[20]);

uint32_t WIZSHA256_Starts(WIZSHA2_Context *pCtx);
uint32_t WIZSHA256_Update(WIZSHA2_Context *pCtx, const uint8_t *pu8Input, uint32_t u32InputLen);
uint32_t WIZSHA256_Finish(WIZSHA2_Context *pCtx, uint8_t au8Digest[32]);

uint32_t WIZSHA_Sha(WIZSHA_MOD_TYPE ShaType, const uint8_t *pu8Input, uint32_t u32Ibytes, uint8_t *pu8Output, uint32_t u32Outlen);

#ifdef __cplusplus
}
#endif  

#endif
