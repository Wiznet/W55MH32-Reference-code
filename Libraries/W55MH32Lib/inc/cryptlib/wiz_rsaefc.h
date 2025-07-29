#ifndef __WIZ_RSAEFC_H
#define __WIZ_RSAEFC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wiz_rsa.h"
    
typedef struct
{
    //Key info
    uint16_t u16NBits;      //Public key N bits
    uint16_t u16EBits;      //Public key E bits
    //Pub key
    uint8_t *pu8N;
    uint8_t *pu8E;
    uint8_t *pu8ND;
    uint32_t *pu32NC;
    uint32_t u32Crc;
} WIZ_RSA_PUBKEY;    
    
uint32_t WIZRSA_SetPubEfc(WIZ_RSA_PUBKEY *pSrcPub, WIZ_RSA_KEY_SPACE *pKeyBase, uint8_t *pu8Base);    

uint32_t WIZRSA_CrtEfc(WIZ_RSA_CALL *pCall);

uint32_t WIZRSA_PubEfc(WIZ_RSA_CALL *pCall);

uint32_t WIZRSA_PriEfc(WIZ_RSA_CALL *pCall);


#ifdef __cplusplus
}
#endif

#endif
