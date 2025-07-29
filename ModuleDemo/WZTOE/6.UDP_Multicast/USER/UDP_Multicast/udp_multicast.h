#ifndef _UDP_MULTICAST_H_
#define _UDP_MULTICAST_H_
#include <stdint.h>
#define DATA_BUF_SIZE 2048

/**
 * @brief Multicast receive data
 * @param sn socket number
 * @param buf buffer pointer
 * @param multicast_mac Multicast MAC address
 * @param multicast_ip Multicast IP address
 * @param multicast_port Multicast port number
 * @return The size of the data received, which returns a negative number if the reception fails
 */
int32_t udp_multicast(uint8_t sn, uint8_t *buf, uint8_t *multicast_mac, uint8_t *multicast_ip, uint16_t multicast_port);

#endif
