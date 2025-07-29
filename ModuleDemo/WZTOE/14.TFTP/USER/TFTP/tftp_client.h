#ifndef _TFTP_CLINET_H
#define _TFTP_CLINET_H
#include "stdint.h"

#define TFTP_CLIENT_SOCKET_BUFFER_SIZE (1024 * 2)
#define TFTP_SERVER_IP                 "192.168.1.20"
#define TFTP_SERVER_FILE_NAME          "tftp_test_file.txt"

/**
 * @brief Perform TFTP client operations
 * @param sn:socket number
 * @param buff:ethernet buffer
 * @return void
 */
void do_tftp_client(uint8_t sn, uint8_t *buff);
#endif
