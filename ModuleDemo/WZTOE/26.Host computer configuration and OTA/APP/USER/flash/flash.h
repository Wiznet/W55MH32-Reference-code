#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

void flash_erase(uint32_t address, uint32_t length);
void flash_write(uint32_t address, uint8_t *data, uint32_t length);
void flash_read(uint32_t address, uint8_t *data, uint32_t length);

#endif
