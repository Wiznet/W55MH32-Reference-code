#ifndef __WIZ_MISC_H
#define __WIZ_MISC_H

#ifdef __cplusplus
    extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "wiz_crypt.h"


#define WIZ_CMP_INIT                 (0x00AA)
#define WIZ_CMP_EQUAL                (0x05AA)
#define WIZ_CMP_BIGGER               (0xA5AA)
#define WIZ_CMP_SMALLER              (0x25AA)
#define WIZ_CMP_ERR                  (0xAAAA)
#define WIZ_CMP_UNEQUAL              (0x85AA)
#define WIZ_RET_MISC_PARAM_ERR        (0x51B649AB)


uint32_t WIZMEM_SCopy(void *pvDst, void *pvSrc, uint32_t u32ByteLen, uint8_t *pu8Index);

uint32_t WIZMEM_SEqual(void *pvDst, void *pvSrc, uint32_t u32ByteLen, uint8_t *pu8Index);

uint32_t WIZMEM_SCopyBig(void *pvDst, void *pvSrc, uint32_t u32DstLen, uint32_t u32SrcLen, uint8_t *pu8Index);

uint32_t WIZMEM_CopyBig(void  *pvDst, void  *pvSrc, uint32_t u32DstLen, uint32_t u32SrcLen);

uint32_t WIZMEM_Equal(void *pvDst, void *pvSrc, uint32_t u32ByteLen);

    
#ifdef __cplusplus
}
#endif

#endif
