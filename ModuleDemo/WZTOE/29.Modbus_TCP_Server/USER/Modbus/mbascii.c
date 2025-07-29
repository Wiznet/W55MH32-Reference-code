#include "mbascii.h"
#include "mbtcp.h"
#include "stdio.h"
// #include "timer.h"

volatile uint8_t mb_state_finish;
extern uint8_t   plus_count;

#define MB_SER_PDU_SIZE_MIN 3   /*!< Minimum size of a Modbus ASCII frame. */
#define MB_SER_PDU_SIZE_MAX 256 /*!< Maximum size of a Modbus ASCII frame. */
#define MB_SER_PDU_SIZE_LRC 1   /*!< Size of LRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF 0   /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF  1   /*!< Offset of Modbus-PDU in Ser-PDU. */
typedef enum
{
    STATE_RX_IDLE, /*!< Receiver is in idle state. */
    STATE_RX_RCV,  /*!< Frame is beeing received. */
    STATE_RX_END,
    STATE_RX_ERROR /*!< If the frame is invalid. */
} eMBRcvState;

extern volatile uint8_t *pucASCIIBufferCur;
extern volatile uint16_t usASCIIBufferPos;

extern volatile uint8_t *pucTCPBufferCur;
extern volatile uint16_t usTCPBufferPos;

volatile uint8_t  ucRTUBuf[MB_SER_PDU_SIZE_MAX];
volatile uint8_t *ucASCIIBuf = ucRTUBuf;

// static eMBRcvState eRcvState;

/**
 * @brief Convert hexadecimal characters to binary numbers
 *
 * Converts the given hexadecimal character to the corresponding binary number. Returns 0xFF if the character is not a valid hexadecimal character.
 *
 * @param ucCharacter :Hexadecimal character
 *
 * @return The converted binary number, returning 0xFF if the character is invalid
 */
uint8_t prvucMBCHAR2BIN(uint8_t ucCharacter)
{
    if ((ucCharacter >= '0') && (ucCharacter <= '9'))
    {
        return (uint8_t)(ucCharacter - '0');
    }
    else if ((ucCharacter >= 'A') && (ucCharacter <= 'F'))
    {
        return (uint8_t)(ucCharacter - 'A' + 0x0A);
    }
    else
    {
        return 0xFF;
    }
}

/**
 * @brief Converts a byte to the corresponding character
 *
 * Converts the given byte value to the corresponding character. If the byte value is between 0x00 and 0x09, the corresponding numeric character is returned;
 * If the byte value is between 0x0A and 0x0F, the corresponding uppercase character is returned.
 *
 * @param ucByte :Byte value to convert
 *
 * @return Converted character
 */
uint8_t prvucMBBIN2CHAR(uint8_t ucByte)
{
    if (ucByte <= 0x09)
    {
        return (uint8_t)('0' + ucByte);
    }
    else if ((ucByte >= 0x0A) && (ucByte <= 0x0F))
    {
        return (uint8_t)(ucByte - 0x0A + 'A');
    }
    else
    {
        /* Programming error. */
        //        assert( 0 );
    }
    return '0';
}

/**
 * @brief Calculate the MBLRC checksum
 *
 * According to the given frame data and length, the MBLRC (Modbus Longitudinal Redundancy Check) checksum is calculated.
 *
 * @param pucFrame : frame data pointer
 * @param usLen : frame data length
 *
 * @return The calculated MBLRC checksum
 */
uint8_t prvucMBLRC(uint8_t *pucFrame, uint16_t usLen)
{
    uint8_t ucLRC = 0; /* LRC char initialized */
    while (usLen--)
    {
        ucLRC += *pucFrame++; /* Add buffer byte without carry */
    }

    /* Return twos complement */
    ucLRC = (uint8_t)(-((uint8_t)ucLRC));
    return ucLRC;
}

/**
 * @brief Convert Modbus EVB data packets in ASCII format to binary format
 *
 * Converts the Modbus EVB data packet in the given ASCII format to binary format and returns the converted data packet.
 *
 * @param pucRcvAddress Receive address pointer
 * @param pucFrame Data frame pointer
 * @param pusLength Data length pointer
 *
 * @return The conversion successfully returns 1, and the conversion fails to return 0.
 */
static int mbEVBPackage(uint8_t *pucRcvAddress, uint8_t **pucFrame, uint16_t *pusLength)
{
    uint16_t count;
    for (count = 0; count < usASCIIBufferPos; count++)
    {
        if (count % 2)
        {
            pucASCIIBufferCur[(uint16_t)(count / 2)] |= prvucMBCHAR2BIN(ucASCIIBuf[count]);
        }
        else
        {
            pucASCIIBufferCur[count / 2] = (uint8_t)(prvucMBCHAR2BIN(ucASCIIBuf[count]) << 4);
        }
    }
    if (prvucMBLRC((uint8_t *)pucASCIIBufferCur, usASCIIBufferPos / 2) == 0)
    {
        *pucRcvAddress = pucASCIIBufferCur[MB_SER_PDU_ADDR_OFF];
        *pusLength     = (uint16_t)(usASCIIBufferPos / 2 - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_LRC);
        *pucFrame      = (uint8_t *)&pucASCIIBufferCur[MB_SER_PDU_PDU_OFF];
        return 1;
    }

    return 0;
}

/**
 * @brief Convert Modbus EVB frames to TCP frames
 *
 * Converts a Modbus EVB frame to a Modbus TCP frame.
 *
 * @return The conversion successfully returns 1, and the failure returns 0.
 */
int MBevb2tcpFrame(void)
{
    uint8_t  pucRcvAddress;
    uint16_t pusLength;
    uint8_t *ppucFrame;

    if (mbEVBPackage(&pucRcvAddress, &ppucFrame, &pusLength) != 0)
    {
        /*Form a MODBUS TCP packet*/
        pucTCPBufferCur = ppucFrame - 7;

        pucTCPBufferCur[0] = mbTCPtid1;
        pucTCPBufferCur[1] = mbTCPtid2;

        pucTCPBufferCur[2] = 0;
        pucTCPBufferCur[3] = 0;

        pucTCPBufferCur[4] = (pusLength + 1) >> 8U;
        pucTCPBufferCur[5] = (pusLength + 1) & 0xFF;

        pucTCPBufferCur[6] = pucRcvAddress;

        usTCPBufferPos = pusLength + 7;
        return 1;
    }
    return 0;
}
