#include "bsp_sdio_sdcard.h"


/* Private macro -------------------------------------------------------------*/
/** 
  * @brief  SDIO Static flags, TimeOut, FIFO Address  
  */
#define NULL              0
#define SDIO_STATIC_FLAGS ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT  ((uint32_t)0x00010000)

/** 
  * @brief  Mask for errors Card Status R1 (OCR Register) 
  */
#define SD_OCR_ADDR_OUT_OF_RANGE     ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED       ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR         ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR         ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM       ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION  ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED    ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED        ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD           ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED       ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR              ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN  ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN  ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE    ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP         ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED     ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET           ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR         ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS             ((uint32_t)0xFDFFE008)

/** 
  * @brief  Masks for R6 Response 
  */
#define SD_R6_GENERAL_UNKNOWN_ERROR ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD           ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED        ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY     ((uint32_t)0x40000000)
#define SD_STD_CAPACITY      ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN     ((uint32_t)0x000001AA)

#define SD_MAX_VOLT_TRIAL ((uint32_t)0x0000FFFF)
#define SD_ALLZERO        ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT   ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT ((uint32_t)0x00010000)
#define SD_CARD_LOCKED        ((uint32_t)0x02000000)

#define SD_DATATIMEOUT     ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS        ((uint32_t)0x000000FF)
#define SD_8TO15BITS       ((uint32_t)0x0000FF00)
#define SD_16TO23BITS      ((uint32_t)0x00FF0000)
#define SD_24TO31BITS      ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO      ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES ((uint32_t)0x00000020)

/** 
  * @brief  Command Class Supported 
  */
#define SD_CCCC_LOCK_UNLOCK ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PROT  ((uint32_t)0x00000040)
#define SD_CCCC_ERASE       ((uint32_t)0x00000020)

/** 
  * @brief  Following commands are SD Card Specific commands.
  *         SDIO_APP_CMD should be sent before sending these commands. 
  */
#define SDIO_SEND_IF_COND ((uint32_t)0x00000008)


/* Private variables ---------------------------------------------------------*/
static uint32_t CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1; //The type of memory card, first initialize it as a 1.1 protocol card
static uint32_t CSD_Tab[4], CID_Tab[4], RCA = 0;           //Store CSD, DID, registers and card relative addresses
static uint8_t  SDSTATUS_Tab[16];                          //Memory card status, part of CSR
__IO uint32_t   StopCondition = 0;                         //Signs to stop card operation
__IO SD_Error   TransferError = SD_OK;                     //For memory card errors, initialize to normal state
__IO uint32_t   TransferEnd   = 0;                         //Used to flag the end of transmission, called in the interrupt service function
SD_CardInfo     SDCardInfo;                                //Is it used to store card information as part of the DSR

/*Structure for sdio initialization*/
SDIO_InitTypeDef     SDIO_InitStructure;
SDIO_CmdInitTypeDef  SDIO_CmdInitStructure;
SDIO_DataInitTypeDef SDIO_DataInitStructure;


/* Private function prototypes -----------------------------------------------*/
static SD_Error CmdError(void);
static SD_Error CmdResp1Error(uint8_t cmd);
static SD_Error CmdResp7Error(void);
static SD_Error CmdResp3Error(void);
static SD_Error CmdResp2Error(void);
static SD_Error CmdResp6Error(uint8_t cmd, uint16_t *prca);
static SD_Error SDEnWideBus(FunctionalState NewState);
static SD_Error IsCardProgramming(uint8_t *pstatus);
static SD_Error FindSCR(uint16_t rca, uint32_t *pscr);

static void     GPIO_Configuration(void);
static uint32_t SD_DMAEndOfTransferStatus(void);
static void     SD_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize);
static void     SD_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize);

uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes);

/* Private functions ---------------------------------------------------------*/

/*
* Function name: SD_DeInit
* Description: Reset the SDIO port
* Input: None
* Output: None
*/
void SD_DeInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*!< Disable SDIO Clock */
    SDIO_ClockCmd(DISABLE);

    /*!< Set Power State to OFF */
    SDIO_SetPowerState(SDIO_PowerState_OFF);

    /*!< DeInitializes the SDIO peripheral */
    SDIO_DeInit();

    /*!< Disable the SDIO AHB Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, DISABLE);

    /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*!< Configure PD.02 CMD line */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
}

/*
* Function name: NVIC_Configuration
* Description: SDIO priority is configured as the highest priority.
* Input: None
* Output: None
*/
static void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_InitStructure.NVIC_IRQChannel                   = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Returns the DMA End Of Transfer Status.
  * @param  None
  * @retval DMA SDIO Channel Status.
  */
uint32_t SD_DMAEndOfTransferStatus(void)
{
    return (uint32_t)DMA_GetFlagStatus(DMA2_FLAG_TC4); //Channel4 transfer complete flag.
}


/*
* Function name: SD_DMA_RxConfig
* Description: Request to configure channel 4 of DMA2 for SDIO to receive data
* Input: BufferDST: variable pointer for loading data
*: BufferSize: Buffer size
* Output: None
*/
void SD_DMA_RxConfig(uint32_t *BufferDST, uint32_t BufferSize)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4); //Clear DMA flag

    /*!< DMA2 Channel4 disable */
    DMA_Cmd(DMA2_Channel4, DISABLE); //SDIO is the fourth channel

    /*!< DMA2 Channel4 Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS; //Peripheral address, fifo
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)BufferDST;         //destination address
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;       //Peripherals are the original address
    DMA_InitStructure.DMA_BufferSize         = BufferSize / 4;              //1/4 cache size
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;   //Enable peripheral address does not increment
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;        //Enable storage destination address autoincrement
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; //Peripheral data size is word, 32-bit
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;     //Peripheral data size is word, 32-bit
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;             //No loop, loop mode is mainly used on ADC
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;           //High channel priority
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;             //Non-memory to memory mode
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    /*!< DMA2 Channel4 enable */ //Do not set dma interrupt
    DMA_Cmd(DMA2_Channel4, ENABLE);
}

/*
* Function name: SD_DMA_RxConfig
* Description: Send a request for channel 4 of data configuration DMA2 for SDIO
* Input: BufferDST: variable pointer loaded with data
BufferSize: Buffer size
* Output: None
*/
void SD_DMA_TxConfig(uint32_t *BufferSRC, uint32_t BufferSize)
{
    DMA_InitTypeDef DMA_InitStructure;

    DMA_ClearFlag(DMA2_FLAG_TC4 | DMA2_FLAG_TE4 | DMA2_FLAG_HT4 | DMA2_FLAG_GL4);

    /*!< DMA2 Channel4 disable */
    DMA_Cmd(DMA2_Channel4, DISABLE);

    /*!< DMA2 Channel4 Config */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SDIO_FIFO_ADDRESS;
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)BufferSRC;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;     //Peripheral as write target
    DMA_InitStructure.DMA_BufferSize         = BufferSize / 4;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; //Peripheral address does not increase
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    /*!< DMA2 Channel4 enable */
    DMA_Cmd(DMA2_Channel4, ENABLE);
}


