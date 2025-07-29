#ifndef _MBASCII_H
#define _MBASCII_H

#include <stdio.h>
#include <stdint.h>

#define MB_ASCII_START      ':'
#define MB_ASCII_DEFAULT_CR '\r' /*!< Default CR character for Modbus ASCII. */
#define MB_ASCII_DEFAULT_LF '\n' /*!< Default LF character for Modbus ASCII. */

extern volatile uint8_t mb_state_finish;

/**
 * @brief Convert hexadecimal characters to binary numbers
 *
 * Converts the given hexadecimal character to the corresponding binary number. Returns 0xFF if the character is not a valid hexadecimal character.
 *
 * @param ucCharacter :Hexadecimal character
 *
 * @return The converted binary number, returning 0xFF if the character is invalid
 */
uint8_t prvucMBCHAR2BIN(uint8_t ucCharacter);

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
uint8_t prvucMBBIN2CHAR(uint8_t ucByte);

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
uint8_t prvucMBLRC(uint8_t *pucFrame, uint16_t usLen);

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
static int mbEVBPackage(uint8_t *pucRcvAddress, uint8_t **pucFrame, uint16_t *pusLength);

/**
 * @brief Convert Modbus EVB frames to TCP frames
 *
 * Converts a Modbus EVB frame to a Modbus TCP frame.
 *
 * @return The conversion successfully returns 1, and the failure returns 0.
 */
int MBevb2tcpFrame(void);
#endif
