/* des.h - header file for des.c
 */

#ifndef __WIZ_DES_H
#define __WIZ_DES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wiz_crypt.h"


#define WIZ_RET_DES_INIT                         (0x55D659AA)
#define WIZ_RET_DES_PARAM_ERR                    (WIZ_RET_DES_INIT + 0x1)
#define WIZ_RET_DES_TRNG_ERR                     (WIZ_RET_DES_INIT + 0x2)
#define WIZ_RET_DES_PACK_MODE_ERROR              (WIZ_RET_DES_INIT + 0x3)
#define WIZ_RET_DES_INPUT_SIZE_ERROR             (WIZ_RET_DES_INIT + 0x4)
#define WIZ_RET_DES_MODE_ERROR                   (WIZ_RET_DES_INIT + 0x5)
#define WIZ_RET_DES_ATTACKED                     (WIZ_RET_DES_INIT + 0x6)
#define WIZ_RET_DES_OUTBUF_TOO_SMALL             (WIZ_RET_DES_INIT + 0x7)
#define WIZ_RET_DES_DFA_LOCK                     (WIZ_RET_DES_INIT + 0x8)
#define WIZ_RET_DES_KEY_SIZE_ERROR               (WIZ_RET_DES_INIT + 0x9)
#define WIZ_RET_DES_SUCCESS                      (WIZ_RET_DES_INIT + 0x0100)



#define WIZ_DES_OPT_MODE_Mask        (BIT(0))         
#define WIZ_DES_OPT_MODE_ENCRYPT     (0)
#define WIZ_DES_OPT_MODE_DECRYPT     (BIT(0))
#define WIZ_DES_OPT_BLK_Mask         (BIT(1)|BIT(2))
#define WIZ_DES_OPT_BLK_CBC          (BIT(1))
#define WIZ_DES_OPT_BLK_ECB          (BIT(2))
#define WIZ_DES_OPT_BLK_CBC_MAC      (BIT(1)|BIT(2))

#define WIZ_TDES_OPT_KEY_LEN_Mask    (BIT(3))
#define WIZ_TDES_OPT_KEY_2           (0)
#define WIZ_TDES_OPT_KEY_3           (BIT(3))

uint32_t WIZDES_EncDec(WIZ_SYM_CRYPT_CALL *pCall);

uint32_t WIZTDES_EncDec(WIZ_SYM_CRYPT_CALL *pCall);

#ifdef __cplusplus
}
#endif

#endif