/*
* Function name: GPIO_Configuration
* Description: The pins used to initialize SDIO and turn on the clock.
* Input: None
* Output: None
* Call: internal call
*/
static void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*!< GPIOC and GPIOD Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

    /*!< Configure PC.08, PC.09, PC.10, PC.11, PC.12 pin: D0, D1, D2, D3, CLK pin */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*!< Configure PD.02 CMD line */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /*!< Enable the SDIO AHB Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO, ENABLE);

    /*!< Enable the DMA2 Clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
}

/**
* Function name: SD_Init
* Description: Initialize the SD card so that it is ready to transfer data.
* Input: None
* Output: - SD_Error SD card BigInt
* SD_OK when successful
* Call: external call
*/
SD_Error SD_Init(void)
{
    /*Reset SD_Error state*/
    SD_Error errorstatus = SD_OK;

    NVIC_Configuration();

    /* SDIO Peripheral bottom pin initialization */
    GPIO_Configuration();

    /*Reset all SDIO registers*/
    SDIO_DeInit();

    /*Power on and perform the card identification process to confirm the operating voltage of the card  */
    errorstatus = SD_PowerON();

    /*If power on, the identification is unsuccessful, and a "response timed out" error is returned. */
    if (errorstatus != SD_OK)
    {
        /*!< CMD Response TimeOut (wait for CMDSENT flag) */
        return (errorstatus);
    }

    /*Card recognition is successful, and card initialization is performed.    */
    errorstatus = SD_InitializeCards();

    if (errorstatus != SD_OK) //return on failure
    {
        /*!< CMD Response TimeOut (wait for CMDSENT flag) */
        return (errorstatus);
    }

    /* Configure SDIO peripherals
  * After power-on identification and card initialization are completed, enter the data transmission mode to improve the reading and writing speed
  * If the speed exceeds 24M, enter bypass mode
  */

    /* SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_TRANSFER_CLK_DIV) */
    SDIO_InitStructure.SDIO_ClockDiv = SDIO_TRANSFER_CLK_DIV;

    /*Rising edge data collection */
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;

    /* If the clock frequency exceeds 24M, turn on this mode */
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;

    /* Turn off the sd_clk clock when the bus is idle */
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;

    /* Temporarily configured for 1bit mode */
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;

    /* Hardware stream, if turned on, data transmission is suspended when FIFO cannot send and receive data */
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;

    SDIO_Init(&SDIO_InitStructure);

    if (errorstatus == SD_OK)
    {
        /* Used to read csd/cid registers */
        errorstatus = SD_GetCardInfo(&SDCardInfo);
    }

    if (errorstatus == SD_OK)
    {
        /* Select the card to be operated through cmd7, rca */
        errorstatus = SD_SelectDeselect((uint32_t)(SDCardInfo.RCA << 16));
    }

    if (errorstatus == SD_OK)
    {
        /* Finally, in order to improve reading and writing, turn on 4bits mode */
        errorstatus = SD_EnableWideBusOperation(SDIO_BusWide_4b);
    }

    return (errorstatus);
}

/**
  * @brief  Gets the cuurent sd card data transfer status.
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
SDTransferState SD_GetStatus(void)
{
    SDCardState cardstate = SD_CARD_TRANSFER;

    cardstate = SD_GetState();

    if (cardstate == SD_CARD_TRANSFER)
    {
        return (SD_TRANSFER_OK);
    }
    else if (cardstate == SD_CARD_ERROR)
    {
        return (SD_TRANSFER_ERROR);
    }
    else
    {
        return (SD_TRANSFER_BUSY);
    }
}

/**
  * @brief  Returns the current card's state.
  * @param  None
  * @retval SDCardState: SD Card Error or SD Card Current State.
  */
SDCardState SD_GetState(void)
{
    uint32_t resp1 = 0;

    if (SD_SendStatus(&resp1) != SD_OK)
    {
        return SD_CARD_ERROR;
    }
    else
    {
        return (SDCardState)((resp1 >> 9) & 0x0F);
    }
}


/*
* Function name: SD_PowerON
* Description: Ensure the working voltage of the SD card and configure the control clock
* Input: None
* Output: - SD_Error SD card BigInt
* SD_OK when successful
* Call: Call SD_Init ()
*/
SD_Error SD_PowerON(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t response = 0, count = 0, validvoltage = 0;
    uint32_t SDType = SD_STD_CAPACITY;

    /********************************************************************************************************/
    /* Power-on initialization
  * Configure SDIO peripherals
  * SDIOCLK = HCLK, SDIO_CK = HCLK/(2 + SDIO_INIT_CLK_DIV)
  The clock at initialization cannot be greater than 400KHz.
  */
    /* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
    SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;

    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;

    /* Do not use bypass mode, directly use HCLK frequency division to get SDIO_CK */
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;

    /* Do not turn off clock power when idle */
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;

    /* When initializing, temporarily configure the data cable to 1 */
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;

    /* Disable Hardware Flow Control */
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;

    SDIO_Init(&SDIO_InitStructure);

    /* Power on SDIO peripherals */
    SDIO_SetPowerState(SDIO_PowerState_ON);

    /* enable SDIO clock */
    SDIO_ClockCmd(ENABLE);
    /********************************************************************************************************/
    /* Send a series of commands below to start the card recognition process
  * CMD0: GO_IDLE_STATE (reset so SD card goes idle)
  * No corresponding
  */
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE;

    /* No response. */
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;

    /* close pending interrupt */
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;

    /* Then CPSM waits for the end of data transmission before starting to send commands */
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    /* Check if cmd0 is received correctly. */
    errorstatus = CmdError();

    /* Error sending command, return */
    if (errorstatus != SD_OK)
    {
        /* CMD Response TimeOut (wait for CMDSENT flag) */
        return (errorstatus);
    }
    /********************************************************************************************************/
    /* CMD8: SEND_IF_COND 
   * Send CMD8 to verify SD card interface operating condition
	 *          
   * Argument: - [31:12]: Reserved (shall be set to '0')
   *           - [11:8] : Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
   *           - [7:0]  : Check Pattern (recommended 0xAA) 
   * CMD Response: R7 
	 */
    /* Upon receiving the command sd will return this parameter */
    SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;

    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    /*Check if a command has been received.*/
    errorstatus = CmdResp7Error();

    /* If there is a response, the card follows the SD protocol version 2.0 */
    if (errorstatus == SD_OK)
    {
        /* SD Card 2.0 , first define it as a card of type SDSC */
        CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0;

        /* This variable is used as a parameter to ACMD41 to ask whether it is an sdsc card or an sdhc card */
        SDType = SD_HIGH_CAPACITY;
    }
    else /* No response, indicating a 1.x or mmc card */
    {
        /* Give an order CMD55 */
        SDIO_CmdInitStructure.SDIO_Argument = 0x00;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);
        errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
    }

    /* CMD55
  * Send cmd55 to detect whether it is an sd card or an mmc card, or an unsupported card.
  * CMD Response: R1
  */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    /* Whether it responds, the one that does not respond is mmc or an unsupported card. */
    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
    /********************************************************************************************************/
    /* If errorstatus is Command TimeOut, it is a MMC card 
   * If errorstatus is SD_OK it is a SD card: SD card 2.0 (voltage range mismatch)
   * or SD card 1.x 
	 */
    if (errorstatus == SD_OK) //Responded to cmd55, it is an sd card, maybe 1.x, maybe 2.0.
    {
        /* The following starts to cycle the voltage range supported by sdio, and cycle a certain number of times */

        /* SD CARD
     * Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 
		 */
        while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
        {
            /* Before sending the ACMD command, you must first send the CMD55 to the card. 
       * SEND CMD55 APP_CMD with RCA as 0 
			 */
            SDIO_CmdInitStructure.SDIO_Argument = 0x00;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

            if (errorstatus != SD_OK)
            {
                return (errorstatus);
            }

            /* ACMD41
      * Command parameters are composed of supported voltage ranges and HCS bits. The HCS position distinguishes whether the card is SDSC or SDHC.
      * 0: SDSC
      * 1: SDHC
      Response: R3, corresponding to the OCR register
      */
            SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp3Error();

            if (errorstatus != SD_OK)
            {
                return (errorstatus);
            }

            /* If the card demand voltage is within the power supply voltage range of SDIO, it will automatically power on and mark the pwr_up bit
      * Read card register, card status
      */
            response = SDIO_GetResponse(SDIO_RESP1);

            /* Read the pwr_up bit of the card's OCR register to see if it is operating at normal voltage */
            validvoltage = (((response >> 31) == 1) ? 1 : 0);
            count++; /* count cycles */
        }

        if (count >= SD_MAX_VOLT_TRIAL)         /* Cycle detection has not been powered on for more than a certain number of times */
        {
            errorstatus = SD_INVALID_VOLTRANGE; /* SDIO does not support the supply voltage of the card */
            return (errorstatus);
        }

        /* Check the HCS bit in the card return information */
        /* Determine the ccs bit in the ocr, if it is an sdsc card, do not execute the following statement */
        if (response &= SD_HIGH_CAPACITY)
        {
            CardType = SDIO_HIGH_CAPACITY_SD_CARD; /* Change the card type from the initialized sdsc type to sdhc type */
        }

    } /* else MMC Card */

    return (errorstatus);
}

