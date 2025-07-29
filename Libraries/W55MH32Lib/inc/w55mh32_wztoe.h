#ifndef _WZTOE_H_
#define _WZTOE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "wizchip_conf.h"



#define WIZCHIP_CREG_BLOCK     0x00                   //< Common register block
#define WIZCHIP_SREG_BLOCK(N)  (1 + 4 * N)            //< Socket N register block
#define WIZCHIP_TXBUF_BLOCK(N) (2 + 4 * N)            //< Socket N Tx buffer address block
#define WIZCHIP_RXBUF_BLOCK(N) (3 + 4 * N)            //< Socket N Rx buffer address block

#define WIZCHIP_OFFSET_INC(ADDR, N) (ADDR + (N << 8)) //< Increase offset address


///////////////////////////////////////
// Definition For Legacy Chip Driver //
///////////////////////////////////////
#define IINCHIP_READ(ADDR)                WIZCHIP_READ(ADDR)               ///< The defined for legacy chip driver
#define IINCHIP_WRITE(ADDR, VAL)          WIZCHIP_WRITE(ADDR, VAL)         ///< The defined for legacy chip driver
#define IINCHIP_READ_BUF(ADDR, BUF, LEN)  WIZCHIP_READ_BUF(ADDR, BUF, LEN) ///< The defined for legacy chip driver
#define IINCHIP_WRITE_BUF(ADDR, BUF, LEN) WIZCHIP_WRITE(ADDR, BUF, LEN)    ///< The defined for legacy chip driver


#define WZTOE_MR       ((0x0000 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_GAR      ((0x0001 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_SUBR     ((0x0005 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_SHAR     ((0x0009 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_SIPR     ((0x000F << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_INTLEVEL ((0x0013 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_IR       ((0x0015 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_IMR_     ((0x0016 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_SIR      ((0x0017 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_SIMR     ((0x0018 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_RTR_     ((0x0019 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_RCR_     ((0x001B << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_PTIMER   ((0x001C << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_PMAGIC   ((0x001D << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_PHAR     ((0x001E << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_PSID     ((0x0024 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_PMRU     ((0x0026 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_UIPR     ((0x0028 << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_UPORTR   ((0x002C << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_PHYCFGR  ((0x002E << 8) + (WIZCHIP_CREG_BLOCK << 3))
#define WZTOE_VERSIONR ((0x0039 << 8) + (WIZCHIP_CREG_BLOCK << 3))


#define WZTOE_Sn_MR(N)         ((0x0000 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_CR(N)         ((0x0001 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_IR(N)         ((0x0002 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_SR(N)         ((0x0003 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_PORT(N)       ((0x0004 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_DHAR(N)       ((0x0006 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_DIPR(N)       ((0x000C << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_DPORT(N)      ((0x0010 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_MSSR(N)       ((0x0012 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_PROTO(N) 		 ((0x0014 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_TOS(N)        ((0x0015 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_TTL(N)        ((0x0016 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_RXBUF_SIZE(N) ((0x001E << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_TXBUF_SIZE(N) ((0x001F << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_TX_FSR(N)     ((0x0020 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_TX_RD(N)      ((0x0022 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_TX_WR(N)      ((0x0024 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_RX_RSR(N)     ((0x0026 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_RX_RD(N)      ((0x0028 << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_RX_WR(N)      ((0x002A << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_IMR(N)        ((0x002C << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_FRAG(N)       ((0x002D << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))
#define WZTOE_Sn_KPALVTR(N)    ((0x002F << 8) + (WIZCHIP_SREG_BLOCK(N) << 3))

//----------------------------- Register values  -----------------------------

/* MODE register values */
#define MR_RST   0x80
#define MR_WOL   0x20
#define MR_PB    0x10
#define MR_PPPOE 0x08
#define MR_FARP  0x02

/* WZTOE_IR register values */
#define IR_CONFLICT 0x80
#define IR_UNREACH  0x40
#define IR_PPPoE    0x20
#define IR_MP       0x10


