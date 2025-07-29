#include "w55mh32.h"
#include <stdio.h>
#include "bsp_sdio_sdcard.h"
#include "sdio_test.h"
#include "delay.h"
#include "ff.h"

void    USART_Config(uint32_t bound);
uint8_t GetCmd(void);
void    TestList(void);
void    ShowCardInfo(void);
void    SDInfoShow(void);
void    FatfsTest(void);
void    FatfsBigDataTest(void);
void    WriteFileTest(void);
void    CreateDir(void);
void    DeleteDirFile(void);
void    ViewRootDir(void);

extern uint32_t    CSD_Tab[4], CID_Tab[4];
extern SD_CardInfo SDCardInfo;
/**
Define the variables required by FATFS
*/
FATFS   fs;                                                                //FATFS file system object
FIL     fnew;                                                              //file object
FRESULT res_sd;                                                            //File operation result
UINT    fnum;                                                              //Number of successful read and write files
BYTE    ReadBuffer[1024] = {0};                                            //read buffer
BYTE    WriteBuffer[]    = "WIZnet, create a new file system test file\n"; //write buffer

/**
Used to test read and write speed
**/
#define TEST_FILE_LEN (2 * 1024 * 1024) //File length for testing
#define BUF_SIZE      (4 * 1024)        //Maximum data length per read and write to SD
uint8_t TestBuf[BUF_SIZE];

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "delay.h"
#include "w55mh32.h"


USART_TypeDef *USART_TEST = USART1;

void UART_Configuration(uint32_t bound);

int main(void)
{
    uint8_t           cmd = 0;
    RCC_ClocksTypeDef clocks;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
    delay_init();
    UART_Configuration(115200);
    RCC_GetClocksFreq(&clocks);

    printf("\n");
    printf("SYSCLK: %3.1fMhz, HCLK: %3.1fMhz, PCLK1: %3.1fMhz, PCLK2: %3.1fMhz, ADCCLK: %3.1fMhz\n",
           (float)clocks.SYSCLK_Frequency / 1000000, (float)clocks.HCLK_Frequency / 1000000,
           (float)clocks.PCLK1_Frequency / 1000000, (float)clocks.PCLK2_Frequency / 1000000, (float)clocks.ADCCLK_Frequency / 1000000);

    printf("SDIO SD Card Fatfs Test.\n");
    TestList();

    while (1)
    {
        cmd = GetCmd();
        switch (cmd)
        {
        case '1': {
            printf("1.--->>>FatfsTest\r\n");
            FatfsTest();
            TestList();
            break;
        }
        case '2': {
            printf("1.--->>>FatfsBigDataTest\r\n");
            FatfsBigDataTest();
            TestList();
            break;
        }
        case '3': {
            printf("2.--->>>ViewRootDir\r\n");
            ViewRootDir();
            TestList();
            break;
        }
        case '4': {
            printf("3.--->>>CreateDir\r\n");
            CreateDir();
            TestList();
            break;
        }
        case '5': {
            printf("4.--->>>DeleteDirFile\r\n");
            DeleteDirFile();
            TestList();
            break;
        }
        }
    }
}

void TestList(void)
{
    printf("/***************************SD Card Test*******************************/\n");
    printf("==========================List==========================\n");
    printf("1: Create a new file (FatFs read-write test file.txt) for read-write testing\n");
    printf("2: Read and write large amounts of data (FatFs read and write test file .txt), perform read and write tests\n");
    printf("3: Show the file test in the root directory of the SD Card\n");
    printf("4: Create directory(/Dir1,/Dir1/Die1_1,/Dir2)\n");
    printf("5: Delete files and directories (/Dir1,/Dir1/Dir1_1,/Dir2, FatFs read and write test files.txt)\n");
    printf("****************************************************************************/\n");
}

void SDInfoShow(void)
{
    printf("/***************************SD Info Show*******************************/\n");
    printf("SDCardInfo.CardType : %d\n", SDCardInfo.CardType);
    printf("SDCardInfo.CardCapacity : %lld Byte\n", (SDCardInfo.CardCapacity));
    printf("SDCardInfo.CardBlockSize : %d Byte\n", SDCardInfo.CardBlockSize);
}

uint8_t GetCmd(void)
{
    uint8_t tmp = 0;

    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE))
    {
        tmp = USART_ReceiveData(USART1);
    }
    return tmp;
}