/*
* Function name: SD_PowerOFF
* Description: Turn off the output signal of SDIO
* Input: None
* Output: - SD_Error SD card BigInt
* SD_OK when successful
* Call: external call
*/
SD_Error SD_PowerOFF(void)
{
    SD_Error errorstatus = SD_OK;

    /*!< Set Power State to OFF */
    SDIO_SetPowerState(SDIO_PowerState_OFF);

    return (errorstatus);
}


/*
* Function name: SD_InitializeCards
* Description: Initialize all cards or a single card into a ready state
* Input: None
* Output: - SD_Error SD card BigInt
* SD_OK when successful
* Call: In SD_Init () call, after calling power_on () power card identification is completed, call this function for card initialization
*/
SD_Error SD_InitializeCards(void)
{
    SD_Error errorstatus = SD_OK;
    uint16_t rca         = 0x01;

    if (SDIO_GetPowerState() == SDIO_PowerState_OFF)
    {
        errorstatus = SD_REQUEST_NOT_APPLICABLE;
        return (errorstatus);
    }

    /* Type of judgment card */
    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
    {
        /* Send CMD2 ALL_SEND_CID
    Response: R2, corresponding to CID register
    */
        SDIO_CmdInitStructure.SDIO_Argument = 0x0;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp2Error();

        if (SD_OK != errorstatus)
        {
            return (errorstatus);
        }

        /* Store the returned CID information */
        CID_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
        CID_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
        CID_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
        CID_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
    }
    /********************************************************************************************************/
    if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType)
        || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType)
        || (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType)
        || (SDIO_HIGH_CAPACITY_SD_CARD == CardType)) /* Using a 2.0 card */
    {
        /* Send CMD3 SET_REL_ADDR with argument 0 
     * SD Card publishes its RCA.
     * Response: R6, corresponding to RCA register		
		 */
        SDIO_CmdInitStructure.SDIO_Argument = 0x00;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        /* Store the relative address of the received card */
        errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR, &rca);

        if (SD_OK != errorstatus)
        {
            return (errorstatus);
        }
    }
    /********************************************************************************************************/
    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType)
    {
        RCA = rca;

        /* Send CMD9 SEND_CSD with argument as card's RCA 
		 * Response: R2 corresponding register CSD(Card-Specific Data)
		 */
        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp2Error();

        if (SD_OK != errorstatus)
        {
            return (errorstatus);
        }

        CSD_Tab[0] = SDIO_GetResponse(SDIO_RESP1);
        CSD_Tab[1] = SDIO_GetResponse(SDIO_RESP2);
        CSD_Tab[2] = SDIO_GetResponse(SDIO_RESP3);
        CSD_Tab[3] = SDIO_GetResponse(SDIO_RESP4);
    }
    /********************************************************************************************************/
    /*All cards initialized successfully */
    errorstatus = SD_OK;

    return (errorstatus);
}

