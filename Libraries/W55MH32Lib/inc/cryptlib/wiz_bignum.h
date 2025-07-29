#ifndef __WIZ_BIGNUM_H
#define __WIZ_BIGNUM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wiz_crypt.h"
 
#define WIZ_RET_CRYPT_INIT                       (0x508B39AA)    
#define WIZ_RET_CRYPT_PARAM_ERR                  (WIZ_RET_CRYPT_INIT+1)   
#define WIZ_RET_CRYPT_FUNCTION_ID_ERR            (WIZ_RET_CRYPT_INIT+2)
#define WIZ_RET_CRYPT_LENGTH_ERR                 (WIZ_RET_CRYPT_INIT+3)
#define WIZ_RET_CRYPT_GCD_ISNOT_ONE              (WIZ_RET_CRYPT_INIT+4)
#define WIZ_RET_CRYPT_SUCCESS_WITH_OVERFLOW      (WIZ_RET_CRYPT_INIT+5)
#define WIZ_RET_CRYPT_SUCCESS                    (WIZ_RET_CRYPT_INIT + 0x0100) 

typedef struct
{
    uint8_t *pu8Num;
    uint32_t u32Len;
}WIZBN_BIGNUM;


uint32_t WIZBN_BnAdd(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB);

uint32_t WIZBN_BnSub(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB);

uint32_t WIZBN_BnModAdd(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB, WIZBN_BIGNUM *pBigP);

uint32_t WIZBN_BnModSub(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB, WIZBN_BIGNUM *pBigP);

uint32_t WIZBN_BnMul(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB);

uint32_t WIZBN_BnModMul(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB, WIZBN_BIGNUM *pBigP);

uint32_t WIZBN_BnDiv(WIZBN_BIGNUM *pBigQ, WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB);

uint32_t WIZBN_BnModInv(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB);

uint32_t WIZBN_BnModExp(WIZBN_BIGNUM *pBigR, WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB, WIZBN_BIGNUM *pBigP);

uint32_t WIZBN_BnCmp(WIZBN_BIGNUM *pBigA, WIZBN_BIGNUM *pBigB);

uint32_t WIZBN_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif
