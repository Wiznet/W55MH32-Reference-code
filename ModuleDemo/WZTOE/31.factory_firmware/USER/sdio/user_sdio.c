#include <stdio.h>
#include "user_sdio.h"
#include "delay.h"
#include "string.h"
#include "bsp_sdio_sdcard.h"
#include "sdio_test.h"
#include "ff.h"

uint8_t file_cnt           = 0;
uint8_t file_name[20][255] = {0};

int get_sd_filename(void)
{
    extern uint32_t    CSD_Tab[4], CID_Tab[4];
    extern SD_CardInfo SDCardInfo;
    /**
	Define the variables required by FATFS
	*/
    FATFS    fs;     //FATFS file system object
    FRESULT  res_sd; //File operation result
    DIR      dirinf;
    FILINFO  fileinf;
    uint32_t cnt = 0;
    char     name[256];

    res_sd = f_mount(&fs, "0:", 1);
    if (res_sd != FR_OK)
    {
        printf("Failed to mount file system (%d)\r\n", res_sd);
        return 0;
    }

    res_sd = f_opendir(&dirinf, "/");
    if (res_sd != FR_OK)
    {
        printf("Failed to open root directory (%d)\r\n", res_sd);
        return 0;
    }

    fileinf.lfname = name;
    fileinf.lfsize = 256;

    for (cnt = 0;; cnt++)
    {
        res_sd = f_readdir(&dirinf, &fileinf);
        if (res_sd != FR_OK || fileinf.fname[0] == 0)
        {
            break;
        }

        if (fileinf.fname[0] == '.')
        {
            continue;
        }

        if (strcmp((char *)fileinf.lfname, "") == 0)
        {
            strcpy((char *)file_name[file_cnt++], (char *)fileinf.fname);
        }
        else
        {
            strcpy((char *)file_name[file_cnt++], (char *)fileinf.lfname);
        }
    }
    f_mount(NULL, "0:", 1);
    return 1;
}

int put_sd(uint8_t *file_name, uint8_t *file_data, uint16_t data_len)
{
    FATFS   fs;     //FATFS�ļ�ϵͳ����
    FIL     fnew;   //�ļ�����
    FRESULT res_sd; //�ļ��������
    UINT    fnum;   //�ļ��ɹ���д����

    res_sd = f_mount(&fs, "0:", 1);

    res_sd = f_open(&fnew, (char *)file_name, FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res_sd == FR_OK)
    {
        printf("��/�����ļ��ɹ�, ���ļ���д������\r\n");


        res_sd = f_write(&fnew, file_data, data_len, &fnum);
        if (res_sd == FR_OK)
        {
            printf("�ļ�д��ɹ�\n");
        }
        else
        {
            printf("�ļ�д��ʧ�� (%d)\n", res_sd);
            return 0;
        }
        f_close(&fnew);
    }
    else
    {
        printf("��/���� �ļ�ʧ��\r\n");
        return 0;
    }
    return 1;
}