/* WZTOE_PHYCFGR register value */
#define PHYCFGR_RST         ~(1 << 7) //< For PHY reset, must operate AND mask.
#define PHYCFGR_OPMD        (1 << 6)  // Configre PHY with OPMDC value
#define PHYCFGR_OPMDC_ALLA  (7 << 3)
#define PHYCFGR_OPMDC_PDOWN (6 << 3)
#define PHYCFGR_OPMDC_NA    (5 << 3)
#define PHYCFGR_OPMDC_100FA (4 << 3)
#define PHYCFGR_OPMDC_100F  (3 << 3)
#define PHYCFGR_OPMDC_100H  (2 << 3)
#define PHYCFGR_OPMDC_10F   (1 << 3)
#define PHYCFGR_OPMDC_10H   (0 << 3)
#define PHYCFGR_DPX_FULL    (1 << 2)
#define PHYCFGR_DPX_HALF    (0 << 2)
#define PHYCFGR_SPD_100     (1 << 1)
#define PHYCFGR_SPD_10      (0 << 1)
#define PHYCFGR_LNK_ON      (1 << 0)
#define PHYCFGR_LNK_OFF     (0 << 0)

/* IMR register values */
#define IM_IR7 0x80
#define IM_IR6 0x40
#define IM_IR5 0x20
#define IM_IR4 0x10

/* WZTOE_Sn_MR Default values */
#define Sn_MR_MULTI  0x80
#define Sn_MR_BCASTB 0x40
#define Sn_MR_ND     0x20
#define Sn_MR_UCASTB 0x10
#define Sn_MR_MACRAW 0x04
#define Sn_MR_IPRAW  0x03 /**< IP LAYER RAW SOCK */
#define Sn_MR_UDP    0x02
#define Sn_MR_TCP    0x01
#define Sn_MR_CLOSE  0x00

/* WZTOE_Sn_MR values used with Sn_MR_MACRAW */
#define Sn_MR_MFEN  Sn_MR_MULTI
#define Sn_MR_MMB   Sn_MR_ND
#define Sn_MR_MIP6B Sn_MR_UCASTB

/* WZTOE_Sn_MR value used with Sn_MR_UDP & Sn_MR_MULTI */
#define Sn_MR_MC    Sn_MR_ND
#define SOCK_STREAM Sn_MR_TCP
#define SOCK_DGRAM  Sn_MR_UDP


/* WZTOE_Sn_CR values */
#define Sn_CR_OPEN      0x01
#define Sn_CR_LISTEN    0x02
#define Sn_CR_CONNECT   0x04
#define Sn_CR_DISCON    0x08
#define Sn_CR_CLOSE     0x10
#define Sn_CR_SEND      0x20
#define Sn_CR_SEND_MAC  0x21
#define Sn_CR_SEND_KEEP 0x22
#define Sn_CR_RECV      0x40
#define Sn_IR_SENDOK    0x10
#define Sn_IR_TIMEOUT   0x08
#define Sn_IR_RECV      0x04
#define Sn_IR_DISCON    0x02
#define Sn_IR_CON       0x01

/* WZTOE_Sn_SR values */
#define SOCK_CLOSED      0x00
#define SOCK_INIT        0x13
#define SOCK_LISTEN      0x14
#define SOCK_SYNSENT     0x15
#define SOCK_SYNRECV     0x16
#define SOCK_ESTABLISHED 0x17
#define SOCK_FIN_WAIT    0x18
#define SOCK_CLOSING     0x1A
#define SOCK_TIME_WAIT   0x1B
#define SOCK_CLOSE_WAIT  0x1C
#define SOCK_LAST_ACK    0x1D
#define SOCK_UDP         0x22
#define SOCK_IPRAW       0x32 /**< IP raw mode socket */
#define SOCK_MACRAW      0x42

/* IP PROTOCOL */
#define IPPROTO_IP   0   //< Dummy for IP
#define IPPROTO_ICMP 1   //< Control message protocol
#define IPPROTO_IGMP 2   //< Internet group management protocol
#define IPPROTO_GGP  3   //< Gateway^2 (deprecated)
#define IPPROTO_TCP  6   //< TCP
#define IPPROTO_PUP  12  //< PUP
#define IPPROTO_UDP  17  //< UDP
#define IPPROTO_IDP  22  //< XNS idp
#define IPPROTO_ND   77  //< UNOFFICIAL net disk protocol
#define IPPROTO_RAW  255 //< Raw IP packet



