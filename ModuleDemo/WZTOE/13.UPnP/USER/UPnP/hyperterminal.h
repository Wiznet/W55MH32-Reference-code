#ifndef _HYPERTERMINAL_H
#define _HYPERTERMINAL_H
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Exported types ------------------------------------------------------------*/
// ypedef  void (*pFunction)(void);
/* Exported constants --------------------------------------------------------*/
/* Constants used by Serial Command Line Mode */
#define CMD_STRING_SIZE 128
/* Exported macro ------------------------------------------------------------*/
#define IS_AF(c)            ((c >= 'A') && (c <= 'F'))
#define IS_af(c)            ((c >= 'a') && (c <= 'f'))
#define IS_09(c)            ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)       IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       (c - '0')
#define CONVERTHEX_alpha(c) (IS_AF(c) ? (c - 'A' + 10) : (c - 'a' + 10))
#define CONVERTHEX(c)       (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))


#define LED_ON  0
#define LED_OFF 1

#define SET   1
#define RESET 0

typedef void (*onboard_led_f)(uint8_t status);
void UserLED_Control_Init(onboard_led_f fun_cb);

/* Exported functions ------------------------------------------------------- */
/**
 * @brief   Convert an Integer to a string
 * @param   str: The string
 * @param   intnum: The intger to be converted
 * @return  none
 */
void Int2Str(char *str, uint32_t intnum);

/**
 * @brief   Convert a string to an integer
 * @param   inputstr: The string to be converted
 * @param   intnum: The intger value
 * @return  1: Correct;0: Error
 */
uint8_t Str2Int(char *inputstr, uint32_t *intnum);

/**
 * @brief   Display/Manage a Menu on HyperTerminal Window
 * @param   sn: use for SSDP; sn2: use for run tcp/udp loopback; sn3: use for listenes IGD event message
 * @param   buf: use for tcp/udp loopback rx/tx buff; tcps_port: use for tcp loopback listen; udps_port: use for udp loopback receive
 * @return  none
 */
void Main_Menu(uint8_t sn, uint8_t sn2, uint8_t sn3, uint8_t *buf, uint16_t tcps_port, uint16_t udps_port);


#endif /* _HYPERTERMINAL_H */

/*******************(C)COPYRIGHT 2024 WIZnet *****END OF FILE****/
