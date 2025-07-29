#ifndef _USER_SDIO_H_
#define _USER_SDIO_H_

#include <stdint.h>

int get_sd_filename(void);
int put_sd(uint8_t *file_name, uint8_t *file_data, uint16_t data_len);

#endif