////////////////////////
// Basic I/O Function //
////////////////////////

/**
 * @ingroup Basic_IO_function
 * @brief It reads 1 byte value from a register.
 * @param AddrSel Register address
 * @return The value of register
 */
uint8_t WIZCHIP_READ(uint32_t AddrSel);

/**
 * @ingroup Basic_IO_function
 * @brief It writes 1 byte value to a register.
 * @param AddrSel Register address
 * @param wb Write data
 * @return void
 */
void WIZCHIP_WRITE(uint32_t AddrSel, uint8_t wb);

/**
 * @ingroup Basic_IO_function
 * @brief It reads sequence data from registers.
 * @param AddrSel Register address
 * @param pBuf Pointer buffer to read data
 * @param len Data length
 */
void WIZCHIP_READ_BUF(uint32_t AddrSel, uint8_t *pBuf, uint16_t len);

/**
 * @ingroup Basic_IO_function
 * @brief It writes sequence data to registers.
 * @param AddrSel Register address
 * @param pBuf Pointer buffer to write data
 * @param len Data length
 */
void WIZCHIP_WRITE_BUF(uint32_t AddrSel, uint8_t *pBuf, uint16_t len);

/////////////////////////////////
// Common Register I/O function //
/////////////////////////////////
/**
 * @ingroup Common_register_access_function
 * @brief Set Mode Register
 * @param (uint8_t)mr The value to be set.
 * @sa getMR()
 */
#define setMR(mr) \
    WIZCHIP_WRITE(WZTOE_MR, mr)


/**
 * @ingroup Common_register_access_function
 * @brief Get Mode Register
 * @return uint8_t. The value of Mode register.
 * @sa setMR()
 */
#define getMR() \
    WIZCHIP_READ(WZTOE_MR)

/**
 * @ingroup Common_register_access_function
 * @brief Set gateway IP address
 * @param (uint8_t*)gar Pointer variable to set gateway IP address. It should be allocated 4 bytes.
 * @sa getGAR()
 */
