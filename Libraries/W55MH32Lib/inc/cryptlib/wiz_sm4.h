#ifndef __WIZ_SM4_H
#define __WIZ_SM4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdint.h>
#include "wiz_crypt.h"
    
#define WIZ_SM4_OPT_MODE_Mask        (BIT(0))         
#define WIZ_SM4_OPT_MODE_ENCRYPT     (0)
#define WIZ_SM4_OPT_MODE_DECRYPT     (BIT(0))

#define WIZ_SM4_OPT_BLK_Mask         (BIT(1)|BIT(2))
#define WIZ_SM4_OPT_BLK_CBC          (BIT(1))
#define WIZ_SM4_OPT_BLK_ECB          (BIT(2))
#define WIZ_SM4_OPT_BLK_CBC_MAC      (BIT(1)|BIT(2))





#define WIZ_RET_SM4_INIT                         (0x584E69AA)
#define WIZ_RET_SM4_PARAM_ERR                    (WIZ_RET_SM4_INIT + 0x01)
#define WIZ_RET_SM4_TRNG_ERR                     (WIZ_RET_SM4_INIT + 0x02)
#define WIZ_RET_SM4_PACK_MODE_ERROR              (WIZ_RET_SM4_INIT + 0x03)
#define WIZ_RET_SM4_OUTBUF_TOO_SMALL             (WIZ_RET_SM4_INIT + 0x04)
#define WIZ_RET_SM4_INPUT_SIZE_ERROR             (WIZ_RET_SM4_INIT + 0x05)
#define WIZ_RET_SM4_ATTACKED                     (WIZ_RET_SM4_INIT + 0x06)
#define WIZ_RET_SM4_MODE_ERROR                   (WIZ_RET_SM4_INIT + 0x07)
#define WIZ_RET_SM4_SUCCESS                      (WIZ_RET_SM4_INIT + 0x0100)

uint32_t WIZSM4_EncDec(WIZ_SYM_CRYPT_CALL *pCall);


#endif


#ifdef __cplusplus
}
#endif