/*
* Function name: SD_GetCardInfo
* Description: Get specific information about the SD card
* Input: -cardinfo pointer to SD_CardInfo structure
This structure contains the specific information of the SD card
* Output: - SD_Error SD card BigInt
* SD_OK when successful
* Call: external call
*/
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
    SD_Error errorstatus = SD_OK;
    uint8_t  tmp         = 0;

    cardinfo->CardType = (uint8_t)CardType;
    cardinfo->RCA      = (uint16_t)RCA;

    /*!< Byte 0 */
    tmp                             = (uint8_t)((CSD_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_csd.CSDStruct      = (tmp & 0xC0) >> 6;
    cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;
    cardinfo->SD_csd.Reserved1      = tmp & 0x03;

    /*!< Byte 1 */
    tmp                   = (uint8_t)((CSD_Tab[0] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.TAAC = tmp;

    /*!< Byte 2 */
    tmp                   = (uint8_t)((CSD_Tab[0] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.NSAC = tmp;

    /*!< Byte 3 */
    tmp                            = (uint8_t)(CSD_Tab[0] & 0x000000FF);
    cardinfo->SD_csd.MaxBusClkFrec = tmp;

    /*!< Byte 4 */
    tmp                              = (uint8_t)((CSD_Tab[1] & 0xFF000000) >> 24);
    cardinfo->SD_csd.CardComdClasses = tmp << 4;

    /*!< Byte 5 */
    tmp                               = (uint8_t)((CSD_Tab[1] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4;
    cardinfo->SD_csd.RdBlockLen       = tmp & 0x0F;

    /*!< Byte 6 */
    tmp                              = (uint8_t)((CSD_Tab[1] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.PartBlockRead   = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.DSRImpl         = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.Reserved2       = 0; /*!< Reserved */

    if ((CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0))
    {
        cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;

        /*!< Byte 7 */
        tmp                          = (uint8_t)(CSD_Tab[1] & 0x000000FF);
        cardinfo->SD_csd.DeviceSize |= (tmp) << 2;

        /*!< Byte 8 */
        tmp                          = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);
        cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;

        cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3;
        cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);

        /*!< Byte 9 */
        tmp                                 = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);
        cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5;
        cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2;
        cardinfo->SD_csd.DeviceSizeMul      = (tmp & 0x03) << 1;
        /*!< Byte 10 */
        tmp                             = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);
        cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;

        cardinfo->CardCapacity   = (cardinfo->SD_csd.DeviceSize + 1);
        cardinfo->CardCapacity  *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
        cardinfo->CardBlockSize  = 1 << (cardinfo->SD_csd.RdBlockLen);
        cardinfo->CardCapacity  *= cardinfo->CardBlockSize;
    }
    else if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        /*!< Byte 7 */
        tmp                         = (uint8_t)(CSD_Tab[1] & 0x000000FF);
        cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16;

        /*!< Byte 8 */
        tmp = (uint8_t)((CSD_Tab[2] & 0xFF000000) >> 24);

        cardinfo->SD_csd.DeviceSize |= (tmp << 8);

        /*!< Byte 9 */
        tmp = (uint8_t)((CSD_Tab[2] & 0x00FF0000) >> 16);

        cardinfo->SD_csd.DeviceSize |= (tmp);

        /*!< Byte 10 */
        tmp = (uint8_t)((CSD_Tab[2] & 0x0000FF00) >> 8);

        cardinfo->CardCapacity  = (uint64_t)(cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
        cardinfo->CardBlockSize = 512;
    }


    cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.EraseGrMul  = (tmp & 0x3F) << 1;

    /*!< Byte 11 */
    tmp                               = (uint8_t)(CSD_Tab[2] & 0x000000FF);
    cardinfo->SD_csd.EraseGrMul      |= (tmp & 0x80) >> 7;
    cardinfo->SD_csd.WrProtectGrSize  = (tmp & 0x7F);

    /*!< Byte 12 */
    tmp                                = (uint8_t)((CSD_Tab[3] & 0xFF000000) >> 24);
    cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.ManDeflECC        = (tmp & 0x60) >> 5;
    cardinfo->SD_csd.WrSpeedFact       = (tmp & 0x1C) >> 2;
    cardinfo->SD_csd.MaxWrBlockLen     = (tmp & 0x03) << 2;

    /*!< Byte 13 */
    tmp                                   = (uint8_t)((CSD_Tab[3] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.MaxWrBlockLen       |= (tmp & 0xC0) >> 6;
    cardinfo->SD_csd.WriteBlockPaPartial  = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.Reserved3            = 0;
    cardinfo->SD_csd.ContentProtectAppli  = (tmp & 0x01);

    /*!< Byte 14 */
    tmp                               = (uint8_t)((CSD_Tab[3] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;
    cardinfo->SD_csd.CopyFlag         = (tmp & 0x40) >> 6;
    cardinfo->SD_csd.PermWrProtect    = (tmp & 0x20) >> 5;
    cardinfo->SD_csd.TempWrProtect    = (tmp & 0x10) >> 4;
    cardinfo->SD_csd.FileFormat       = (tmp & 0x0C) >> 2;
    cardinfo->SD_csd.ECC              = (tmp & 0x03);

    /*!< Byte 15 */
    tmp                        = (uint8_t)(CSD_Tab[3] & 0x000000FF);
    cardinfo->SD_csd.CSD_CRC   = (tmp & 0xFE) >> 1;
    cardinfo->SD_csd.Reserved4 = 1;


    /*!< Byte 0 */
    tmp                             = (uint8_t)((CID_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ManufacturerID = tmp;

    /*!< Byte 1 */
    tmp                          = (uint8_t)((CID_Tab[0] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.OEM_AppliID = tmp << 8;

    /*!< Byte 2 */
    tmp                           = (uint8_t)((CID_Tab[0] & 0x000000FF00) >> 8);
    cardinfo->SD_cid.OEM_AppliID |= tmp;

    /*!< Byte 3 */
    tmp                        = (uint8_t)(CID_Tab[0] & 0x000000FF);
    cardinfo->SD_cid.ProdName1 = tmp << 24;

    /*!< Byte 4 */
    tmp                         = (uint8_t)((CID_Tab[1] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdName1 |= tmp << 16;

    /*!< Byte 5 */
    tmp                         = (uint8_t)((CID_Tab[1] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.ProdName1 |= tmp << 8;

    /*!< Byte 6 */
    tmp                         = (uint8_t)((CID_Tab[1] & 0x0000FF00) >> 8);
    cardinfo->SD_cid.ProdName1 |= tmp;

    /*!< Byte 7 */
    tmp                        = (uint8_t)(CID_Tab[1] & 0x000000FF);
    cardinfo->SD_cid.ProdName2 = tmp;

    /*!< Byte 8 */
    tmp                      = (uint8_t)((CID_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdRev = tmp;

    /*!< Byte 9 */
    tmp                     = (uint8_t)((CID_Tab[2] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.ProdSN = tmp << 24;

    /*!< Byte 10 */
    tmp                      = (uint8_t)((CID_Tab[2] & 0x0000FF00) >> 8);
    cardinfo->SD_cid.ProdSN |= tmp << 16;

    /*!< Byte 11 */
    tmp                      = (uint8_t)(CID_Tab[2] & 0x000000FF);
    cardinfo->SD_cid.ProdSN |= tmp << 8;

    /*!< Byte 12 */
    tmp                      = (uint8_t)((CID_Tab[3] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdSN |= tmp;

    /*!< Byte 13 */
    tmp                            = (uint8_t)((CID_Tab[3] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.Reserved1    |= (tmp & 0xF0) >> 4;
    cardinfo->SD_cid.ManufactDate  = (tmp & 0x0F) << 8;

    /*!< Byte 14 */
    tmp                            = (uint8_t)((CID_Tab[3] & 0x0000FF00) >> 8);
    cardinfo->SD_cid.ManufactDate |= tmp;

    /*!< Byte 15 */
    tmp                        = (uint8_t)(CID_Tab[3] & 0x000000FF);
    cardinfo->SD_cid.CID_CRC   = (tmp & 0xFE) >> 1;
    cardinfo->SD_cid.Reserved2 = 1;

    return (errorstatus);
}

/**
  * @brief  Enables wide bus opeartion for the requeseted card if supported by 
  *         card.
  * @param  WideMode: Specifies the SD card wide bus mode. 
  *   This parameter can be one of the following values:
  *     @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
  *     @arg SDIO_BusWide_4b: 4-bit data transfer
  *     @arg SDIO_BusWide_1b: 1-bit data transfer
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_GetCardStatus(SD_CardStatus *cardstatus)
{
    SD_Error errorstatus = SD_OK;
    uint8_t  tmp         = 0;

    errorstatus = SD_SendSDStatus((uint32_t *)SDSTATUS_Tab);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    /*!< Byte 0 */
    tmp                       = (uint8_t)((SDSTATUS_Tab[0] & 0xC0) >> 6);
    cardstatus->DAT_BUS_WIDTH = tmp;

    /*!< Byte 0 */
    tmp                      = (uint8_t)((SDSTATUS_Tab[0] & 0x20) >> 5);
    cardstatus->SECURED_MODE = tmp;

    /*!< Byte 2 */
    tmp                      = (uint8_t)((SDSTATUS_Tab[2] & 0xFF));
    cardstatus->SD_CARD_TYPE = tmp << 8;

    /*!< Byte 3 */
    tmp                       = (uint8_t)((SDSTATUS_Tab[3] & 0xFF));
    cardstatus->SD_CARD_TYPE |= tmp;

    /*!< Byte 4 */
    tmp                                = (uint8_t)(SDSTATUS_Tab[4] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA = tmp << 24;

    /*!< Byte 5 */
    tmp                                 = (uint8_t)(SDSTATUS_Tab[5] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 16;

    /*!< Byte 6 */
    tmp                                 = (uint8_t)(SDSTATUS_Tab[6] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA |= tmp << 8;

    /*!< Byte 7 */
    tmp                                 = (uint8_t)(SDSTATUS_Tab[7] & 0xFF);
    cardstatus->SIZE_OF_PROTECTED_AREA |= tmp;

    /*!< Byte 8 */
    tmp                     = (uint8_t)((SDSTATUS_Tab[8] & 0xFF));
    cardstatus->SPEED_CLASS = tmp;

    /*!< Byte 9 */
    tmp                          = (uint8_t)((SDSTATUS_Tab[9] & 0xFF));
    cardstatus->PERFORMANCE_MOVE = tmp;

    /*!< Byte 10 */
    tmp                 = (uint8_t)((SDSTATUS_Tab[10] & 0xF0) >> 4);
    cardstatus->AU_SIZE = tmp;

    /*!< Byte 11 */
    tmp                    = (uint8_t)(SDSTATUS_Tab[11] & 0xFF);
    cardstatus->ERASE_SIZE = tmp << 8;

    /*!< Byte 12 */
    tmp                     = (uint8_t)(SDSTATUS_Tab[12] & 0xFF);
    cardstatus->ERASE_SIZE |= tmp;

    /*!< Byte 13 */
    tmp                       = (uint8_t)((SDSTATUS_Tab[13] & 0xFC) >> 2);
    cardstatus->ERASE_TIMEOUT = tmp;

    /*!< Byte 13 */
    tmp                      = (uint8_t)((SDSTATUS_Tab[13] & 0x3));
    cardstatus->ERASE_OFFSET = tmp;

    return (errorstatus);
}


/*
* Function name: SD_ EnableWideBusOperation
* Description: Configure the data width of the card (but it depends on whether the card supports it)
* Enter: -WideMode Specifies the data cable width of the SD card
* The details can be configured as follows
* @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
* @arg SDIO_BusWide_4b: 4-bit data transfer
* @arg SDIO_BusWide_1b: 1-bit data transfer (default)
* Output: - SD_Error SD card BigInt
* SD_OK when successful
* Call: external call
*/
SD_Error SD_EnableWideBusOperation(uint32_t WideMode)
{
    SD_Error errorstatus = SD_OK;

    /*!< MMC Card doesn't support this feature */
    if (SDIO_MULTIMEDIA_CARD == CardType)
    {
        errorstatus = SD_UNSUPPORTED_FEATURE;
        return (errorstatus);
    }
    else if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
        if (SDIO_BusWide_8b == WideMode) //2.0 SD does not support 8bits.
        {
            errorstatus = SD_UNSUPPORTED_FEATURE;
            return (errorstatus);
        }
        else if (SDIO_BusWide_4b == WideMode)  //4 data cable mode
        {
            errorstatus = SDEnWideBus(ENABLE); //Use acmd6 to set the bus width and the transmission method of the card

            if (SD_OK == errorstatus)
            {
                /*!< Configure the SDIO peripheral */
                SDIO_InitStructure.SDIO_ClockDiv            = SDIO_TRANSFER_CLK_DIV;
                SDIO_InitStructure.SDIO_ClockEdge           = SDIO_ClockEdge_Rising;
                SDIO_InitStructure.SDIO_ClockBypass         = SDIO_ClockBypass_Disable;
                SDIO_InitStructure.SDIO_ClockPowerSave      = SDIO_ClockPowerSave_Disable;
                SDIO_InitStructure.SDIO_BusWide             = SDIO_BusWide_4b; //This is the transmission method of setting the sdio of w55mh32. The switching mode must correspond to both the card and the sdio.
                SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
                SDIO_Init(&SDIO_InitStructure);
            }
        }
        else //Single data cable mode
        {
            errorstatus = SDEnWideBus(DISABLE);

            if (SD_OK == errorstatus)
            {
                /*!< Configure the SDIO peripheral */
                SDIO_InitStructure.SDIO_ClockDiv            = SDIO_TRANSFER_CLK_DIV;
                SDIO_InitStructure.SDIO_ClockEdge           = SDIO_ClockEdge_Rising;
                SDIO_InitStructure.SDIO_ClockBypass         = SDIO_ClockBypass_Disable;
                SDIO_InitStructure.SDIO_ClockPowerSave      = SDIO_ClockPowerSave_Disable;
                SDIO_InitStructure.SDIO_BusWide             = SDIO_BusWide_1b;
                SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
                SDIO_Init(&SDIO_InitStructure);
            }
        }
    }

    return (errorstatus);
}

/*
* Function name: SD_SelectDeselect
* Description: Use cmd7 to select the card with the relative address of addr, and cancel the selection of other cards
* If addr = 0, deselect all cards
* Enter: -addr to select the address of the card
* Output: - SD_Error SD card BigInt
* SD_OK when successful
* Call: external call
*/
SD_Error SD_SelectDeselect(uint32_t addr)
{
    SD_Error errorstatus = SD_OK;

    /*!< Send CMD7 SDIO_SEL_DESEL_CARD */
    SDIO_CmdInitStructure.SDIO_Argument = addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SEL_DESEL_CARD);

    return (errorstatus);
}

/**
  * @brief  Allows to read one block from a specified address in a card. The Data
  *         transfer can be managed by DMA mode or Polling mode. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.            
  * @param  readbuff: pointer to the buffer that will contain the received data
  * @param  ReadAddr: Address from where data are to be read.  
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ReadBlock(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize)
{
    SD_Error errorstatus = SD_OK;
#if defined(SD_POLLING_MODE)
    uint32_t count = 0, *tempbuff = (uint32_t *)readbuff;
#endif

    TransferError = SD_OK;
    TransferEnd   = 0; //Transmission end flag, set at interrupt service
    StopCondition = 0;

    SDIO->DCTRL = 0x0;


    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        BlockSize  = 512;
        ReadAddr  /= 512;
    }
    /*******************add, without this section is easy to get stuck in DMA detection *************************************/
    /* Set Block Size for Card, cmd16,
  * If it is an SDSC card, it can be used to set the block size.
  * For SDHCs, the block size is 512 bytes and is not affected by cmd16
  */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r1
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if (SD_OK != errorstatus)
    {
        return (errorstatus);
    }
    /*********************************************************************************/
    SDIO_DataInitStructure.SDIO_DataTimeOut   = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength    = BlockSize;
    SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t)9 << 4;
    SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /*!< Send CMD17 READ_SINGLE_BLOCK */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

#if defined(SD_POLLING_MODE)
    /*!< In case of single block transfer, no need of stop transfer at all.*/
    /*!< Polling mode */
    while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
    {
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
        {
            for (count = 0; count < 8; count++)
            {
                *(tempbuff + count) = SDIO_ReadData();
            }
            tempbuff += 8;
        }
    }

    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        errorstatus = SD_DATA_TIMEOUT;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        errorstatus = SD_DATA_CRC_FAIL;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        errorstatus = SD_RX_OVERRUN;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        errorstatus = SD_START_BIT_ERR;
        return (errorstatus);
    }
    while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
    {
        *tempbuff = SDIO_ReadData();
        tempbuff++;
    }

    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

#elif defined(SD_DMA_MODE)
    SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);
    SDIO_DMACmd(ENABLE);
    SD_DMA_RxConfig((uint32_t *)readbuff, BlockSize);
#endif

    return (errorstatus);
}

/**
  * @brief  Allows to read blocks from a specified address  in a card.  The Data
  *         transfer can be managed by DMA mode or Polling mode. //In two modes
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.	   //In DMA mode, the following two functions are called
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer. 
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.   
  * @param  readbuff: pointer to the buffer that will contain the received data.
  * @param  ReadAddr: Address from where data are to be read.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be read.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_ReadMultiBlocks(uint8_t *readbuff, uint64_t ReadAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
    SD_Error errorstatus = SD_OK;
    TransferError        = SD_OK;
    TransferEnd          = 0;
    StopCondition        = 1;

    SDIO->DCTRL = 0x0;                          //Reset data control register

    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) //The address of the SDHC card is in blocks, each block is 512 bytes
    {
        BlockSize  = 512;
        ReadAddr  /= 512;
    }

    /*!< Set Block Size for Card，cmd16,If it is an sdsc card, it can be used to set the block size. If it is an sdhc card, the block size is 512 bytes and is not affected by cmd16. */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r1
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if (SD_OK != errorstatus)
    {
        return (errorstatus);
    }

    SDIO_DataInitStructure.SDIO_DataTimeOut   = SD_DATATIMEOUT;             //wait timeout limit
    SDIO_DataInitStructure.SDIO_DataLength    = NumberOfBlocks * BlockSize; //For block data transfers, the value in the data length register must be a multiple of the data block length (see SDIO_DCTRL)
    SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t)9 << 4;           //SDIO_DataBlockSize_512b
    SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToSDIO;    //Transmission direction
    SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;    //transmission mode
    SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;           //Turn on the data finite-state machine
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /*!< Send CMD18 READ_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)ReadAddr;  //starting address
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r1
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_READ_MULT_BLOCK);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);                              //Turn on data transfer end interrupt ，Data end (data counter, SDIDCOUNT, is zero) interrupt
    SDIO_DMACmd(ENABLE);                                                 //Enable dma mode
    SD_DMA_RxConfig((uint32_t *)readbuff, (NumberOfBlocks * BlockSize)); //Configure DMA Receive

    return (errorstatus);
}

/**
  * @brief  This function waits until the SDIO DMA data transfer is finished. 
  *         This function should be called after SDIO_ReadMultiBlocks() function
  *         to insure that all data sent by the card are already transferred by 
  *         the DMA controller.        
  * @param  None.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WaitReadOperation(void)
{
    SD_Error errorstatus = SD_OK;
    //Wait for the DMA transfer to end
    while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
    {
    }

    if (TransferError != SD_OK)
    {
        return (TransferError);
    }

    return (errorstatus);
}

/**
  * @brief  Allows to write one block starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode or Polling mode.
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.      
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  WriteAddr: Address from where data are to be read.   
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WriteBlock(uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize)
{
    SD_Error errorstatus = SD_OK;

#if defined(SD_POLLING_MODE)
    uint32_t  bytestransferred = 0, count = 0, restwords = 0;
    uint32_t *tempbuff = (uint32_t *)writebuff;
#endif

    TransferError = SD_OK;
    TransferEnd   = 0;
    StopCondition = 0;

    SDIO->DCTRL = 0x0;


    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        BlockSize  = 512;
        WriteAddr /= 512;
    }

    /*-------------- add, without this section is easy to get stuck in DMA detection -------------------*/
    /* Set Block Size for Card, cmd16,
  * If it is an SDSC card, it can be used to set the block size.
  * For SDHCs, the block size is 512 bytes and is not affected by cmd16
  */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if (SD_OK != errorstatus)
    {
        return (errorstatus);
    }
    /*********************************************************************************/

    /*!< Send CMD24 WRITE_SINGLE_BLOCK */
    SDIO_CmdInitStructure.SDIO_Argument = WriteAddr;           //write address
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r1
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    //Configure the write data register of sdio
    SDIO_DataInitStructure.SDIO_DataTimeOut   = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength    = BlockSize;
    SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t)9 << 4;        //This parameter can be used instead of SDIO_DataBlockSize_512b
    SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToCard; //Write the data.
    SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;        //Open data channel finite-state machine
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /*!< In case of single data block transfer no need of stop command at all */
#if defined(SD_POLLING_MODE) //Normal mode
    while (!(SDIO->STA & (SDIO_FLAG_DBCKEND | SDIO_FLAG_TXUNDERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_STBITERR)))
    {
        if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)
        {
            if ((512 - bytestransferred) < 32)
            {
                restwords = ((512 - bytestransferred) % 4 == 0) ? ((512 - bytestransferred) / 4) : ((512 - bytestransferred) / 4 + 1);
                for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
                {
                    SDIO_WriteData(*tempbuff);
                }
            }
            else
            {
                for (count = 0; count < 8; count++)
                {
                    SDIO_WriteData(*(tempbuff + count));
                }
                tempbuff         += 8;
                bytestransferred += 32;
            }
        }
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        errorstatus = SD_DATA_TIMEOUT;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        errorstatus = SD_DATA_CRC_FAIL;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
        errorstatus = SD_TX_UNDERRUN;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        errorstatus = SD_START_BIT_ERR;
        return (errorstatus);
    }
#elif defined(SD_DMA_MODE) //DMA mode
    SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);            //Data transfer terminated
    SD_DMA_TxConfig((uint32_t *)writebuff, BlockSize); //Configure dma, similar to rx
    SDIO_DMACmd(ENABLE);                               //	Enable dma requests for sdio
#endif

    return (errorstatus);
}

/**
  * @brief  Allows to write blocks starting from a specified address in a card.
  *         The Data transfer can be managed by DMA mode only. 
  * @note   This operation should be followed by two functions to check if the 
  *         DMA Controller and SD Card status.
  *          - SD_ReadWaitOperation(): this function insure that the DMA
  *            controller has finished all data transfer.
  *          - SD_GetStatus(): to check that the SD Card has finished the 
  *            data transfer and it is ready for data.     
  * @param  WriteAddr: Address from where data are to be read.
  * @param  writebuff: pointer to the buffer that contain the data to be transferred.
  * @param  BlockSize: the SD card Data block size. The Block size should be 512.
  * @param  NumberOfBlocks: number of blocks to be written.
  * @retval SD_Error: SD Card Error code.
  */

/*
* Function name: SD_WriteMultiBlocks
* Description: From the starting address of the input, write multiple data blocks to the card,
This function can only be used in DMA mode
Attention: Be sure to call this function after calling it
SD_WaitWriteOperation () to wait for DMA transfer to end
And SD_GetStatus () check whether the transfer between the card and the FIFO of the SDIO has been completed
* Enter:
* @param WriteAddr: Address from where data are to be read.
* @param writebuff: pointer to the buffer that contains the data to be transferred.
* @param BlockSize: the SD card Data block size. The Block size should be 512.
* @param NumberOfBlocks: number of blocks to be written.
Output: SD error type
*/
SD_Error SD_WriteMultiBlocks(uint8_t *writebuff, uint64_t WriteAddr, uint16_t BlockSize, uint32_t NumberOfBlocks)
{
    SD_Error      errorstatus = SD_OK;
    __IO uint32_t count       = 0;

    TransferError = SD_OK;
    TransferEnd   = 0;
    StopCondition = 1;

    SDIO->DCTRL = 0x0;

    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        BlockSize  = 512;
        WriteAddr /= 512;
    }

    /*******************add, without this section is easy to get stuck in DMA detection *************************************/
    /*! < Set Block Size for Card, cmd16, if it is an sdsc card, it can be used to set the block size, if it is an sdhc card, the block size is 512 bytes, not affected by cmd16 */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)BlockSize;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r1
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if (SD_OK != errorstatus)
    {
        return (errorstatus);
    }
    /*********************************************************************************/

    /*!< To improve performance  */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(RCA << 16);
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD; // cmd55
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);


    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }
    /*!< To improve performance */                                  //  pre-erasedThis command can be sent to pre-erase when writing multiple blocks
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)NumberOfBlocks; //The parameter is the number of blocks to be written
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;   //cmd23
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }


    /*!< Send CMD25 WRITE_MULT_BLOCK with argument data address */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)WriteAddr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);

    if (SD_OK != errorstatus)
    {
        return (errorstatus);
    }

    SDIO_DataInitStructure.SDIO_DataTimeOut   = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength    = NumberOfBlocks * BlockSize;
    SDIO_DataInitStructure.SDIO_DataBlockSize = (uint32_t)9 << 4;
    SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    SDIO_ITConfig(SDIO_IT_DATAEND, ENABLE);
    SDIO_DMACmd(ENABLE);
    SD_DMA_TxConfig((uint32_t *)writebuff, (NumberOfBlocks * BlockSize));

    return (errorstatus);
}

/**
  * @brief  This function waits until the SDIO DMA data transfer is finished. 
  *         This function should be called after SDIO_WriteBlock() and
  *         SDIO_WriteMultiBlocks() function to insure that all data sent by the 
  *         card are already transferred by the DMA controller.        
  * @param  None.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_WaitWriteOperation(void)
{
    SD_Error errorstatus = SD_OK;
    //Wait for DMA to end transmission
    while ((SD_DMAEndOfTransferStatus() == RESET) && (TransferEnd == 0) && (TransferError == SD_OK))
    {
    }

    if (TransferError != SD_OK)
    {
        return (TransferError);
    }

    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    return (errorstatus);
}

/**
  * @brief  Gets the cuurent data transfer state.
  * @param  None
  * @retval SDTransferState: Data Transfer state.
  *   This value can be: 
  *        - SD_TRANSFER_OK: No data transfer is acting
  *        - SD_TRANSFER_BUSY: Data transfer is acting
  */
SDTransferState SD_GetTransferState(void)
{
    if (SDIO->STA & (SDIO_FLAG_TXACT | SDIO_FLAG_RXACT))
    {
        return (SD_TRANSFER_BUSY);
    }
    else
    {
        return (SD_TRANSFER_OK);
    }
}

/**
  * @brief  Aborts an ongoing data transfer.
  * @param  None
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_StopTransfer(void)
{
    SD_Error errorstatus = SD_OK;

    /*!< Send CMD12 STOP_TRANSMISSION  */
    SDIO->ARG   = 0x0;
    SDIO->CMD   = 0x44C;
    errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);

    return (errorstatus);
}

/**
  * @brief  Allows to erase memory area specified for the given card.
  * @param  startaddr: the start address.
  * @param  endaddr: the end address.
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_Erase(uint32_t startaddr, uint32_t endaddr)
{
    SD_Error      errorstatus = SD_OK;
    uint32_t      delay       = 0;
    __IO uint32_t maxdelay    = 0;
    uint8_t       cardstate   = 0;

    /*!< Check if the card coomnd class supports erase command */
    if (((CSD_Tab[1] >> 20) & SD_CCCC_ERASE) == 0)
    {
        errorstatus = SD_REQUEST_NOT_APPLICABLE;
        return (errorstatus);
    }

    maxdelay = 120000 / ((SDIO->CLKCR & 0xFF) + 2);    //Delay, calculated according to the clock division setting

    if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) //Card is locked
    {
        errorstatus = SD_LOCK_UNLOCK_FAILED;
        return (errorstatus);
    }

    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) //SDHC card, why /512? See 2.0 protocol page52 for details
    {                                           //On the SDSC card, the address parameter is the block address, each block is 512 bytes, and the SDSC card address is the byte address.
        startaddr /= 512;
        endaddr   /= 512;
    }

    /*!< According to sd-card spec 1.0 ERASE_GROUP_START (CMD32) and erase_group_end(CMD33) */
    if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
        /*!< Send CMD32 SD_ERASE_GRP_START with argument as addr  */
        SDIO_CmdInitStructure.SDIO_Argument = startaddr;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_ERASE_GRP_START;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //R1
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_START);
        if (errorstatus != SD_OK)
        {
            return (errorstatus);
        }

        /*!< Send CMD33 SD_ERASE_GRP_END with argument as addr  */
        SDIO_CmdInitStructure.SDIO_Argument = endaddr;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_ERASE_GRP_END;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_SD_ERASE_GRP_END);
        if (errorstatus != SD_OK)
        {
            return (errorstatus);
        }
    }

    /*!< Send CMD38 ERASE */
    SDIO_CmdInitStructure.SDIO_Argument = 0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ERASE;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_ERASE);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    for (delay = 0; delay < maxdelay; delay++)
    {
    }

    /*!< Wait till the card is in programming state */
    errorstatus = IsCardProgramming(&cardstate);

    while ((errorstatus == SD_OK) && ((SD_CARD_PROGRAMMING == cardstate) || (SD_CARD_RECEIVING == cardstate)))
    {
        errorstatus = IsCardProgramming(&cardstate);
    }

    return (errorstatus);
}