void UART_Configuration(uint32_t bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate            = bound;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART_TEST, &USART_InitStructure);
    USART_Cmd(USART_TEST, ENABLE);
}

void FatfsBigDataTest(void)
{
    uint32_t i;

    res_sd = f_mount(&fs, "0:", 1);

    /***************************File system testing --->>> Big data write test*********************/
    printf("\nFile system test --->>> write test\n");
    res_sd = f_open(&fnew, "0:FatFs read and write test files.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res_sd == FR_OK)
    {
        printf("Open/create FatFs to read and write the test file.txt successfully, and write data to the file\r\n");

        for (i = 0; i < 0xFFFFF; i++)
        {
            res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
            if ((i % 0x8FFF) == 0)
            {
                printf("......\n");
            }
        }
        if (res_sd == FR_OK)
        {
            printf("File written successfully\n");
        }
        else
        {
            printf("File write failed (%d)\n", res_sd);
        }
        f_close(&fnew);
    }
    else
    {
        printf("Failed to open/create, file\r\n");
    }
}

void FatfsTest(void)
{
    res_sd = f_mount(&fs, "0:", 1);

    /***************************Format test**************************/
    printf("\n format test\n");
    if (res_sd == FR_NO_FILESYSTEM)
    {
        printf("The SD card has no file system and is about to be formatted\r\n");

        res_sd = f_mkfs("0:", 0, 0);

        if (res_sd == FR_OK)
        {
            printf("The SD card successfully mounted the file system\r\n");
            res_sd = f_mount(NULL, "0:", 1);
            res_sd = f_mount(&fs, "0:", 1);
        }
        else
        {
            printf("SD card formatting failed\r\n");
            while (1);
        }
    }
    else if (res_sd != FR_OK)
    {
        printf("SD card mount failed (%d), maybe SD card initialization failed\r\n", res_sd);
        while (1);
    }
    else
    {
        printf("The file system is mounted and can be read and written for testing\r\n");
    }

    SDInfoShow();
    /***************************File system testing --->>> Write test*********************/
    printf("\n file system test --->>> Write test\n");
    res_sd = f_open(&fnew, "0:FatFs read and write test files.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    if (res_sd == FR_OK)
    {
        printf("Open/create FatFs to read and write the test file.txt successfully, and write data to the file\r\n");

        res_sd = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);

        if (res_sd == FR_OK)
        {
            printf("The file was written successfully, the number of bytes written:% d The data written is: \n%s\r\n", fnum, WriteBuffer);
        }
        else
        {
            printf("File write failed (%d)\n", res_sd);
        }
        f_close(&fnew);
    }
    else
    {
        printf("Failed to open/create, file\r\n");
    }

    /*************************File system testing --->>> read test**************************/
    printf("\n file system test --->>> read test\n");
    res_sd = f_open(&fnew, "0:FatFs read and write test files.txt", FA_OPEN_ALWAYS | FA_READ);
    if (res_sd == FR_OK)
    {
        printf("File successfully opened\r\n");
        res_sd = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (res_sd == FR_OK)
        {
            printf("File read successful. Bytes read:% d The data read was: \n%s\r\n", fnum, ReadBuffer);
        }
        else
        {
            printf("File read failed (%d)\n", res_sd);
        }
    }
    else
    {
        printf("File opening failed\n");
    }

    f_close(&fnew);

    f_mount(NULL, "0:", 1);
}


void CreateDir(void)
{
    res_sd = f_mount(&fs, "0:", 1);
    if (res_sd != FR_OK)
    {
        printf("Failed to mount file system (%d)\r\n", res_sd);
    }

    res_sd = f_mkdir("/Dir1");
    if (res_sd == FR_OK)
    {
        printf("f_mkdir Dir1 OK\r\n");
    }
    else if (res_sd == FR_EXIST)
    {
        printf("Dir1 Target already exists(%d)\r\n", res_sd);
    }
    else
    {
        printf("f_mkdir Dir1 fail(%d)\r\n", res_sd);
        return;
    }

    res_sd = f_mkdir("/Dir2");
    if (res_sd == FR_OK)
    {
        printf("f_mkdir Dir2 OK\r\n");
    }
    else if (res_sd == FR_EXIST)
    {
        printf("Dir2 Target already exists(%d)\r\n", res_sd);
    }
    else
    {
        printf("f_mkdir Dir2 fail (%d)\r\n", res_sd);
        return;
    }

    res_sd = f_mkdir("/Dir1/Dir1_1");
    if (res_sd == FR_OK)
    {
        printf("f_mkdir Dir1_1 OK\r\n");
    }
    else if (res_sd == FR_EXIST)
    {
        printf("Dir1_1 Target already exists(%d)\r\n", res_sd);
    }
    else
    {
        printf("f_mkdir Dir1_1 fail (%d)\r\n", res_sd);
        return;
    }

    f_mount(NULL, "0:", 1);
}

void DeleteDirFile(void)
{
    res_sd = f_mount(&fs, "0:", 1);
    if (res_sd != FR_OK)
    {
        printf("Failed to mount file system (%d)\r\n", res_sd);
    }

    res_sd = f_unlink("/Dir1/Dir1_1");
    if (res_sd == FR_OK)
    {
        printf("Delete subdirectory /Dir1/Dir1_1 success\r\n");
    }
    else if ((res_sd == FR_NO_FILE) || (res_sd == FR_NO_PATH))
    {
        printf("No file or directory found: %s\r\n", "/Dir1/Dir1_1");
    }
    else
    {
        printf("Deleting subdirectory/Dir1/Dir1_1 failed (errcode =% d) File read-only or directory not empty\r\n", res_sd);
    }

    res_sd = f_unlink("/Dir1");
    if (res_sd == FR_OK)
    {
        printf("Delete directory/Dir1 successfully\r\n");
    }
    else if ((res_sd == FR_NO_FILE) || (res_sd == FR_NO_PATH))
    {
        printf("No file or directory found : %s\r\n", "/Dir1");
    }
    else
    {
        printf("Deleting subdirectory/Dir1/Dir1_1 failed (errcode =% d) File read-only or directory not empty\r\n", res_sd);
    }

    res_sd = f_unlink("/Dir2");
    if (res_sd == FR_OK)
    {
        printf("Delete/Dir2 successfully\r\n");
    }
    else if ((res_sd == FR_NO_FILE) || (res_sd == FR_NO_PATH))
    {
        printf("No file or directory found : %s\r\n", "/Dir2");
    }
    else
    {
        printf("Deleting subdirectory/Dir1/Dir1_1 failed (errcode = %d) File read-only or directory is not empty\r\n", res_sd);
    }

    res_sd = f_unlink("FatFs read and write test files.txt");
    if (res_sd == FR_OK)
    {
        printf("Delete FatFs read and write test file.txt successfully\r\n");
    }
    else if ((res_sd == FR_NO_FILE) || (res_sd == FR_NO_PATH))
    {
        printf("No file or directory found : %s\r\n", "/FatFs read and write test files.txt");
    }
    else
    {
        printf("Failed to delete FatFs read and write test file.txt(errcode = %d) File read-only or directory is not empty\r\n", res_sd);
    }
}

void ViewRootDir(void)
{
    DIR      dirinf;
    FILINFO  fileinf;
    uint32_t cnt = 0;
    char     name[256];

    res_sd = f_mount(&fs, "0:", 1);
    if (res_sd != FR_OK)
    {
        printf("Failed to mount file system (%d)\r\n", res_sd);
    }

    res_sd = f_opendir(&dirinf, "/");
    if (res_sd != FR_OK)
    {
        printf("Failed to open root directory (%d)\r\n", res_sd);
        return;
    }

    fileinf.lfname = name;
    fileinf.lfsize = 256;

    printf("attribute		|	file size	|	short filename	|	long file name\r\n");
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

        if (fileinf.fattrib & AM_DIR)
        {
            printf("(0x%02d)directory", fileinf.fattrib);
        }
        else
        {
            printf("(0x%02d)attribute", fileinf.fattrib);
        }

        printf("%10d	", fileinf.fsize);
        printf("	%s |", fileinf.fname);
        printf("	%s\r\n", (char *)fileinf.lfname);
    }

    f_mount(NULL, "0:", 1);
}

int SER_PutChar(int ch)
{
    while (!USART_GetFlagStatus(USART_TEST, USART_FLAG_TC));
    USART_SendData(USART_TEST, (uint8_t)ch);

    return ch;
}

int fputc(int c, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */
    if (c == '\n')
    {
        SER_PutChar('\r');
    }
    return (SER_PutChar(c));
}

