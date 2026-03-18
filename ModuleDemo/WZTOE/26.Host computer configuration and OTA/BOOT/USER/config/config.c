#include <stdio.h>
#include <string.h>

#include "w55mh32_conf.h"
#include "wizchip_conf.h"
#include "config.h"
#include "bsp_uart.h"
#include "flash.h"

#define DEFAULT_MAC_EN 1

CONFIG_MSG system_cfg={0},temp_cfg={0};

uint8_t reboot_flag = 0;

uint8_t public_buff[2048];

const uint8_t user_default_mac[6] = {0x00, 0x08, 0xdc, 0x11, 0x12, 0x13};

void system_config_param_write(void *data)
{
    flash_erase(CONFIG_PARAM_ADDRESS, CONFIG_PARAM_SIZE);
    flash_write(CONFIG_PARAM_ADDRESS, (uint8_t *)data + 4, CONFIG_PARAM_SIZE);
}

void system_param_config_read(void *data)
{
    flash_read(CONFIG_PARAM_ADDRESS, (uint8_t *)data + 4, CONFIG_PARAM_SIZE);
}

void system_config_param_default(void)
{
    CONFIG_MSG *cfg = &system_cfg;

    cfg->state = FW_APP_NORMAL;

    cfg->sw_ver[0] = FW_BOOT_VER_HIGH;
    cfg->sw_ver[1] = FW_BOOT_VER_LOW;
    memcpy(cfg->lip, "\xC0\xA8\x01\x58", 4);
    memcpy(cfg->sub, "\xFF\xFF\xFF\x00", 4);
    memcpy(cfg->gw, "\xC0\xA8\x01\x01", 4);
    memcpy(cfg->dns, "\x72\x72\x72\x72", 4);
}

void system_config_param_save(void)
{
    system_config_param_write(&system_cfg);
}

void system_reboot(void)
{
    NVIC_SystemReset();
}

void system_config_param_get(void)
{
    system_param_config_read(&system_cfg);
	  system_cfg.sw_ver[0] = FW_BOOT_VER_HIGH;
	  system_cfg.sw_ver[1] = FW_BOOT_VER_LOW;
}

int system_first_run_check(void)
{
    if ((system_cfg.mac[0] != 0x00) && (system_cfg.mac[0] != 0x08) && (system_cfg.mac[0] != 0xdc))
    {
        printf("First download\r\n");
#if DEFAULT_MAC_EN ==1
        getSHAR(system_cfg.mac); 
#else
        memcpy(system_cfg.mac, user_default_mac, 6);
#endif
        system_config_param_default();
        system_config_param_save();
        return 1;
    }
    return 0;
}


typedef void (*pFunction)(void);

void reboot_app(void)
{
    pFunction JumpToApplication;
    uint32_t  JumpAddress;
    JumpAddress       = *(volatile uint32_t *)(APPLICATION_START_ADDRESS + 4);
    JumpToApplication = (pFunction)JumpAddress;
    //initialize user application's stack pointer
    __set_MSP(*(__IO uint32_t *)APPLICATION_START_ADDRESS);
    JumpToApplication();
}


int app_is_inside(void)
{
    if (*(volatile uint32_t *)APPLICATION_START_ADDRESS == 0xFFFFFFFF)
    {
        return 0;
    }
    return 1;
}

int app_copy(uint32_t fw_len, uint32_t fw_sum_check)
{
    uint32_t i;
    uint32_t status;
    uint32_t write_data;
    uint32_t write_address = APPLICATION_START_ADDRESS;
    uint32_t read_address  = APPLICATION_BACK_ADDRESS;
    uint32_t sum_check     = 0;


    printf("[bootload] fw_len = %u\r\n", fw_len);
    printf("[bootload] fw_sum_check = %u\r\n", fw_sum_check);

    if (fw_len == 0 || fw_len > APPLICATION_AREA_SIZE)
    {
        return -1;
    }


    flash_erase(APPLICATION_START_ADDRESS, APPLICATION_AREA_SIZE);

    FLASH_Unlock();

    for (i = 0; i < fw_len; i += 4)
    {
        write_address = APPLICATION_START_ADDRESS + i;
        read_address  = APPLICATION_BACK_ADDRESS + i;

        write_data = *(volatile uint32_t *)(read_address);

       // printf("##i = %08X, address = %08X, fw_len = %08X\r\n", i, write_address, fw_len);

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

        sum_check += write_data;
    }

    FLASH_Lock();

    printf("sum = %08X, check = %08X\r\n", sum_check, fw_sum_check);
    if (sum_check != fw_sum_check)
    {
        return -1;
    }
    

    printf("copy commilet\r\n");
    return 0;
}
