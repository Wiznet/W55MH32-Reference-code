#ifndef __WIZ_CRYPT_H
#define __WIZ_CRYPT_H

#ifdef __cplusplus
 extern "C" {
#endif
     
#include <stdint.h>
#include <string.h>
#include "w55mh32.h"

void WIZCRYPT_SYMCITClear(void);

void WIZCRYPT_ASYMCITClear(void);

void WIZCRYPT_SetBusKey(uint32_t pu32Key[2]);

void WIZCRYPT_CramInit(void);

void WIZCRYPT_CramKeySet(uint32_t pu32Key[2]);

void WIZCRYPT_CramKeyReset(void);

void WIZCRYPT_SecureEnable(void);

void WIZCRYPT_SecureDisable(void);

typedef struct
{
    uint8_t *pu8In;
    uint8_t *pu8Out;
    uint8_t *pu8IV;
    uint8_t *pu8IVout;
    uint8_t *pu8Key;
    uint32_t u32InLen;
    uint32_t u32OutLen;
    uint16_t u16Opt;
    uint32_t u32Crc;
} WIZ_SYM_CRYPT_CALL;

#ifdef __cplusplus
}
#endif

#endif // __WIZ_CRYPT_H