/**
  * @brief  Returns the current card's status.
  * @param  pcardstatus: pointer to the buffer that will contain the SD card 
  *         status (Card Status register).
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
    SD_Error errorstatus = SD_OK;

    SDIO->ARG = (uint32_t)RCA << 16;
    SDIO->CMD = 0x44D;

    errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    *pcardstatus = SDIO->RESP1;
    return (errorstatus);
}

/**
  * @brief  Returns the current SD card's status.
  * @param  psdstatus: pointer to the buffer that will contain the SD card status 
  *         (SD Status register).
  * @retval SD_Error: SD Card Error code.
  */
SD_Error SD_SendSDStatus(uint32_t *psdstatus)
{
    SD_Error errorstatus = SD_OK;
    uint32_t count       = 0;

    if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED)
    {
        errorstatus = SD_LOCK_UNLOCK_FAILED;
        return (errorstatus);
    }

    /*!< Set block size for card if it is not equal to current block size for card. */
    SDIO_CmdInitStructure.SDIO_Argument = 64;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    /*!< CMD55 */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    SDIO_DataInitStructure.SDIO_DataTimeOut   = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength    = 64;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_64b;
    SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /*!< Send ACMD13 SD_APP_STAUS  with argument as card's RCA.*/
    SDIO_CmdInitStructure.SDIO_Argument = 0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_STAUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);
    errorstatus = CmdResp1Error(SD_CMD_SD_APP_STAUS);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
    {
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
        {
            for (count = 0; count < 8; count++)
            {
                *(psdstatus + count) = SDIO_ReadData();
            }
            psdstatus += 8;
        }
    }

    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        errorstatus = SD_DATA_TIMEOUT;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        errorstatus = SD_DATA_CRC_FAIL;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        errorstatus = SD_RX_OVERRUN;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        errorstatus = SD_START_BIT_ERR;
        return (errorstatus);
    }

    while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
    {
        *psdstatus = SDIO_ReadData();
        psdstatus++;
    }

    /*!< Clear all the static status flags*/
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    return (errorstatus);
}

