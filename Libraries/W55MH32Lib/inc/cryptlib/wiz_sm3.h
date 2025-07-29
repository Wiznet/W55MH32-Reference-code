#ifndef __WIZ_SM3_H
#define __WIZ_SM3_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdint.h>

#define WIZ_RET_SM3_INIT                     (0x594F29AA)
#define WIZ_RET_SM3_PARAM_ERR                (WIZ_RET_SM3_INIT + 0x01)
#define WIZ_RET_SM3_BUSY                     (WIZ_RET_SM3_INIT + 0x02)
#define WIZ_RET_SM3_SUCCESS                  (WIZ_RET_SM3_INIT + 0x0100)  

typedef struct
{
    uint32_t    au32Total[2];
    uint8_t     au8Buffer[64];
    uint32_t    au32State[8];
} WIZSM3_Context;

/**
  * @method	wiz_sm3
  * @brief	SM3 Hash Check
  * @param	output		:output data buffer
  * @param	input		:input data buffer
  * @param	ibytes		:size of input data
  * @retval 			:SM3_SUCCESS or SM3_BUSY
  */
uint32_t WIZSM3_Cal(uint8_t au8Output[32], uint8_t *pcu8Input, uint32_t u32Size);
uint32_t WIZSM3_Starts(WIZSM3_Context *pCtx);
uint32_t WIZSM3_Update(WIZSM3_Context *pCtx, uint8_t *pcu8Input, uint32_t u32Ilen);
uint32_t WIZSM3_Finish(WIZSM3_Context *pCtx, uint8_t au8Digest[32]);
    
#ifdef __cplusplus
}
#endif

#endif
