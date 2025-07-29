#ifndef _WIZ_SM7_H_
#define _WIZ_SM7_H_

#include "wiz_crypt.h"

#define WIZ_SM7_OPT_MODE_Mask        (BIT(0))         
#define WIZ_SM7_OPT_MODE_ENCRYPT     (0)
#define WIZ_SM7_OPT_MODE_DECRYPT     (BIT(0))          
#define WIZ_SM7_OPT_BLK_Mask         (BIT(1)|BIT(2))
#define WIZ_SM7_OPT_BLK_CBC          (BIT(1))
#define WIZ_SM7_OPT_BLK_ECB          (BIT(2))
#define WIZ_SM7_OPT_BLK_CBC_MAC      (BIT(1)|BIT(2))

#define WIZ_RET_SM7_INIT                         (0x5C3479AA)
#define WIZ_RET_SM7_PARAM_ERR                    (WIZ_RET_SM7_INIT + 1)
#define WIZ_RET_SM7_MODE_ERROR                   (WIZ_RET_SM7_INIT + 2)
#define WIZ_RET_SM7_PACK_MODE_ERROR              (WIZ_RET_SM7_INIT + 3)
#define WIZ_RET_SM7_OUTBUF_TOO_SMALL             (WIZ_RET_SM7_INIT + 4)
#define WIZ_RET_SM7_INPUT_SIZE_ERROR             (WIZ_RET_SM7_INIT + 5)
#define WIZ_RET_SM7_ATTACKED                     (WIZ_RET_SM7_INIT + 6)
#define WIZ_RET_SM7_SUCCESS                      (WIZ_RET_SM7_INIT + 0x0100)


uint32_t WIZSM7_EncDec(WIZ_SYM_CRYPT_CALL *pCall);

#endif // _WIZ_SM7_H_