/*
* Function name: SD_ProcessIRQSrc
* Description: Data transfer is interrupted
* Input: None
Output: SD error type
*/
SD_Error SD_ProcessIRQSrc(void)
{
    if (StopCondition == 1) //When did you set 1?
    {
        SDIO->ARG = 0x0;    //Command parameter register
        SDIO->CMD = 0x44C;  // Command Register: 0100 	01 	 	001100
                            //						[7:6]  	[5:0]
                            //				CPSMEN  WAITRESP CMDINDEX
                            //		Turn on command fine-state machine short response   cmd12 STOP_ TRANSMISSION
        TransferError = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
    }
    else
    {
        TransferError = SD_OK;
    }
    SDIO_ClearITPendingBit(SDIO_IT_DATAEND); //clear interrupt
    SDIO_ITConfig(SDIO_IT_DATAEND, DISABLE); //Turn off sdio interrupt enable
    TransferEnd = 1;
    return (TransferError);
}


/*
  * Function name: CmdError
  * Description: A check for the CMD0 command.
  * Input: None
  Output: SD error type
  */
static SD_Error CmdError(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t timeout;

    timeout = SDIO_CMD0TIMEOUT; /*!< 10000 */

    /*Check if the command has been sent*/
    while ((timeout > 0) && (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) == RESET))
    {
        timeout--;
    }

    if (timeout == 0)
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        return (errorstatus);
    }

    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS); //Clear static flag

    return (errorstatus);
}


