#include "tftp_client.h"
#include "tftp.h"
#include "netutil.h"
#include "stdio.h"
#include "wizchip_conf.h"
int     tftp_state;
uint8_t tftp_read_flag = 0;

/**
 * @brief Perform TFTP client operations
 *
 * This function is used to perform operations on the TFTP client, including sending read requests and receiving file data.
 *
 * @return void
 */
void do_tftp_client(uint8_t sn, uint8_t *buff)
{
    uint32_t tftp_server_ip        = inet_addr((uint8_t*)TFTP_SERVER_IP);
    uint8_t  tftp_read_file_name[] = TFTP_SERVER_FILE_NAME;


    TFTP_init(sn, buff);
    while (1)
    {
        if (tftp_read_flag == 0)
        {
            printf("tftp server ip: %s, file name: %s\r\n", TFTP_SERVER_IP, TFTP_SERVER_FILE_NAME);
            printf("send request\r\n");
            TFTP_read_request(tftp_server_ip, (uint8_t*)TFTP_SERVER_FILE_NAME);
            tftp_read_flag = 1;
        }
        else
        {
            tftp_state = TFTP_run();
            if (tftp_state == TFTP_SUCCESS)
            {
                printf("tftp read success, file name: %s\r\n", tftp_read_file_name);
                while (1)
                {
                }
            }
            else if (tftp_state == TFTP_FAIL)
            {
                printf("tftp read fail, file name: %s\r\n", tftp_read_file_name);
                while (1)
                {
                }
            }
        }
    }
}
