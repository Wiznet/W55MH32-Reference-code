#ifndef __WIZ_ECC_DEFINE_H
#define __WIZ_ECC_DEFINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "wiz_bignum.h"

#define     WIZ_ECC192_SPACE_LEN     (156)
#define     WIZ_ECC224_SPACE_LEN     (168)
#define     WIZ_ECC256_SPACE_LEN     (180)
#define     WIZ_ECC384_SPACE_LEN     (228)
#define     WIZ_ECC521_SPACE_LEN     (282)

typedef struct
{
    uint16_t u16BitLen;			  
    uint8_t *pu8A;                
    uint8_t *pu8B;                                           
    uint8_t *pu8Gx;               
    uint8_t *pu8Gy;                                               
    uint8_t *pu8N;                                              
    uint8_t *pu8P;               
    uint8_t *pu8Space;            //for inner space
    uint32_t u32SpaceLen;         //inner space length
    uint32_t u32Crc;                            
} WIZECC_PARAM;   
    
  
typedef struct
{
    uint16_t u16Bits;
    uint8_t *pu8PubX;
    uint8_t *pu8PubY;   
    uint8_t *pu8D;
    uint32_t u32Crc;
} WIZECC_KEY;

typedef struct
{
    uint8_t *pu8X;
    uint8_t *pu8Y;
}WIZECC_POINT;

    
#define WIZ_RET_ECC_INIT                      (0x53c2d9AA)      
#define WIZ_RET_ECC_PARAM_ERR                 (WIZ_RET_ECC_INIT + 0x01)
#define WIZ_RET_ECC_TRNG_ERR                  (WIZ_RET_ECC_INIT + 0x02)
#define WIZ_RET_ECC_VERIFY_KEY_FAILURE        (WIZ_RET_ECC_INIT + 0x03)
#define WIZ_RET_ECC_VERIFY_CURVE_FAILURE      (WIZ_RET_ECC_INIT + 0x04)
#define WIZ_RET_ECC_VERIFY_POINT_FAILURE      (WIZ_RET_ECC_INIT + 0x05)
#define WIZ_RET_ECC_INFI_FAR_VERIFY_FAILURE   (WIZ_RET_ECC_INIT + 0x06)
#define WIZ_RET_ECC_VERIFY_SIGN_FAILURE       (WIZ_RET_ECC_INIT + 0x07)
#define WIZ_RET_ECC_FAILURE                   (WIZ_RET_ECC_INIT + 0x08)
#define WIZ_RET_ECC_ERR                       (WIZ_RET_ECC_INIT + 0x09)
#define WIZ_RET_ECC_DFA_LOCK                  (WIZ_RET_ECC_INIT + 0x0A)
#define WIZ_RET_ECC_SHA_PARAM_ERR             (WIZ_RET_ECC_INIT + 0x0C)
#define WIZ_RET_ECC_SHA_BUSY                  (WIZ_RET_ECC_INIT + 0x0D)
#define WIZ_RET_ECC_POINT_INIT                (WIZ_RET_ECC_INIT + 0x10)
#define WIZ_RET_ECC_PONIT_RANG_CHECK_FAILURE  (WIZ_RET_ECC_INIT + 0x11) 
#define WIZ_RET_ECC_POINT_SUCCESS             (WIZ_RET_ECC_INIT + 0x200)  
#define WIZ_RET_ECC_VERIFY_SUCCESS            (WIZ_RET_ECC_INIT + 0x300)
#define WIZ_RET_ECC_SUCCESS                   (WIZ_RET_ECC_INIT + 0x0100)

#ifdef __cplusplus
}
#endif

#endif
