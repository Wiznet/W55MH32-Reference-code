#include "ring_buf.h"

uint16_t Buf_Get_Used_Size(ring_buf_t *p_ring_buf)
{
    uint16_t u16Data_Len;
    if (p_ring_buf->u16Tail >= p_ring_buf->u16Head)
        u16Data_Len = p_ring_buf->u16Tail - p_ring_buf->u16Head;
    else
        u16Data_Len = RING_BUF_SIZE - p_ring_buf->u16Head + p_ring_buf->u16Tail;
    return u16Data_Len;
}

uint16_t Buf_Get_Remain_Size(ring_buf_t *p_ring_buf)
{
    return RING_BUF_SIZE - Buf_Get_Used_Size(p_ring_buf) - 1;
}

uint16_t Buf_Write_Byte(ring_buf_t *p_ring_buf, uint8_t Data)
{
    int i = (p_ring_buf->u16Tail + 1) % RING_BUF_SIZE;

    if (i == p_ring_buf->u16Head)
        return 0;
    p_ring_buf->aucBuf[p_ring_buf->u16Tail] = Data;
    p_ring_buf->u16Tail                     = i;

    if ((p_ring_buf->u16Tail + 1) % RING_BUF_SIZE == p_ring_buf->u16Head)
        return 2;
    return 1;
}

uint16_t Buf_Write_Bytes(ring_buf_t *p_ring_buf, uint8_t *pu8Buff, uint16_t u16Len)
{
    uint16_t u16WriteLen = 0; /* Data buffer queue byte length */
    uint16_t u16Count    = 0; /* Temporary Byte Calculation Variable */
    uint16_t u16Ret      = 0;

    if (p_ring_buf->u16Tail >= p_ring_buf->u16Head)
    {
        u16WriteLen = RING_BUF_SIZE - p_ring_buf->u16Tail + p_ring_buf->u16Head - 1;
        if (u16WriteLen >= u16Len)
        {
            u16Count = RING_BUF_SIZE - p_ring_buf->u16Tail;
            if (u16Count > u16Len)
            {
                memcpy(&p_ring_buf->aucBuf[p_ring_buf->u16Tail], pu8Buff, u16Len);
                p_ring_buf->u16Tail += u16Len;
                u16Ret               = u16Len;
            }
            else
            {
                memcpy(&p_ring_buf->aucBuf[p_ring_buf->u16Tail], pu8Buff, u16Count);
                memcpy(&p_ring_buf->aucBuf[0], &pu8Buff[u16Count], u16Len - u16Count);
                p_ring_buf->u16Tail = u16Len - u16Count;
                u16Ret              = u16Len;
            }
        }
    }
    else
    {
        u16WriteLen = p_ring_buf->u16Head - p_ring_buf->u16Tail - 1;
        if (u16WriteLen >= u16Len)
        {
            memcpy(&p_ring_buf->aucBuf[p_ring_buf->u16Tail], pu8Buff, u16Len);
            p_ring_buf->u16Tail += u16Len;
            u16Ret               = u16Len;
        }
    }
    return u16Ret;
}

uint16_t Buf_Read_Byte(ring_buf_t *p_ring_buf, uint8_t *pData)
{
    if (p_ring_buf->u16Tail == p_ring_buf->u16Head)
        return 0;
    *pData              = p_ring_buf->aucBuf[p_ring_buf->u16Head];
    p_ring_buf->u16Head = (p_ring_buf->u16Head + 1) % RING_BUF_SIZE;
    return 1;
}

uint16_t Buf_Read_Bytes(ring_buf_t *p_ring_buf, uint8_t *pu8Buff, uint16_t u16Len)
{
    uint16_t u16ReadLen = 0; /* Data buffer queue byte length */
    uint16_t u16Count   = 0; /* Temporary Byte Calculation Variable */
    uint16_t u16Ret     = 0;
    uint16_t u16Tail    = p_ring_buf->u16Tail;

    if (u16Tail >= p_ring_buf->u16Head)
    {
        u16ReadLen = u16Tail - p_ring_buf->u16Head;
        if (u16ReadLen >= u16Len)
        {
            memcpy(pu8Buff, &p_ring_buf->aucBuf[p_ring_buf->u16Head], u16Len);
            p_ring_buf->u16Head += u16Len;
            u16Ret               = u16Len;
        }
    }
    else
    {
        u16ReadLen = RING_BUF_SIZE - p_ring_buf->u16Head + u16Tail;
        if (u16ReadLen >= u16Len)
        {
            u16Count = RING_BUF_SIZE - p_ring_buf->u16Head;
            if (u16Count >= u16Len)
            {
                memcpy(pu8Buff, &p_ring_buf->aucBuf[p_ring_buf->u16Head], u16Len);
                p_ring_buf->u16Head += u16Len;
                u16Ret               = u16Len;
            }
            else
            {
                memcpy(pu8Buff, &p_ring_buf->aucBuf[p_ring_buf->u16Head], u16Count);
                memcpy(&pu8Buff[u16Count], &p_ring_buf->aucBuf[0], u16Len - u16Count);
                p_ring_buf->u16Head = u16Len - u16Count;
                u16Ret              = u16Len;
            }
        }
    }
    return u16Ret;
}

