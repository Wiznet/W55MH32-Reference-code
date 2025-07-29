#ifndef _TCP_SERVER_MULTI_SOCKET_H_
#define _TCP_SERVER_MULTI_SOCKET_H_
#include "wizchip_conf.h"

#ifndef DATA_BUF_SIZE
#define DATA_BUF_SIZE 2048
#endif

/**
 * @brief multi socket loopback function
 * @param buf :buffer pointer
 * @param destport: local port number
 *
 * @return Returns the operation result, 1 for success and error code for failure
 */
int32_t multi_tcps_socket(uint8_t *buf, uint16_t localport);
#endif