/*
* Function name: CmdResp7Error
* Description: Check for a command with a response type of R7
* Input: None
Output: SD error type
*/
static SD_Error CmdResp7Error(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t timeout = SDIO_CMD0TIMEOUT;

    status = SDIO->STA; //Read the SDIO status register, which is the register of stm32

    /* Command response received (CRC check failed) ：Command response received (CRC check passed)：Command response timeout */

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)) && (timeout > 0))
    {
        timeout--;
        status = SDIO->STA;
    }
    //Card not responding to cmd8
    if ((timeout == 0) || (status & SDIO_FLAG_CTIMEOUT))
    {
        /*!< Card is not V2.0 complient or card does not support the set voltage range */
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return (errorstatus);
    }

    if (status & SDIO_FLAG_CMDREND)
    {
        /*!< Card is SD V2.0 compliant */
        errorstatus = SD_OK;
        SDIO_ClearFlag(SDIO_FLAG_CMDREND);
        return (errorstatus);
    }
    return (errorstatus);
}


/*
  * Function name: CmdResp1Error
  * Description: Check for commands with response type R1
  * Input: None
  Output: SD error type
  */
static SD_Error CmdResp1Error(uint8_t cmd)
{
    /*Not these states just wait	*/
    while (!(SDIO->STA & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
    }

    SDIO->ICR = SDIO_STATIC_FLAGS;                     //clear interrupt flag

    return (SD_Error)(SDIO->RESP1 & SD_OCR_ERRORBITS); //Determine whether it is within the power supply range
}

/*
  * Function name: CmdResp3Error
  * Description: Check for commands with response type R3
  * Input: None
  Output: SD error type
  */
static SD_Error CmdResp3Error(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return (errorstatus);
    }
    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return (errorstatus);
}

/*
  * Function name: CmdResp2Error
  * Description: Check for commands with response type R2
  * Input: None
  Output: SD error type
  */
static SD_Error CmdResp2Error(void)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return (errorstatus);
    }
    else if (status & SDIO_FLAG_CCRCFAIL)
    {
        errorstatus = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return (errorstatus);
    }

    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    return (errorstatus);
}

/*
  * Function name: CmdResp6Error
  * Description: Check for commands with response type R6
  * Enter: cmd command index number,
  Prca is used to store the received card relative address
  Output: SD error type
  */
static SD_Error CmdResp6Error(uint8_t cmd, uint16_t *prca)
{
    SD_Error errorstatus = SD_OK;
    uint32_t status;
    uint32_t response_r1;

    status = SDIO->STA;

    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_CMDREND)))
    {
        status = SDIO->STA;
    }

    if (status & SDIO_FLAG_CTIMEOUT)
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return (errorstatus);
    }
    else if (status & SDIO_FLAG_CCRCFAIL)
    {
        errorstatus = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return (errorstatus);
    }

    /*!< Check response received is of desired command */
    if (SDIO_GetCommandResponse() != cmd) //Detect whether a normal command has been received.
    {
        errorstatus = SD_ILLEGAL_CMD;
        return (errorstatus);
    }

    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    /*!< We have received response, retrieve it.  */
    response_r1 = SDIO_GetResponse(SDIO_RESP1);

    /*The following status is all 0 to indicate that the RCA returned by the card was successfully received */
    if (SD_ALLZERO == (response_r1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
    {
        *prca = (uint16_t)(response_r1 >> 16); //Shift 16 bits to the right, which is the received return rca.
        return (errorstatus);
    }

    if (response_r1 & SD_R6_GENERAL_UNKNOWN_ERROR)
    {
        return (SD_GENERAL_UNKNOWN_ERROR);
    }

    if (response_r1 & SD_R6_ILLEGAL_CMD)
    {
        return (SD_ILLEGAL_CMD);
    }

    if (response_r1 & SD_R6_COM_CRC_FAILED)
    {
        return (SD_COM_CRC_FAILED);
    }

    return (errorstatus);
}

/*
* Function name: SDEnWideBus
* Description: Enable or disable 4-bit mode of SDIO
* Input: New state ENABLE or DISABLE
Output: SD error type
*/
static SD_Error SDEnWideBus(FunctionalState NewState)
{
    SD_Error errorstatus = SD_OK;

    uint32_t scr[2] = {0, 0};

    if (SDIO_GetResponse(SDIO_RESP1) & SD_CARD_LOCKED) //Check if the card is locked
    {
        errorstatus = SD_LOCK_UNLOCK_FAILED;
        return (errorstatus);
    }

    /*!< Get SCR Register */
    errorstatus = FindSCR(RCA, scr); //Get the contents of the scr register into the scr array

    if (errorstatus != SD_OK)        //degug,CRC error, scr can't read the value
    {
        return (errorstatus);
    }

    /*!< If wide bus operation to be enabled */
    if (NewState == ENABLE)
    {
        /*!< If requested card supports wide bus operation */
        if ((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO) //Determine whether the card supports 4-bit mode
        {
            /*!< Send CMD55 APP_CMD with argument as card's RCA.*/
            SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

            if (errorstatus != SD_OK)
            {
                return (errorstatus);
            }

            /*!< Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
            /*Enable the command acmd6 in 4bit mode*/
            SDIO_CmdInitStructure.SDIO_Argument = 0x2;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);

            if (errorstatus != SD_OK)
            {
                return (errorstatus);
            }
            return (errorstatus);
        }
        else
        {
            errorstatus = SD_REQUEST_NOT_APPLICABLE;
            return (errorstatus);
        }
    } /*!< If wide bus operation to be disabled */
    else
    {
        /*!< If requested card supports 1 bit mode operation */
        if ((scr[1] & SD_SINGLE_BUS_SUPPORT) != SD_ALLZERO)
        {
            /*!< Send CMD55 APP_CMD with argument as card's RCA.*/
            SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);


            errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

            if (errorstatus != SD_OK)
            {
                return (errorstatus);
            }

            /*!< Send ACMD6 APP_CMD with argument as 0 for single bus mode */
            SDIO_CmdInitStructure.SDIO_Argument = 0x00;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);

            if (errorstatus != SD_OK)
            {
                return (errorstatus);
            }

            return (errorstatus);
        }
        else
        {
            errorstatus = SD_REQUEST_NOT_APPLICABLE;
            return (errorstatus);
        }
    }
}


