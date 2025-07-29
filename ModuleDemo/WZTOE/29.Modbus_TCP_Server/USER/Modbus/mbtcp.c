#include <string.h>
#include "socket.h"
#include "mbascii.h"
#include "mbtcp.h"
#include "stdio.h"
#include "mb.h"

#define MB_TCP_BUF_SIZE (256 + 7) /* Must hold a complete Modbus TCP frame. */

#define MB_TCP_TID1         0
#define MB_TCP_TID2         1
#define MB_TCP_PID          2
#define MB_TCP_LEN          4
#define MB_TCP_UID          6
#define MB_TCP_FUNC         7
#define MB_SER_PDU_SIZE_MIN 3
#define MB_SER_PDU_ADDR_OFF 0 /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF  1 /*!< Offset of Modbus-PDU in Ser-PDU. */

#define MB_RTU_ADDR_SIZE 1

#define MB_UDP_CRC16_SIZE 2

#define MB_TCP_PROTOCOL_ID 0 /* 0 = Modbus Protocol */

uint8_t        b_listening_printed = 0;
uint8_t        mbTCPtid1, mbTCPtid2;
static uint8_t aucTCPBuf[MB_TCP_BUF_SIZE];
/* control user led callback function */
int (*getUserLED_cb)(void)      = NULL;
void (*setUserLED_cb)(uint32_t) = NULL;
extern volatile uint8_t *pucASCIIBufferCur;
extern volatile uint16_t usASCIIBufferPos;
extern uint8_t           recv_data[2048];
extern uint16_t          recv_len;
uint8_t                  lip[4];
extern uint16_t          local_port;
uint8_t                  rip[4];
uint16_t                 port;

static int mbTCPGet(uint8_t **ppucMBTCPFrame, uint16_t *usTCPLength)
{
    uint16_t len;
    uint16_t usTCPBufPos;

    len = getSn_RX_RSR(0);

    if (len > 0)
    {
        switch (getSn_SR(0))
        {
        case SOCK_ESTABLISHED:
        case SOCK_CLOSE_WAIT:
            usTCPBufPos = recv(0, aucTCPBuf, len);
            break;
        default:
            break;
        }
        *ppucMBTCPFrame = &aucTCPBuf[0];
        *usTCPLength    = usTCPBufPos;
        return 1;
    }
    return 0;
}

/* ----------------------- Defines ------------------------------------------*/

/* ----------------------- MBAP Header --------------------------------------*/
/*
 *
 * <------------------------ MODBUS TCP/IP ADU(1) ------------------------->
 *              <----------- MODBUS PDU (1') ---------------->
 *  +-----------+---------------+------------------------------------------+
 *  | TID | PID | Length | UID  |Code | Data                               |
 *  +-----------+---------------+------------------------------------------+
 *  |     |     |        |      |
 * (2)   (3)   (4)      (5)    (6)
 *
 * (2)  ... MB_TCP_TID          = 0 (Transaction Identifier - 2 Byte)
 * (3)  ... MB_TCP_PID          = 2 (Protocol Identifier - 2 Byte)
 * (4)  ... MB_TCP_LEN          = 4 (Number of bytes - 2 Byte)
 * (5)  ... MB_TCP_UID          = 6 (Unit Identifier - 1 Byte)
 * (6)  ... MB_TCP_FUNC         = 7 (Modbus Function Code)
 *
 * (1)  ... Modbus TCP/IP Application Data Unit
 * (1) ... Modbus Protocol Data Unit
 */

/**
 * @brief Processing MBTCP data packets
 *
 * Get the data frame from the MBTCP protocol and process it. If the data frame is successfully obtained, parse the 7-byte header of the MODBUS protocol,
 * And according to the MODBUS protocol of TCP transmission, obtain the protocol identifier, source address, transaction identifier and data frame length and other information.
 *
 * @param pucRcvAddress Receive address pointer
 * @param ppucFrame Pointer to the data frame pointer
 * @param pusLength Data frame length pointer
 *
 * @return Success returns 1, failure returns 0.
 */
