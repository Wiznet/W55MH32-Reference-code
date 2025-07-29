#ifndef __HTTPCLIENT_H_
#define __HTTPCLIENT_H_

#include <stdint.h>

uint32_t http_get_pkt(uint8_t *pkt);
uint32_t http_post_pkt(uint8_t *pkt);
uint8_t  do_http_request(uint8_t sn, uint8_t *buf, uint16_t len, uint8_t *destip, uint16_t destport);

#endif