/*
* Function name: IsCardProgramming
* Description: Detect whether the SD card is performing internal read and write operations
* Input: Pointer to load SD state
Output: SD error type
*/
static SD_Error IsCardProgramming(uint8_t *pstatus)
{
    SD_Error      errorstatus = SD_OK;
    __IO uint32_t respR1 = 0, status = 0;

    /* cmd13 lets the card send the card status register, which is stored in the sdio_sta register */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16; //Card relative address parameters
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    status = SDIO->STA;
    while (!(status & (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CMDREND | SDIO_FLAG_CTIMEOUT)))
    {
        status = SDIO->STA;
    }
    /* A series of status judgments */
    if (status & SDIO_FLAG_CTIMEOUT)
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return (errorstatus);
    }
    else if (status & SDIO_FLAG_CCRCFAIL)
    {
        errorstatus = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return (errorstatus);
    }

    status = (uint32_t)SDIO_GetCommandResponse();

    /*!< Check response received is of desired command */
    if (status != SD_CMD_SEND_STATUS)
    {
        errorstatus = SD_ILLEGAL_CMD;
        return (errorstatus);
    }

    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);


    /*!< We have received response, retrieve it for analysis  */
    respR1 = SDIO_GetResponse(SDIO_RESP1);

    /*!< Find out card status */
    *pstatus = (uint8_t)((respR1 >> 9) & 0x0000000F); //status[12:9] :cardstate

    if ((respR1 & SD_OCR_ERRORBITS) == SD_ALLZERO)
    {
        return (errorstatus);
    }

    if (respR1 & SD_OCR_ADDR_OUT_OF_RANGE)
    {
        return (SD_ADDR_OUT_OF_RANGE);
    }

    if (respR1 & SD_OCR_ADDR_MISALIGNED)
    {
        return (SD_ADDR_MISALIGNED);
    }

    if (respR1 & SD_OCR_BLOCK_LEN_ERR)
    {
        return (SD_BLOCK_LEN_ERR);
    }

    if (respR1 & SD_OCR_ERASE_SEQ_ERR)
    {
        return (SD_ERASE_SEQ_ERR);
    }

    if (respR1 & SD_OCR_BAD_ERASE_PARAM)
    {
        return (SD_BAD_ERASE_PARAM);
    }

    if (respR1 & SD_OCR_WRITE_PROT_VIOLATION)
    {
        return (SD_WRITE_PROT_VIOLATION);
    }

    if (respR1 & SD_OCR_LOCK_UNLOCK_FAILED)
    {
        return (SD_LOCK_UNLOCK_FAILED);
    }

    if (respR1 & SD_OCR_COM_CRC_FAILED)
    {
        return (SD_COM_CRC_FAILED);
    }

    if (respR1 & SD_OCR_ILLEGAL_CMD)
    {
        return (SD_ILLEGAL_CMD);
    }

    if (respR1 & SD_OCR_CARD_ECC_FAILED)
    {
        return (SD_CARD_ECC_FAILED);
    }

    if (respR1 & SD_OCR_CC_ERROR)
    {
        return (SD_CC_ERROR);
    }

    if (respR1 & SD_OCR_GENERAL_UNKNOWN_ERROR)
    {
        return (SD_GENERAL_UNKNOWN_ERROR);
    }

    if (respR1 & SD_OCR_STREAM_READ_UNDERRUN)
    {
        return (SD_STREAM_READ_UNDERRUN);
    }

    if (respR1 & SD_OCR_STREAM_WRITE_OVERRUN)
    {
        return (SD_STREAM_WRITE_OVERRUN);
    }

    if (respR1 & SD_OCR_CID_CSD_OVERWRIETE)
    {
        return (SD_CID_CSD_OVERWRITE);
    }

    if (respR1 & SD_OCR_WP_ERASE_SKIP)
    {
        return (SD_WP_ERASE_SKIP);
    }

    if (respR1 & SD_OCR_CARD_ECC_DISABLED)
    {
        return (SD_CARD_ECC_DISABLED);
    }

    if (respR1 & SD_OCR_ERASE_RESET)
    {
        return (SD_ERASE_RESET);
    }

    if (respR1 & SD_OCR_AKE_SEQ_ERROR)
    {
        return (SD_AKE_SEQ_ERROR);
    }

    return (errorstatus);
}


/*
  * Function name: FindSCR
  * Description: Read the contents of the SD card's SCR register
  * Enter: RCA card relative address
  Pscr pointer to load SCR content
  Output: SD error type
  */
static SD_Error FindSCR(uint16_t rca, uint32_t *pscr)
{
    uint32_t index       = 0;
    SD_Error errorstatus = SD_OK;
    uint32_t tempscr[2]  = {0, 0};

    /*!< Set Block Size To 8 Bytes */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)8;         //If the block size is an sdhc card, the block size cannot be changed//original parameter 8
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN; //	 cmd16
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r1
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    /*!< Send CMD55 APP_CMD with argument as card's RCA */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }
    /*Set data receive register*/
    SDIO_DataInitStructure.SDIO_DataTimeOut   = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength    = 8; //8byte,64λ
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b;
    SDIO_DataInitStructure.SDIO_TransferDir   = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode  = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM          = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /*!< Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r1
    SDIO_CmdInitStructure.SDIO_Wait     = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM     = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);

    if (errorstatus != SD_OK)
    {
        return (errorstatus);
    }

    /*Waiting to receive data */
    /*Not in these cases*/
    /*overflow error	  //Data crc failed		//data timeout	  //Received data block, crc detected successfully	//No start signal detected on all data lines*/
    while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
    {
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) //Is the received data available?
        {
            *(tempscr + index) = SDIO_ReadData();
            index++;
            if (index > 1)
                break;
        }
    }

    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        errorstatus = SD_DATA_TIMEOUT;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        errorstatus = SD_DATA_CRC_FAIL;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        errorstatus = SD_RX_OVERRUN;
        return (errorstatus);
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        errorstatus = SD_START_BIT_ERR;
        return (errorstatus);
    }

    /*!< Clear all the static flags */
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    *(pscr + 1) = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);

    *(pscr) = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);

    return (errorstatus);
}


/**
  * @brief  Converts the number of bytes in power of two and returns the power.
  * @param  NumberOfBytes: number of bytes.
  * @retval None
  */
uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes)
{
    uint8_t count = 0;

    while (NumberOfBytes != 1)
    {
        NumberOfBytes >>= 1;
        count++;
    }
    return (count);
}

/******************* (C) COPYRIGHT 2024 WIZnet *****END OF FILE****/
