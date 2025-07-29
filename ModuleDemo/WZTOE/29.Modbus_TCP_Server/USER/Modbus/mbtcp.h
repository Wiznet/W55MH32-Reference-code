#ifndef _MBTCP_H_
#define _MBTCP_H_

#include <stdio.h>
#include <stdint.h>

extern uint8_t mbTCPtid1;
extern uint8_t mbTCPtid2;

extern volatile uint8_t *pucRTUBufferCur;
extern volatile uint16_t usRTUBufferPos;
extern volatile uint8_t *pucTCPBufferCur;
extern volatile uint16_t usTCPBufferPos;

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
static int mbTCPPackage(uint8_t *pucRcvAddress, uint8_t **ppucFrame, uint16_t *pusLength);

/**
 * @brief modbus tcp frame to evb frame
 *
 * Converts MBTCP data packets to EVB frame format.
 *
 * @return The conversion successfully returns 1, otherwise returns 0.
 */
int MBtcp2evbFrame(void);

/**
 * @brief Perform Modbus TCP Server operations
 *
 * Modbus TCP main function
 *
 * @param sn : socket number
 */
void do_Modbus(uint8_t sn);

/* Registration function */
void user_led_control_init(int (*get_fun)(void), void (*set_fun)(uint32_t));
int  get_led_status(void);
void set_led_status(int32_t val);
#endif
