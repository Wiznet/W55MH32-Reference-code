#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "w55mh32_conf.h"
#include "socket.h"
#include "fw_update.h"
#include "flash.h"
#include "config.h"

extern CONFIG_MSG system_cfg;

static uint32_t update_firmware_calc_checksum(uint32_t length)
{
    uint32_t address = APPLICATION_BACK_ADDRESS;

    uint32_t checksum = 0;

    for (uint32_t i = 0; i < length; i += 4)
    {
        checksum += *(volatile uint32_t *)(address + i);
    }

    return checksum;
}

void fw_update_process(void)
{
    static uint32_t file_size = 0;
    static uint32_t file_recv_length = 0;

    static uint8_t is_erased_flag = 0;
    static uint32_t flash_offset = 0;

    uint32_t len = 0;

    if (system_cfg.state != FW_UPDATE_CONFIG)
    {
        return;
    }

    switch (getSn_SR(SOCK_FW_UPDATE))
    {
    case SOCK_ESTABLISHED:
        if (getSn_IR(SOCK_FW_UPDATE) & Sn_IR_CON)
        {
            setSn_IR(SOCK_FW_UPDATE, Sn_IR_CON);

            is_erased_flag = 0;

            flash_offset = APPLICATION_BACK_ADDRESS;

            file_size = 0;
            file_recv_length = 0;
        }

        // some timeout function should be added here
        len = getSn_RX_RSR(SOCK_FW_UPDATE);
        if (len > 0)
        {
            if ((len == 4) && (is_erased_flag == 0))
            {
                recv(SOCK_FW_UPDATE, (uint8_t *)&file_size, 4);

                flash_erase(APPLICATION_BACK_ADDRESS, APPLICATION_AREA_SIZE);
                is_erased_flag = 1;

                // send the len to PC program to tell him flash erased over
                send(SOCK_FW_UPDATE, (uint8_t *)&file_size, 4);

                printf("file_size %d\r\n", file_size);
                printf(">");
            }
            else
            {
                len = recv(SOCK_FW_UPDATE, public_buff, len);

                printf(".");

                flash_write(flash_offset, public_buff, len);
                flash_offset += len;
                file_recv_length += len;

                send(SOCK_FW_UPDATE, (uint8_t *)&len, (uint16_t)4);

                if (file_size == file_recv_length)
                {
                    printf("\r\n");
                    disconnect(SOCK_FW_UPDATE);
                    system_cfg.fw_checksum = update_firmware_calc_checksum(file_size);
                    system_cfg.fw_len = file_size;
                    system_config_param_write(&system_cfg);
                    reboot_flag = 1; // reboot
                }
            }
        }
        break;
    case SOCK_CLOSE_WAIT:
        // disconnect(SOCK_FW_UPDATE);
        break;
    case SOCK_CLOSED:
        close(SOCK_FW_UPDATE);
        socket(SOCK_FW_UPDATE, Sn_MR_TCP, FW_UPDATE_PORT, Sn_MR_ND);
        break;
    case SOCK_INIT:
        listen(SOCK_FW_UPDATE);
        printf("fw update socket init\r\n");
        break;
    }
}
