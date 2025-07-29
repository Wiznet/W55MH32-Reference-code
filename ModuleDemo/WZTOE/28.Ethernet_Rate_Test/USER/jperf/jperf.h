#ifndef _JPERF_H
#define _JPERF_H
#include "stdint.h"

/**
 * @brief Perform speed measurements
 *
 * Perform speed measurement operations based on the specified sequence number, send buffer, destination IP address, and destination port number.
 *
 * @param SN serial number
 * @param buffer_txst Send buffer pointer
 * @param des_ip Array of destination IP addresses
 * @param des_port Destination port number
 */
void do_jperf(uint8_t sn, uint8_t *buffer_txst, uint8_t *des_ip, uint16_t des_port);

#endif
