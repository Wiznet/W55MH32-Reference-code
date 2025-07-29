#ifndef _MB_H_
#define _MB_H_

/**
 * @brief Parse Modbus TCP instructions and process them accordingly
 *
 * According to the given sequence number, the Modbus TCP data is converted into an EVB instruction and processed accordingly.
 *
 * @param sn socket number
 */
void mbTCPtoEVB(uint8_t sn);
#endif
