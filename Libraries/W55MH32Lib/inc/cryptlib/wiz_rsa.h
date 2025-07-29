#ifndef __WIZ_RSA_H
#define __WIZ_RSA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wiz_bignum.h"

#define WIZ_RET_RSA_INIT                      (0x52a659AA)
#define WIZ_RET_RSA_PARAM_ERR                 (WIZ_RET_RSA_INIT + 0x01)
#define WIZ_RET_RSA_TRNG_ERR                  (WIZ_RET_RSA_INIT + 0x02) 
#define WIZ_RET_RSA_ATTACKED                  (WIZ_RET_RSA_INIT + 0x03)
#define WIZ_RET_RSA_ERR                       (WIZ_RET_RSA_INIT + 0x04) 
#define WIZ_RET_RSA_DFA_LOCK                  (WIZ_RET_RSA_INIT + 0x05)
#define WIZ_RET_PRIME_IS_PRIME                (WIZ_RET_RSA_INIT + 0x06)
#define WIZ_RET_PRIME_INPUT_PARA_ERR          (WIZ_RET_RSA_INIT + 0x07)
#define WIZ_RET_RSA_GCD_ISNOT_ONE             (WIZ_RET_RSA_INIT + 0x08)
#define WIZ_RET_PRIME_NOT_ACCEPTABLE          (WIZ_RET_RSA_INIT + 0x09)

#define WIZ_RET_RSA_SUCCESS                   (WIZ_RET_RSA_INIT + 0x0100)

typedef struct
{
    //Key info
    uint16_t u16NBits;      //Public key N bits
    uint16_t u16EBits;      //Public key E bits

    //Pub key
    uint8_t *pu8N;            //len : (Nbits + 7)>>3
    uint8_t *pu8E;            //len : (Ebits + 7)>>3
    uint8_t *pu8D;            //len : (Nbits + 7)>>3

    //CRT key
    uint8_t *pu8P;        //len : (Nbits + 15)>>4
    uint8_t *pu8Q;        //len : (Nbits + 15)>>4
    uint8_t *pu8Dp;       //len : (Nbits + 15)>>4
    uint8_t *pu8Dq;       //len : (Nbits + 15)>>4
    uint8_t *pu8Qi;       //len : (Nbits + 15)>>4

    uint32_t u32Crc;
} WIZ_RSA_KEY;

typedef struct
{
    uint32_t au32InnerKey[12];
} WIZ_RSA_KEY_SPACE;


typedef struct
{
    //Call parameters
    uint8_t *pu8Input;
    uint8_t *pu8Output;
    uint32_t u32InLen;
    uint32_t u32OutLen;
    WIZ_RSA_KEY_SPACE  *KeyBase;
    uint32_t u32Crc;
} WIZ_RSA_CALL;


#define WIZ_RSA_COMPLETE_E                    (0)
#define WIZ_RSA_COMPLETE_D                    (1)
#define WIZ_RSA_COMPLETE_CRT                  (2)
#define IS_RSA_COMPLETE_TYPE(TYPE)          (((TYPE) == WIZ_RSA_COMPLETE_E) || \
                                             ((TYPE) == WIZ_RSA_COMPLETE_D) || \
                                            ((TYPE) == WIZ_RSA_COMPLETE_CRT))

#define WIZ_RSA_GENKEY_EFC                    (0)
#define WIZ_RSA_GENKEY_GENE                   (1)
#define IS_RSA_GENKEY_TYPE(TYPE)          (((TYPE) == WIZ_RSA_GENKEY_EFC) || \
                                          ((TYPE) == WIZ_RSA_GENKEY_GENE))

uint32_t WIZRSA_Init(WIZ_RSA_KEY *pKeySrc, WIZ_RSA_KEY_SPACE *pKeyBase);

uint32_t WIZRSA_SetKey(WIZ_RSA_KEY *pKeySrc, WIZ_RSA_KEY_SPACE *pKeyBase, uint8_t *pu8Base);

uint32_t WIZRSA_GenKey(WIZ_RSA_KEY *pKeySrc, WIZ_RSA_KEY_SPACE *pKeyBase, uint8_t *pu8Base,  uint32_t u32GenkeyType);

uint32_t WIZRSA_Pri(WIZ_RSA_CALL *pCall);

uint32_t WIZRSA_Pub(WIZ_RSA_CALL *pCall);

uint32_t WIZRSA_Crt(WIZ_RSA_CALL *pCall);

uint32_t WIZRSA_CompleteKey(WIZ_RSA_KEY *pKeySrc, WIZ_RSA_KEY_SPACE *pKeyBase, uint8_t *pu8Base, uint32_t u32CompType);



#ifdef __cplusplus
}
#endif

#endif
