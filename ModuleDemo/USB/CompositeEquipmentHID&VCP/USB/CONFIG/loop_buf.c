#include "loop_buf.h"

uint16_t Buf_Get_Used_Size(loop_buf_t *p_Loop_Data)
{
    uint16_t u16Data_Len;
    if (p_Loop_Data->u16Tail >= p_Loop_Data->u16Head)
        u16Data_Len = p_Loop_Data->u16Tail - p_Loop_Data->u16Head;
    else
        u16Data_Len = LOOP_BUF_SIZE - p_Loop_Data->u16Head + p_Loop_Data->u16Tail;
    return u16Data_Len;
}

uint16_t Buf_Get_Remain_Size(loop_buf_t *p_Loop_Data)
{
    return LOOP_BUF_SIZE - Buf_Get_Used_Size(p_Loop_Data) - 1;
}

uint16_t Buf_Write_Byte(loop_buf_t *p_Loop_Data, uint8_t Data)
{
    if ((p_Loop_Data->u16Tail + 1 == p_Loop_Data->u16Head)
        || ((p_Loop_Data->u16Tail + 1 >= LOOP_BUF_SIZE) && p_Loop_Data->u16Head == 0))
        return 0;

    p_Loop_Data->aucBuf[p_Loop_Data->u16Tail] = Data;
    if (p_Loop_Data->u16Tail >= (LOOP_BUF_SIZE - 1))
        p_Loop_Data->u16Tail = 0;
    else
        p_Loop_Data->u16Tail++;

    if ((p_Loop_Data->u16Tail + 1 == p_Loop_Data->u16Head)
        || ((p_Loop_Data->u16Tail + 1 >= LOOP_BUF_SIZE) && p_Loop_Data->u16Head == 0))
        return 2;
    return 1;
}

uint16_t Buf_Write_Bytes(loop_buf_t *p_Loop_Data, uint8_t *pu8Buff, uint16_t u16Len)
{
    uint16_t u16WriteLen = 0; /* Data buffer queue byte length */
    uint16_t u16Count    = 0; /* Temporary Byte Calculation Variable */
    uint16_t u16Ret      = 0;

    if (p_Loop_Data->u16Tail >= p_Loop_Data->u16Head)
    {
        u16WriteLen = LOOP_BUF_SIZE - p_Loop_Data->u16Tail + p_Loop_Data->u16Head - 1;
        if (u16WriteLen >= u16Len)
        {
            u16Count = LOOP_BUF_SIZE - p_Loop_Data->u16Tail;
            if (u16Count > u16Len)
            {
                memcpy(&p_Loop_Data->aucBuf[p_Loop_Data->u16Tail], pu8Buff, u16Len);
                p_Loop_Data->u16Tail += u16Len;
                u16Ret                = u16Len;
            }
            else
            {
                memcpy(&p_Loop_Data->aucBuf[p_Loop_Data->u16Tail], pu8Buff, u16Count);
                memcpy(&p_Loop_Data->aucBuf[0], &pu8Buff[u16Count], u16Len - u16Count);
                p_Loop_Data->u16Tail = u16Len - u16Count;
                u16Ret               = u16Len;
            }
        }
    }
    else
    {
        u16WriteLen = p_Loop_Data->u16Head - p_Loop_Data->u16Tail - 1;
        if (u16WriteLen >= u16Len)
        {
            memcpy(&p_Loop_Data->aucBuf[p_Loop_Data->u16Tail], pu8Buff, u16Len);
            p_Loop_Data->u16Tail += u16Len;
            u16Ret                = u16Len;
        }
    }
    return u16Ret;
}

uint16_t Buf_Read_Bytes(loop_buf_t *p_Loop_Data, uint8_t *pu8Buff, uint16_t u16Len)
{
    uint16_t u16ReadLen = 0; /* Data buffer queue byte length */
    uint16_t u16Count   = 0; /* Temporary Byte Calculation Variable */
    uint16_t u16Ret     = 0;
    uint16_t u16Tail    = p_Loop_Data->u16Tail;

    if (u16Tail >= p_Loop_Data->u16Head)
    {
        u16ReadLen = u16Tail - p_Loop_Data->u16Head;
        if (u16ReadLen >= u16Len)
        {
            memcpy(pu8Buff, &p_Loop_Data->aucBuf[p_Loop_Data->u16Head], u16Len);
            p_Loop_Data->u16Head += u16Len;
            u16Ret                = u16Len;
        }
        else
            u16Ret = u16ReadLen;
    }
    else
    {
        u16ReadLen = LOOP_BUF_SIZE - p_Loop_Data->u16Head + u16Tail;
        if (u16ReadLen >= u16Len)
        {
            u16Count = LOOP_BUF_SIZE - p_Loop_Data->u16Head;
            if (u16Count >= u16Len)
            {
                memcpy(pu8Buff, &p_Loop_Data->aucBuf[p_Loop_Data->u16Head], u16Len);
                p_Loop_Data->u16Head += u16Len;
                u16Ret                = u16Len;
            }
            else
            {
                memcpy(pu8Buff, &p_Loop_Data->aucBuf[p_Loop_Data->u16Head], u16Count);
                memcpy(&pu8Buff[u16Count], &p_Loop_Data->aucBuf[0], u16Len - u16Count);
                p_Loop_Data->u16Head = u16Len - u16Count;
                u16Ret               = u16Len;
            }
        }
        else
            u16Ret = u16ReadLen;
    }
    return u16Ret;
}