#define setGAR(gar) \
    WIZCHIP_WRITE_BUF(WZTOE_GAR, gar, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Get gateway IP address
 * @param (uint8_t*)gar Pointer variable to get gateway IP address. It should be allocated 4 bytes.
 * @sa setGAR()
 */
#define getGAR(gar) \
    WIZCHIP_READ_BUF(WZTOE_GAR, gar, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Set subnet mask address
 * @param (uint8_t*)subr Pointer variable to set subnet mask address. It should be allocated 4 bytes.
 * @sa getSUBR()
 */
#define setSUBR(subr) \
    WIZCHIP_WRITE_BUF(WZTOE_SUBR, subr, 4)


/**
 * @ingroup Common_register_access_function
 * @brief Get subnet mask address
 * @param (uint8_t*)subr Pointer variable to get subnet mask address. It should be allocated 4 bytes.
 * @sa setSUBR()
 */
#define getSUBR(subr) \
    WIZCHIP_READ_BUF(WZTOE_SUBR, subr, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Set local MAC address
 * @param (uint8_t*)shar Pointer variable to set local MAC address. It should be allocated 6 bytes.
 * @sa getSHAR()
 */
#define setSHAR(shar) \
    WIZCHIP_WRITE_BUF(WZTOE_SHAR, shar, 6)

/**
 * @ingroup Common_register_access_function
 * @brief Get local MAC address
 * @param (uint8_t*)shar Pointer variable to get local MAC address. It should be allocated 6 bytes.
 * @sa setSHAR()
 */
#define getSHAR(shar) \
    WIZCHIP_READ_BUF(WZTOE_SHAR, shar, 6)

/**
 * @ingroup Common_register_access_function
 * @brief Set local IP address
 * @param (uint8_t*)sipr Pointer variable to set local IP address. It should be allocated 4 bytes.
 * @sa getSIPR()
 */
#define setSIPR(sipr) \
    WIZCHIP_WRITE_BUF(WZTOE_SIPR, sipr, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Get local IP address
 * @param (uint8_t*)sipr Pointer variable to get local IP address. It should be allocated 4 bytes.
 * @sa setSIPR()
 */
#define getSIPR(sipr) \
    WIZCHIP_READ_BUF(WZTOE_SIPR, sipr, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Set WZTOE_INTLEVEL register
 * @param (uint16_t)intlevel Value to set @ref WZTOE_INTLEVEL register.
 * @sa getINTLEVEL()
 */
#define setINTLEVEL(intlevel)                                                    \
    {                                                                            \
        WIZCHIP_WRITE(WZTOE_INTLEVEL, (uint8_t)(intlevel >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_INTLEVEL, 1), (uint8_t)intlevel); \
    }


/**
 * @ingroup Common_register_access_function
 * @brief Get WZTOE_INTLEVEL register
 * @return uint16_t. Value of @ref WZTOE_INTLEVEL register.
 * @sa setINTLEVEL()
 */
#define getINTLEVEL() \
    (((uint16_t)WIZCHIP_READ(WZTOE_INTLEVEL) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_INTLEVEL, 1)))

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_IR register
 * @param (uint8_t)ir Value to set @ref WZTOE_IR register.
 * @sa getIR()
 */
#define setIR(ir) \
    WIZCHIP_WRITE(WZTOE_IR, (ir & 0xF0))

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_IR register
 * @return uint8_t. Value of @ref WZTOE_IR register.
 * @sa setIR()
 */
#define getIR() \
    (WIZCHIP_READ(WZTOE_IR) & 0xF0)
/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_IMR_ register
 * @param (uint8_t)imr Value to set @ref WZTOE_IMR_ register.
 * @sa getIMR()
 */
#define setIMR(imr) \
    WIZCHIP_WRITE(WZTOE_IMR_, imr)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_IMR_ register
 * @return uint8_t. Value of @ref WZTOE_IMR_ register.
 * @sa setIMR()
 */
#define getIMR() \
    WIZCHIP_READ(WZTOE_IMR_)

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_SIR register
 * @param (uint8_t)sir Value to set @ref WZTOE_SIR register.
 * @sa getSIR()
 */
#define setSIR(sir) \
    WIZCHIP_WRITE(WZTOE_SIR, sir)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_SIR register
 * @return uint8_t. Value of @ref WZTOE_SIR register.
 * @sa setSIR()
 */
#define getSIR() \
    WIZCHIP_READ(WZTOE_SIR)
/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_SIMR register
 * @param (uint8_t)simr Value to set @ref WZTOE_SIMR register.
 * @sa getSIMR()
 */
#define setSIMR(simr) \
    WIZCHIP_WRITE(WZTOE_SIMR, simr)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_SIMR register
 * @return uint8_t. Value of @ref WZTOE_SIMR register.
 * @sa setSIMR()
 */
#define getSIMR() \
    WIZCHIP_READ(WZTOE_SIMR)

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_RTR_ register
 * @param (uint16_t)rtr Value to set @ref WZTOE_RTR_ register.
 * @sa getRTR()
 */
#define setRTR(rtr)                                                     \
    {                                                                   \
        WIZCHIP_WRITE(WZTOE_RTR_, (uint8_t)(rtr >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_RTR_, 1), (uint8_t)rtr); \
    }

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_RTR_ register
 * @return uint16_t. Value of @ref WZTOE_RTR_ register.
 * @sa setRTR()
 */
#define getRTR() \
    (((uint16_t)WIZCHIP_READ(WZTOE_RTR_) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_RTR_, 1)))


/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_RCR_ register
 * @param (uint8_t)rcr Value to set @ref WZTOE_RCR_ register.
 * @sa getRCR()
 */
#define setRCR(rcr) \
    WIZCHIP_WRITE(WZTOE_RCR_, rcr)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_RCR_ register
 * @return uint8_t. Value of @ref WZTOE_RCR_ register.
 * @sa setRCR()
 */
#define getRCR() \
    WIZCHIP_READ(WZTOE_RCR_)

//================================================== test done ===========================================================

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_PTIMER register
 * @param (uint8_t)ptimer Value to set @ref WZTOE_PTIMER register.
 * @sa getPTIMER()
 */
#define setPTIMER(ptimer) \
    WIZCHIP_WRITE(WZTOE_PTIMER, ptimer)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_PTIMER register
 * @return uint8_t. Value of @ref WZTOE_PTIMER register.
 * @sa setPTIMER()
 */
#define getPTIMER() \
    WIZCHIP_READ(WZTOE_PTIMER)

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_PMAGIC register
 * @param (uint8_t)pmagic Value to set @ref WZTOE_PMAGIC register.
 * @sa getPMAGIC()
 */
#define setPMAGIC(pmagic) \
    WIZCHIP_WRITE(WZTOE_PMAGIC, pmagic)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_PMAGIC register
 * @return uint8_t. Value of @ref WZTOE_PMAGIC register.
 * @sa setPMAGIC()
 */
#define getPMAGIC() \
    WIZCHIP_READ(WZTOE_PMAGIC)

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_PHAR address
 * @param (uint8_t*)phar Pointer variable to set PPP destination MAC register address. It should be allocated 6 bytes.
 * @sa getPHAR()
 */
#define setPHAR(phar) \
    WIZCHIP_WRITE_BUF(WZTOE_PHAR, phar, 6)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_PHAR address
 * @param (uint8_t*)phar Pointer variable to PPP destination MAC register address. It should be allocated 6 bytes.
 * @sa setPHAR()
 */
#define getPHAR(phar) \
    WIZCHIP_READ_BUF(WZTOE_PHAR, phar, 6)

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_PSID register
 * @param (uint16_t)psid Value to set @ref WZTOE_PSID register.
 * @sa getPSID()
 */
#define setPSID(psid)                                                    \
    {                                                                    \
        WIZCHIP_WRITE(WZTOE_PSID, (uint8_t)(psid >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_PSID, 1), (uint8_t)psid); \
    }

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_PSID register
 * @return uint16_t. Value of @ref WZTOE_PSID register.
 * @sa setPSID()
 */
#define getPSID() \
    (((uint16_t)WIZCHIP_READ(WZTOE_PSID) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_PSID, 1)))

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_PMRU register
 * @param (uint16_t)pmru Value to set @ref WZTOE_PMRU register.
 * @sa getPMRU()
 */
#define setPMRU(pmru)                                                    \
    {                                                                    \
        WIZCHIP_WRITE(WZTOE_PMRU, (uint8_t)(pmru >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_PMRU, 1), (uint8_t)pmru); \
    }

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_PMRU register
 * @return uint16_t. Value of @ref WZTOE_PMRU register.
 * @sa setPMRU()
 */
#define getPMRU() \
    (((uint16_t)WIZCHIP_READ(WZTOE_PMRU) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_PMRU, 1)))

/**
 * @ingroup Common_register_access_function
 * @brief Get unreachable IP address
 * @param (uint8_t*)uipr Pointer variable to get unreachable IP address. It should be allocated 4 bytes.
 */
#define getUIPR(uipr) \
    WIZCHIP_READ_BUF(WZTOE_UIPR, uipr, 4)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_UPORTR register
 * @return uint16_t. Value of @ref WZTOE_UPORTR register.
 */
#define getUPORTR() \
    (((uint16_t)WIZCHIP_READ(WZTOE_UPORTR) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_UPORTR, 1)))

