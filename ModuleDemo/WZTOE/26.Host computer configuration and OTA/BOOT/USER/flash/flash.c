#include "flash.h"
#include <stdint.h>
#include <string.h>
#include "w55mh32_conf.h"

int flash_erase(uint32_t address, uint32_t length)
{
    int i;
    uint32_t status;
    uint32_t page_num;
    uint32_t erase_address;

    if(address < FLASH_BASE_ADDRESS || (address + length) > (FLASH_BASE_ADDRESS + FLASH_SIZE))
    {
        return -1; // Illegal address
    }
    page_num = length / FLASH_PAGE_SIZE;

    if (length % FLASH_PAGE_SIZE > 0)
    {
        page_num++;
    }

    FLASH_Unlock();

    for (i = 0; i < page_num; i++)
    {
        erase_address = address + i * FLASH_PAGE_SIZE;

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
    return 0;
}

int flash_write(uint32_t address, uint8_t *data, uint32_t length)
{
    int i;

    uint32_t status;
    uint32_t write_address;
    uint32_t write_data;

    if(address < FLASH_BASE_ADDRESS || (address + length) > (FLASH_BASE_ADDRESS + FLASH_SIZE))
    {
        return -1; // Illegal address
    }
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
    return 0;
}

int flash_read(uint32_t address, uint8_t *data, uint32_t length)
{
    uint32_t i;

    if(address < FLASH_BASE_ADDRESS || (address + length) > (FLASH_BASE_ADDRESS + FLASH_SIZE))
    {
        return -1; // Illegal address
    }
    for (i = 0; i < length; i++)
    {
        data[i] = *(__IO uint8_t *)(address + i);
    }
    return 0;
}
