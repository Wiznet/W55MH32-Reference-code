#ifndef _TCP_CLIENT_MULTI_SOCKET_H_
#define _TCP_CLIENT_MULTI_SOCKET_H_
#include "wizchip_conf.h"

#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE 2048
#endif

/**
 * @brief Multi-TCP connection socket operation
 *
 * Perform multi-TCP connection socket operations based on the given buffer, destination IP address, and destination port number.
 *
 * @param buf buffer pointer
 * @param destip array of destination IP addresses
 * @param destport destination port number
 *
 * @return Returns the operation result, 1 for success and error code for failure
 */
int32_t multi_tcpc_socket(uint8_t *buf, uint8_t *destip, uint16_t destport);
#endif