/**
 * @ingroup Common_register_access_function
 * @brief Set @ref WZTOE_PHYCFGR register
 * @param (uint8_t)phycfgr Value to set @ref WZTOE_PHYCFGR register.
 * @sa getPHYCFGR()
 */
#define setPHYCFGR(phycfgr) \
    WIZCHIP_WRITE(WZTOE_PHYCFGR, phycfgr)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_PHYCFGR register
 * @return uint8_t. Value of @ref WZTOE_PHYCFGR register.
 * @sa setPHYCFGR()
 */
#define getPHYCFGR() \
    WIZCHIP_READ(WZTOE_PHYCFGR)

/**
 * @ingroup Common_register_access_function
 * @brief Get @ref WZTOE_VERSIONR register
 * @return uint8_t. Value of @ref WZTOE_VERSIONR register.
 */
#define getVERSIONR() \
    WIZCHIP_READ(WZTOE_VERSIONR)

/////////////////////////////////////

///////////////////////////////////
// Socket N register I/O function //
///////////////////////////////////
/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_MR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)mr Value to set @ref WZTOE_Sn_MR
 * @sa getSn_MR()
 */
#define setSn_MR(sn, mr) \
    WIZCHIP_WRITE(WZTOE_Sn_MR(sn), mr)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_MR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_MR.
 * @sa setSn_MR()
 */
