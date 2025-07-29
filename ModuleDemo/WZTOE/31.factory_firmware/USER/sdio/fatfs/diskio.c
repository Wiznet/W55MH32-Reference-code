/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include "diskio.h"
#include "w55mh32.h"
#include "bsp_sdio_sdcard.h"

/* Define a physical number for each device */
#define ATA       0 //SD card
#define SPI_FLASH 1 // Reserve external SPI Flash for use

#define SD_BLOCKSIZE 512

extern SD_CardInfo SDCardInfo;

/*-----------------------------------------------------------------------*/
/* Get device status                                                          */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
    BYTE pdrv /* Physical number */
)
{
    DSTATUS status = STA_NOINIT;

    switch (pdrv)
    {
    case ATA: /* SD CARD */
        status &= ~STA_NOINIT;
        break;

    case SPI_FLASH: /* SPI Flash */
        break;

    default:
        status = STA_NOINIT;
    }
    return status;
}

/*-----------------------------------------------------------------------*/
/* device initialization                                                            */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(
    BYTE pdrv /* Physical number */
)
{
    DSTATUS status = STA_NOINIT;
    switch (pdrv)
    {
    case ATA: /* SD CARD */
        if (SD_Init() == SD_OK)
        {
            status &= ~STA_NOINIT;
        }
        else
        {
            status = STA_NOINIT;
        }

        break;

    case SPI_FLASH: /* SPI Flash */
        break;

    default:
        status = STA_NOINIT;
    }
    return status;
}


/*-----------------------------------------------------------------------*/
/* Read sector: Read sector content to the specified storage area                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read(
    BYTE  pdrv,   /* Device physical number(0..) */
    BYTE *buff,   /* data buffer */
    DWORD sector, /* sector head address */
    UINT  count   /* number of sectors(1..128) */
)
{
    DRESULT  status   = RES_PARERR;
    SD_Error SD_state = SD_OK;

    switch (pdrv)
    {
    case ATA: /* SD CARD */
        if ((DWORD)buff & 3)
        {
            DRESULT res = RES_OK;
            DWORD   scratch[SD_BLOCKSIZE / 4];

            while (count--)
            {
                res = disk_read(ATA, (void *)scratch, sector++, 1);

                if (res != RES_OK)
                {
                    break;
                }
                memcpy(buff, scratch, SD_BLOCKSIZE);
                buff += SD_BLOCKSIZE;
            }
            return res;
        }

        SD_state = SD_ReadMultiBlocks(buff, (uint64_t)sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
        if (SD_state == SD_OK)
        {
            /* Check if the Transfer is finished */
            SD_state = SD_WaitReadOperation();
            while (SD_GetStatus() != SD_TRANSFER_OK);
        }
        if (SD_state != SD_OK)
            status = RES_PARERR;
        else
            status = RES_OK;
        break;

    case SPI_FLASH:
        break;

    default:
        status = RES_PARERR;
    }
    return status;
}


/*-----------------------------------------------------------------------*/
/* Write sector: see Data is written to the specified sector space                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write(
    BYTE        pdrv,   /* Device physical number(0..) */
    const BYTE *buff,   /* Buffer to write data to */
    DWORD       sector, /* sector head address */
    UINT        count   /* number of sectors(1..128) */
)
{
    DRESULT  status   = RES_PARERR;
    SD_Error SD_state = SD_OK;

    if (!count)
    {
        return RES_PARERR; /* Check parameter */
    }

    switch (pdrv)
    {
    case ATA: /* SD CARD */
        if ((DWORD)buff & 3)
        {
            DRESULT res = RES_OK;
            DWORD   scratch[SD_BLOCKSIZE / 4];

            while (count--)
            {
                memcpy(scratch, buff, SD_BLOCKSIZE);
                res = disk_write(ATA, (void *)scratch, sector++, 1);
                if (res != RES_OK)
                {
                    break;
                }
                buff += SD_BLOCKSIZE;
            }
            return res;
        }

        SD_state = SD_WriteMultiBlocks((uint8_t *)buff, (uint64_t)sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
        if (SD_state == SD_OK)
        {
            /* Check if the Transfer is finished */
            SD_state = SD_WaitWriteOperation();

            /* Wait until end of DMA transfer */
            while (SD_GetStatus() != SD_TRANSFER_OK);
        }
        if (SD_state != SD_OK)
            status = RES_PARERR;
        else
            status = RES_OK;
        break;

    case SPI_FLASH:
        break;

    default:
        status = RES_PARERR;
    }
    return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* Other controls                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl(
    BYTE  pdrv, /* Physical number */
    BYTE  cmd,  /* control instruction */
    void *buff  /* Write or read data address pointer */
)
{
    DRESULT status = RES_PARERR;
    switch (pdrv)
    {
    case ATA: /* SD CARD */
        switch (cmd)
        {
        // Get R/W sector size (WORD)
        case GET_SECTOR_SIZE:
            *(WORD *)buff = SD_BLOCKSIZE;
            break;
        // Get erase block size in unit of sector (DWORD)
        case GET_BLOCK_SIZE:
            *(DWORD *)buff = 1;
            break;

        case GET_SECTOR_COUNT:
            *(DWORD *)buff = SDCardInfo.CardCapacity / SDCardInfo.CardBlockSize;
            break;
        case CTRL_SYNC:
            break;
        }
        status = RES_OK;
        break;

    case SPI_FLASH:
        break;

    default:
        status = RES_PARERR;
    }
    return status;
}
#endif


__weak DWORD get_fattime(void)
{
    /* Returns the current timestamp */
    return ((DWORD)(2015 - 1980) << 25) /* Year 2015 */
           | ((DWORD)1 << 21)           /* Month 1 */
           | ((DWORD)1 << 16)           /* Mday 1 */
           | ((DWORD)0 << 11)           /* Hour 0 */
           | ((DWORD)0 << 5)            /* Min 0 */
           | ((DWORD)0 >> 1);           /* Sec 0 */
}

