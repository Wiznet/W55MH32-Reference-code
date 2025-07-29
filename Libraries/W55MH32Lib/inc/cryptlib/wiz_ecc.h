#ifndef __WIZ_ECC_H
#define __WIZ_ECC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "wiz_bignum.h"
#include "wiz_crypt.h"
#include "wiz_ecc_define.h"

typedef struct
{
    uint32_t au32InBase[2];
}WIZ_ECC_INNER_BASE;

typedef struct
{
    //Call parameters
    uint8_t *pu8Input;
    uint8_t *pu8Output;
    uint8_t *pu8SharedMsg1;
    uint8_t *pu8SharedMsg2;
    uint16_t u16InLen;
    uint16_t u16OutLen;
    uint16_t u16M1Len;
    uint16_t u16M2Len;
    uint16_t u16MackeyLen;
    WIZ_ECC_INNER_BASE *pInBase;
    WIZECC_PARAM *pParam;
    uint32_t u32Crc;
} WIZECC_ENC_DEC_CALL;

typedef struct
{
    uint8_t *pu8SignR;
    uint8_t *pu8SignS;
}WIZECC_SIGN;


typedef struct
{
    //Call parameters
    uint8_t *pu8Msg;
    uint8_t *pu8E;
    uint32_t u32MLen;
    uint16_t u16ELen;
    WIZECC_SIGN *pu8Sign;
    WIZ_ECC_INNER_BASE *pInBase;
    WIZECC_PARAM *pParam;
    uint32_t u32Crc;
} WIZECC_SIG_VER_CALL;

typedef struct
{
    //Call parameters
    uint8_t *pu8KeyOut;
    WIZ_ECC_INNER_BASE *pInBaseU;
    WIZ_ECC_INNER_BASE *pInBaseV;
    WIZECC_PARAM *pParam;
    uint32_t u32Crc;
} WIZECC_ECDH_CALL;


uint32_t WIZECC_ParamInit(WIZECC_PARAM *pEccPara);

uint32_t WIZECC_SetKey(WIZECC_KEY *pKeySrc, WIZ_ECC_INNER_BASE *pInBase, uint8_t *pu8Base, WIZECC_PARAM *pEccPara);

uint32_t WIZECC_HashE(WIZECC_SIG_VER_CALL *pCall);

uint32_t WIZECC_EcdsaSign(WIZECC_SIG_VER_CALL *pCall);

uint32_t WIZECC_EcdsaVerify(WIZECC_SIG_VER_CALL *pCall);

uint32_t WIZECC_ECDH(WIZECC_ECDH_CALL *pCall);

uint32_t WIZECC_EciesEnc(WIZECC_ENC_DEC_CALL *pCall);

uint32_t WIZECC_EciesDec(WIZECC_ENC_DEC_CALL *pCall);

uint32_t WIZECC_GenKey(WIZECC_KEY *pKey, WIZECC_PARAM *pEccPara);

uint32_t WIZECC_CompleteKey(WIZECC_KEY *pKey,  WIZECC_PARAM *pEccPara);

uint32_t WIZECC_PointMul(WIZECC_POINT *pPointR, WIZECC_POINT *pPointA, WIZBN_BIGNUM *pK, WIZECC_PARAM *pParam);

uint32_t WIZECC_PointAdd(WIZECC_POINT *pPointR, WIZECC_POINT *pPointA, WIZECC_POINT *pPointB, WIZECC_PARAM *pParam);

#ifdef __cplusplus
}
#endif

#endif