#define getSn_MR(sn) \
    WIZCHIP_READ(WZTOE_Sn_MR(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)cr Value to set @ref WZTOE_Sn_CR
 * @sa getSn_CR()
 */
#define setSn_CR(sn, cr) \
    WIZCHIP_WRITE(WZTOE_Sn_CR(sn), cr)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_CR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_CR.
 * @sa setSn_CR()
 */
#define getSn_CR(sn) \
    WIZCHIP_READ(WZTOE_Sn_CR(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)ir Value to set @ref WZTOE_Sn_IR
 * @sa getSn_IR()
 */
#define setSn_IR(sn, ir) \
    WIZCHIP_WRITE(WZTOE_Sn_IR(sn), (ir & 0x1F))

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_IR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_IR.
 * @sa setSn_IR()
 */
#define getSn_IR(sn) \
    (WIZCHIP_READ(WZTOE_Sn_IR(sn)) & 0x1F)

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_IMR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)imr Value to set @ref WZTOE_Sn_IMR
 * @sa getSn_IMR()
 */
#define setSn_IMR(sn, imr) \
    WIZCHIP_WRITE(WZTOE_Sn_IMR(sn), (imr & 0x1F))

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_IMR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_IMR.
 * @sa setSn_IMR()
 */
#define getSn_IMR(sn) \
    (WIZCHIP_READ(WZTOE_Sn_IMR(sn)) & 0x1F)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_SR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_SR.
 */
#define getSn_SR(sn) \
    WIZCHIP_READ(WZTOE_Sn_SR(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_PORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)port Value to set @ref WZTOE_Sn_PORT.
 * @sa getSn_PORT()
 */
#define setSn_PORT(sn, port)                                                    \
    {                                                                           \
        WIZCHIP_WRITE(WZTOE_Sn_PORT(sn), (uint8_t)(port >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_Sn_PORT(sn), 1), (uint8_t)port); \
    }

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_PORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_PORT.
 * @sa setSn_PORT()
 */
#define getSn_PORT(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_PORT(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_PORT(sn), 1)))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_DHAR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dhar Pointer variable to set socket n destination hardware address. It should be allocated 6 bytes.
 * @sa getSn_DHAR()
 */
#define setSn_DHAR(sn, dhar) \
    WIZCHIP_WRITE_BUF(WZTOE_Sn_DHAR(sn), dhar, 6)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_MR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dhar Pointer variable to get socket n destination hardware address. It should be allocated 6 bytes.
 * @sa setSn_DHAR()
 */
#define getSn_DHAR(sn, dhar) \
    WIZCHIP_READ_BUF(WZTOE_Sn_DHAR(sn), dhar, 6)

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dipr Pointer variable to set socket n destination IP address. It should be allocated 4 bytes.
 * @sa getSn_DIPR()
 */
#define setSn_DIPR(sn, dipr) \
    WIZCHIP_WRITE_BUF(WZTOE_Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_DIPR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t*)dipr Pointer variable to get socket n destination IP address. It should be allocated 4 bytes.
 * @sa setSn_DIPR()
 */
#define getSn_DIPR(sn, dipr) \
    WIZCHIP_READ_BUF(WZTOE_Sn_DIPR(sn), dipr, 4)

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)dport Value to set @ref WZTOE_Sn_DPORT
 * @sa getSn_DPORT()
 */
#define setSn_DPORT(sn, dport)                                                    \
    {                                                                             \
        WIZCHIP_WRITE(WZTOE_Sn_DPORT(sn), (uint8_t)(dport >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_Sn_DPORT(sn), 1), (uint8_t)dport); \
    }

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_DPORT register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_DPORT.
 * @sa setSn_DPORT()
 */
#define getSn_DPORT(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_DPORT(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_DPORT(sn), 1)))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)mss Value to set @ref WZTOE_Sn_MSSR
 * @sa setSn_MSSR()
 */
#define setSn_MSSR(sn, mss)                                                    \
    {                                                                          \
        WIZCHIP_WRITE(WZTOE_Sn_MSSR(sn), (uint8_t)(mss >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_Sn_MSSR(sn), 1), (uint8_t)mss); \
    }

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_MSSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_MSSR.
 * @sa setSn_MSSR()
 */
#define getSn_MSSR(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_MSSR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_MSSR(sn), 1)))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_TOS register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)tos Value to set @ref WZTOE_Sn_TOS
 * @sa getSn_TOS()
 */
#define setSn_TOS(sn, tos) \
    WIZCHIP_WRITE(WZTOE_Sn_TOS(sn), tos)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_TOS register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of WZTOE_Sn_TOS.
 * @sa setSn_TOS()
 */
#define getSn_TOS(sn) \
    WIZCHIP_READ(WZTOE_Sn_TOS(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_TTL register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)ttl Value to set @ref WZTOE_Sn_TTL
 * @sa getSn_TTL()
 */
#define setSn_TTL(sn, ttl) \
    WIZCHIP_WRITE(WZTOE_Sn_TTL(sn), ttl)


/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_TTL register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_TTL.
 * @sa setSn_TTL()
 */
#define getSn_TTL(sn) \
    WIZCHIP_READ(WZTOE_Sn_TTL(sn))


/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_RXBUF_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)rxbufsize Value to set @ref WZTOE_Sn_RXBUF_SIZE
 * @sa getSn_RXBUF_SIZE()
 */
#define setSn_RXBUF_SIZE(sn, rxbufsize) \
    WIZCHIP_WRITE(WZTOE_Sn_RXBUF_SIZE(sn), rxbufsize)


/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_RXBUF_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_RXBUF_SIZE.
 * @sa setSn_RXBUF_SIZE()
 */
#define getSn_RXBUF_SIZE(sn) \
    WIZCHIP_READ(WZTOE_Sn_RXBUF_SIZE(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_TXBUF_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)txbufsize Value to set @ref WZTOE_Sn_TXBUF_SIZE
 * @sa getSn_TXBUF_SIZE()
 */
#define setSn_TXBUF_SIZE(sn, txbufsize) \
    WIZCHIP_WRITE(WZTOE_Sn_TXBUF_SIZE(sn), txbufsize)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_TXBUF_SIZE register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_TXBUF_SIZE.
 * @sa setSn_TXBUF_SIZE()
 */
#define getSn_TXBUF_SIZE(sn) \
    WIZCHIP_READ(WZTOE_Sn_TXBUF_SIZE(sn))

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_TX_FSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_TX_FSR.
 */
uint16_t getSn_TX_FSR(uint8_t sn);

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_TX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_TX_RD.
 */
#define getSn_TX_RD(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_TX_RD(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_TX_RD(sn), 1)))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_TX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)txwr Value to set @ref WZTOE_Sn_TX_WR
 * @sa GetSn_TX_WR()
 */
#define setSn_TX_WR(sn, txwr)                                                    \
    {                                                                            \
        WIZCHIP_WRITE(WZTOE_Sn_TX_WR(sn), (uint8_t)(txwr >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_Sn_TX_WR(sn), 1), (uint8_t)txwr); \
    }

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_TX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_TX_WR.
 * @sa setSn_TX_WR()
 */
#define getSn_TX_WR(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_TX_WR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_TX_WR(sn), 1)))


/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_RX_RSR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_RX_RSR.
 */
uint16_t getSn_RX_RSR(uint8_t sn);


/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_RX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)rxrd Value to set @ref WZTOE_Sn_RX_RD
 * @sa getSn_RX_RD()
 */
#define setSn_RX_RD(sn, rxrd)                                                    \
    {                                                                            \
        WIZCHIP_WRITE(WZTOE_Sn_RX_RD(sn), (uint8_t)(rxrd >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_Sn_RX_RD(sn), 1), (uint8_t)rxrd); \
    }

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_RX_RD register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_RX_RD.
 * @sa setSn_RX_RD()
 */
#define getSn_RX_RD(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_RX_RD(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_RX_RD(sn), 1)))

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_RX_WR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_RX_WR.
 */
#define getSn_RX_WR(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_RX_WR(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_RX_WR(sn), 1)))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_FRAG register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint16_t)frag Value to set @ref WZTOE_Sn_FRAG
 * @sa getSn_FRAD()
 */
#define setSn_FRAG(sn, frag)                                                    \
    {                                                                           \
        WIZCHIP_WRITE(WZTOE_Sn_FRAG(sn), (uint8_t)(frag >> 8));                 \
        WIZCHIP_WRITE(WIZCHIP_OFFSET_INC(WZTOE_Sn_FRAG(sn), 1), (uint8_t)frag); \
    }

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_FRAG register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of @ref WZTOE_Sn_FRAG.
 * @sa setSn_FRAG()
 */
#define getSn_FRAG(sn) \
    (((uint16_t)WIZCHIP_READ(WZTOE_Sn_FRAG(sn)) << 8) + WIZCHIP_READ(WIZCHIP_OFFSET_INC(WZTOE_Sn_FRAG(sn), 1)))

/**
 * @ingroup Socket_register_access_function
 * @brief Set @ref WZTOE_Sn_KPALVTR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param (uint8_t)kpalvt Value to set @ref WZTOE_Sn_KPALVTR
 * @sa getSn_KPALVTR()
 */
#define setSn_KPALVTR(sn, kpalvt) \
    WIZCHIP_WRITE(WZTOE_Sn_KPALVTR(sn), kpalvt)

/**
 * @ingroup Socket_register_access_function
 * @brief Get @ref WZTOE_Sn_KPALVTR register
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint8_t. Value of @ref WZTOE_Sn_KPALVTR.
 * @sa setSn_KPALVTR()
 */
#define getSn_KPALVTR(sn) \
    WIZCHIP_READ(WZTOE_Sn_KPALVTR(sn))

//////////////////////////////////////

/////////////////////////////////////
// Sn_TXBUF & Sn_RXBUF IO function //
/////////////////////////////////////
/**  
 * @brief Socket_register_access_function
 * @brief Gets the max buffer size of socket sn passed as parameter.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of Socket n RX max buffer size.
 */
#define getSn_RxMAX(sn) \
    (((uint16_t)getSn_RXBUF_SIZE(sn)) << 10)

/**  
 * @brief Socket_register_access_function
 * @brief Gets the max buffer size of socket sn passed as parameters.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @return uint16_t. Value of Socket n TX max buffer size.
 */
#define getSn_TxMAX(sn) \
    (((uint16_t)getSn_TXBUF_SIZE(sn)) << 10)

/**
 * @ingroup Basic_IO_function
 * @brief It copies data to internal TX memory
 *
 * @details This function reads the Tx write pointer register and after that,
 * it copies the <i>wizdata(pointer buffer)</i> of the length of <i>len(variable)</i> bytes to internal TX memory
 * and updates the Tx write pointer register.
 * This function is being called by send() and sendto() function also.
 *
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param wizdata Pointer buffer to write data
 * @param len Data length
 * @sa wiz_recv_data()
 */
void wiz_send_data(uint8_t sn, uint8_t *wizdata, uint16_t len);

/**
 * @ingroup Basic_IO_function
 * @brief It copies data to your buffer from internal RX memory
 *
 * @details This function read the Rx read pointer register and after that,
 * it copies the received data from internal RX memory
 * to <i>wizdata(pointer variable)</i> of the length of <i>len(variable)</i> bytes.
 * This function is being called by recv() also.
 *
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param wizdata Pointer buffer to read data
 * @param len Data length
 * @sa wiz_send_data()
 */
void wiz_recv_data(uint8_t sn, uint8_t *wizdata, uint16_t len);

/**
 * @ingroup Basic_IO_function
 * @brief It discard the received data in RX memory.
 * @details It discards the data of the length of <i>len(variable)</i> bytes in internal RX memory.
 * @param (uint8_t)sn Socket number. It should be <b>0 ~ 7</b>.
 * @param len Data length
 */
void wiz_recv_ignore(uint8_t sn, uint16_t len);

/**  
 * @brief Initialize TOE
 * @brief Initialize TOE to use Ethernet functions
 */
int wiz_toe_init(void);

#ifdef __cplusplus
}
#endif

#endif
