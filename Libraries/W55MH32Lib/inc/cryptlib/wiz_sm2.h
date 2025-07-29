#ifndef __WIZ_SM2_H
#define __WIZ_SM2_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "wiz_misc.h"
#include "wiz_rand.h"

#define WIZ_SM2_KEY_BITS 		256
#define WIZ_SM2_KEY_BYTES		((WIZ_SM2_KEY_BITS + 7) / 8)
#define WIZ_SM2_KEY_WORDS		((WIZ_SM2_KEY_BYTES + 3) / 4)

typedef struct
{
    uint8_t *pu8PubX;
    uint8_t *pu8PubY;
    uint8_t *pu8D;
    uint32_t u32Crc;
} WIZ_SM2_KEY;

typedef struct
{
    uint32_t au32Space[2];
}WIZ_SM2_INNER_BASE;

typedef struct
{
    //Call parameters
    uint8_t *pu8Input;
    uint8_t *pu8Output;
    uint32_t u32InLen;
    uint32_t u32OutLen;
    WIZ_SM2_INNER_BASE *pInBase;
    uint32_t u32Crc;
} WIZ_SM2_ENC_DEC_CALL;


typedef struct
{
    //Call parameters
    uint8_t *pu8Msg;
    uint8_t *pu8IDa;
    uint8_t *pu8E;
    uint32_t u32MLen;
    uint32_t u32IDaLen;
    uint8_t *pu8Sign;
    WIZ_SM2_INNER_BASE *pInBase;
    WIZ_SM2_KEY  *pSm2Key;
    uint32_t u32Crc;
} WIZ_SM2_SIG_VER_CALL;

uint32_t WIZSM2_GenKey(WIZ_SM2_KEY *pKey);
uint32_t WIZSM2_CompleteKey(WIZ_SM2_KEY *pKey);
uint32_t WIZSM2_SetKey(WIZ_SM2_KEY *pKeySrc, WIZ_SM2_INNER_BASE *pInBase, uint8_t *pu8Base);
uint32_t WIZSM2_Enc(WIZ_SM2_ENC_DEC_CALL *pCall);
uint32_t WIZSM2_Dec(WIZ_SM2_ENC_DEC_CALL *pCall);
uint32_t WIZSM2_HashE(WIZ_SM2_SIG_VER_CALL *pCall);
uint32_t WIZSM2_SignE(WIZ_SM2_SIG_VER_CALL *pCall);
uint32_t WIZSM2_VeriE(WIZ_SM2_SIG_VER_CALL *pCall);
uint32_t WIZSM2_Sign(WIZ_SM2_SIG_VER_CALL *pCall);
uint32_t WIZSM2_Veri(WIZ_SM2_SIG_VER_CALL *pCall);
uint32_t WIZSM2_VerifyKeyPair(WIZ_SM2_KEY *pKey);

#define WIZ_RET_SM2_INIT                     (0x5A8379AA)
#define WIZ_RET_SM2_PARAM_ERR                (WIZ_RET_SM2_INIT + 0x01)
#define WIZ_RET_SM2_TRNG_ERR                 (WIZ_RET_SM2_INIT + 0x02)
#define WIZ_RET_SM2_FAILURE                  (WIZ_RET_SM2_INIT + 0x03)
#define WIZ_RET_SM2_VERIFY_SIGN_FAILURE      (WIZ_RET_SM2_INIT + 0x04)
#define WIZ_RET_SM2_VERIFY_KEY_FAILURE       (WIZ_RET_SM2_INIT + 0x05)
#define WIZ_RET_SM2_VERIFY_POINT_FAILURE     (WIZ_RET_SM2_INIT + 0x06)
#define WIZ_RET_SM2_DFA_LOCK                 (WIZ_RET_SM2_INIT + 0x07)
#define WIZ_RET_SM2_ERR                      (WIZ_RET_SM2_INIT + 0x08)
#define WIZ_RET_SM2_KDF_FAILURE              (WIZ_RET_SM2_INIT + 0x09)
#define WIZ_RET_SM2_SUCCESS                  (WIZ_RET_SM2_INIT + 0x0100)

#ifdef __cplusplus
}
#endif

#endif 

