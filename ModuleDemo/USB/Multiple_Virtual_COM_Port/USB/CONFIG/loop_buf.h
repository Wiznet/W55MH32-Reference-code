#ifndef __LOOP_BUF_H

#include "string.h"
#include "stdint.h"

#define LOOP_BUF_SIZE 1024
typedef struct buf_data
{
    uint16_t u16Head;
    uint16_t u16Tail;
    uint8_t  aucBuf[LOOP_BUF_SIZE];
} loop_buf_t;


uint16_t Buf_Get_Used_Size(loop_buf_t *p_Loop_Data);
uint16_t Buf_Get_Remain_Size(loop_buf_t *p_Loop_Data);
uint16_t Buf_Write_Byte(loop_buf_t *p_Loop_Data, uint8_t Data);
uint16_t Buf_Write_Bytes(loop_buf_t *p_Loop_Data, uint8_t *pu8Buff, uint16_t u16Len);
uint16_t Buf_Read_Bytes(loop_buf_t *p_Loop_Data, uint8_t *pu8Buff, uint16_t u16Len);

#endif