static int mbTCPPackage(uint8_t *pucRcvAddress, uint8_t **ppucFrame, uint16_t *pusLength)
{
    uint8_t *pucMBTCPFrame;
    uint16_t usLength;
    uint16_t usPID;
    int      i;

    if (mbTCPGet(&pucMBTCPFrame, &usLength) != 0) // Determine whether there is network data and get the data frame and length
    {
        for (i = 0; i < usLength; i++)
        {
            recv_data[i] = pucMBTCPFrame[i];
        }
        recv_len = usLength;
        // Parse the 7-byte header of the MODUBUS protocol based on TCP transmission
        usPID  = pucMBTCPFrame[MB_TCP_PID] << 8U; // 2-byte protocol identifier 0 --> modubus protocol
        usPID |= pucMBTCPFrame[MB_TCP_PID + 1];

        if (usPID == MB_TCP_PROTOCOL_ID)
        {
            /* Modbus TCP does not use any addresses. Fake the source address such
			 * that the processing part deals with this frame.
			 */
            *pucRcvAddress = pucMBTCPFrame[MB_TCP_UID]; // Gets the length of the frame after the cell identifier
            /*Records transaction processing identifiers*/
            mbTCPtid1 = pucMBTCPFrame[MB_TCP_TID1];
            mbTCPtid2 = pucMBTCPFrame[MB_TCP_TID2];

            *ppucFrame = &pucMBTCPFrame[MB_TCP_FUNC]; // Gets the cell identifier
            *pusLength = usLength - MB_TCP_FUNC;
            return 1;
        }
    }
    return 0;
}
// bulking

/**
 * @brief modbus tcp frame to evb frame
 *
 * Converts MBTCP data packets to EVB frame format.
 *
 * @return The conversion successfully returns 1, otherwise returns 0.
 */
int MBtcp2evbFrame(void)
{
    uint8_t  pucRcvAddress;
    uint16_t pusLength;
    uint8_t *ppucFrame;
    uint8_t  usLRC;

    if (mbTCPPackage(&pucRcvAddress, &ppucFrame, &pusLength) != 0)                                           // Parsing data frame
    {
        pucASCIIBufferCur                      = ppucFrame - 1;                                              // Get the location function code and the following data
        pucASCIIBufferCur[MB_SER_PDU_ADDR_OFF] = pucRcvAddress;                                              // Get the function code and the following data
        usASCIIBufferPos                       = pusLength + MB_RTU_ADDR_SIZE;                               // Get data size
        usLRC                                  = prvucMBLRC((uint8_t *)pucASCIIBufferCur, usASCIIBufferPos); // LRC check
        pucASCIIBufferCur[usASCIIBufferPos++]  = usLRC;

        return 1;
    }
    return 0;
}

/**
 * @brief Perform Modbus TCP Server operations
 *
 * Modbus TCP main function
 *
 * @param sn : socket number
 */
void do_Modbus(uint8_t sn)
{
    uint8_t  state = 0;
    uint16_t len;
    getSIPR(lip);
    state = getSn_SR(sn);
    switch (state)
    {
    case SOCK_SYNSENT:
        break;
    case SOCK_INIT:
        listen(sn);
        if (!b_listening_printed)
        {
            b_listening_printed = 1;
            printf("Listening on %d.%d.%d.%d:%d\r\n",
                   lip[0], lip[1], lip[2], lip[3], local_port);
        }
        break;
    case SOCK_LISTEN:
        break;
    case SOCK_ESTABLISHED:
        if (getSn_IR(sn) & Sn_IR_CON)
        {
            setSn_IR(sn, Sn_IR_CON);
            printf("Connected\r\n");
            getSn_DIPR(sn, rip);
            port = getSn_DPORT(sn);
            printf("RemoteIP:%d.%d.%d.%d Port:%d\r\n", rip[0], rip[1], rip[2], rip[3], port);

            if (b_listening_printed)
                b_listening_printed = 0;
        }
        len = getSn_RX_RSR(sn);
        if (len > 0)
        {
            mbTCPtoEVB(sn);
        }
        break;
    case SOCK_CLOSE_WAIT:
        disconnect(sn);
        break;
    case SOCK_CLOSED:
    case SOCK_FIN_WAIT:
        close(sn);
        socket(sn, Sn_MR_TCP, local_port, Sn_MR_ND); // Sn_MR_ND
        break;
    default:
        break;
    }
}

/* Registration function */
void user_led_control_init(int (*get_fun)(void), void (*set_fun)(uint32_t))
{
    if (get_fun != NULL && set_fun != NULL)
    {
        getUserLED_cb = get_fun;
        setUserLED_cb = set_fun;
    }
}

int get_led_status(void)
{
    return getUserLED_cb();
}
void set_led_status(int32_t val)
{
    setUserLED_cb(val);
}
