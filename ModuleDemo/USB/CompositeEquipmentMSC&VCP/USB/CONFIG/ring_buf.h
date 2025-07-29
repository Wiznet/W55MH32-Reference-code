#ifndef __RING_BUF_H_
#define __RING_BUF_H_

#include "string.h"
#include "stdint.h"

#define RING_BUF_SIZE 1024
typedef struct buf_data
{
    uint16_t u16Head;
    uint16_t u16Tail;
    uint8_t  aucBuf[RING_BUF_SIZE];
} ring_buf_t;


uint16_t Buf_Get_Used_Size(ring_buf_t *p_Loop_Data);
uint16_t Buf_Get_Remain_Size(ring_buf_t *p_Loop_Data);
uint16_t Buf_Write_Byte(ring_buf_t *p_Loop_Data, uint8_t Data);
uint16_t Buf_Write_Bytes(ring_buf_t *p_Loop_Data, uint8_t *pu8Buff, uint16_t u16Len);
uint16_t Buf_Read_Byte(ring_buf_t *p_Loop_Data, uint8_t *pData);
uint16_t Buf_Read_Bytes(ring_buf_t *p_Loop_Data, uint8_t *pu8Buff, uint16_t u16Len);

#endif

