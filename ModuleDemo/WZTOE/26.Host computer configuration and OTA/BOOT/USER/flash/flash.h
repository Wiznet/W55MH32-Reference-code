#ifndef __FLASH_H__
#define __FLASH_H__

#include <stdint.h>

#define FLASH_BASE_ADDRESS 0x08000000
#define FLASH_SIZE         (1024 * 1024) // 1MB
#define FLASH_PAGE_SIZE    (4 * 1024)          // 4KB per page

int flash_erase(uint32_t address, uint32_t length);
int flash_write(uint32_t address, uint8_t *data, uint32_t length);
int flash_read(uint32_t address, uint8_t *data, uint32_t length);

#endif
