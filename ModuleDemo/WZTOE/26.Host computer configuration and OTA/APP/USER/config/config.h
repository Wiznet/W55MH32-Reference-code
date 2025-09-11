#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdio.h>
#include <stdint.h>


#define FW_BOOT_VER_HIGH 1
#define FW_BOOT_VER_LOW  1

#define SOCK_UDP_CONFIG 0
#define SOCK_FW_UPDATE 1


#define CONFIG_STORAGE_MEDIUM_FLASH  1
#define CONFIG_STORAGE_MEDIUM_EEPROM 0

#define CONFIG_STORAGE_MEDIUM CONFIG_STORAGE_MEDIUM_EEPROM


#define BOOTLOAD_AREA_SIZE    0x00010000 // bootloader area size 64kb
#define APPLICATION_AREA_SIZE 0x00032000 // application size 200kb


#define APPLICATION_START_OFFSET 0x000010000


#define BOOTLOAD_START_ADDRESS    0x08000000                                          // bootloader area size 60kb
#define MAGIC_CODE_ADDRESS        (0x08000000 + 0x1000)                               // magic code area, size 4kb bytes
#define APPLICATION_START_ADDRESS 0x08010000                                          // application area size 200kb
#define APPLICATION_BACK_ADDRESS  (APPLICATION_START_ADDRESS + APPLICATION_AREA_SIZE) // application backup area size 200kb
#define CONFIG_PARAM_ADDRESS      0x080FF000                                          // config parameter area size 4kb


#define CONFIG_PARAM_ADDRESS 0x080FF000 // config parameter area size 4kb

#pragma pack(1)
typedef struct _CONFIG_MSG
{
    uint8_t op[4];
    uint8_t mac[6];
    uint8_t sw_ver[2];
    uint8_t lip[4];
    uint8_t sub[4];
    uint8_t gw[4];
    uint8_t dns[4];
	
    uint32_t fw_len;
	  uint32_t fw_checksum;
    uint8_t  state;
} CONFIG_MSG;
#pragma pack()


#define CONFIG_PARAM_SIZE (sizeof(CONFIG_MSG) -4)

#define FW_APP_NORMAL    0x80
#define FW_UPDATE_CONFIG 0x10

extern CONFIG_MSG	RecvMsg;
extern uint8_t reboot_flag;

extern uint8_t public_buff[2048];



void system_config_init(void);
void system_config_param_default(void);
void system_config_param_write(void *data);
void system_param_config_read(void *data);
void system_reboot(void);


void     system_config_param_get(void);
int      system_first_run_check(void);
//int      cpu_id_check(void);
uint16_t crc16_ccitt(uint8_t *data, uint16_t length);

int  app_is_inside(void);
void reboot_app(void);
int  app_copy(uint32_t fw_len, uint32_t fw_sum_check);
void system_config_param_save(void);

#endif


