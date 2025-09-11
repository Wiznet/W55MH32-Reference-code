#include <stdint.h>
#include <string.h>
#include "w55mh32_conf.h"

void flash_erase(uint32_t address, uint32_t length)
{
    int i;
    uint32_t status;
    uint32_t page_num;
    uint32_t erase_address;

    page_num = length / 4096;

    if (length % 4096 > 0)
    {
        page_num++;
    }

    FLASH_Unlock();

    for (i = 0; i < page_num; i++)
    {
        erase_address = address + i * 4096;

        //  printf("[bootload] %08X\r\n", erase_address);
        status = FLASH_ErasePage(erase_address);

        while (status == FLASH_BUSY)
        {
            if (erase_address < 0x807FFFF)
            {
                status = FLASH_GetBank1Status();
            }
            else
            {
                status = FLASH_GetBank2Status();
            }
        }
    }

    FLASH_Lock();
}

void flash_write(uint32_t address, uint8_t *data, uint32_t length)
{
    int i;

    uint32_t status;
    uint32_t write_address;
    uint32_t write_data;

    FLASH_Unlock();

    for (i = 0; i < length; i += 4)
    {
        memcpy(&write_data, data + i, 4);
        write_address = address + i;
        status = FLASH_ProgramWord(write_address, write_data);

        while (status == FLASH_BUSY)
        {
            if (write_address < 0x807FFFF)
            {
                status = FLASH_GetBank1Status();
            }
            else
            {
                status = FLASH_GetBank2Status();
            }
        }
    }
    FLASH_Lock();
}

void flash_read(uint32_t address, uint8_t *data, uint32_t length)
{
    uint32_t i;

    for (i = 0; i < length; i++)
    {
        data[i] = *(__IO uint8_t *)(address + i);
    }
}
