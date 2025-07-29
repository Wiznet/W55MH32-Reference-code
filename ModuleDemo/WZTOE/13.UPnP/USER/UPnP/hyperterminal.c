
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "hyperterminal.h"
#include "UPnP.h"
#include "MakeXML.h"
#include "loopback.h"
#include "wiz_interface.h"
#include "bsp_uart.h"

onboard_led_f UserLED_ctl_cb = NULL;

uint8_t gMsgBuf[20];

/**
 * @brief   LED Control init func
 * @param   fun_cb function pointer
 * @return  none
 */
void UserLED_Control_Init(onboard_led_f fun_cb)
{
    if (fun_cb != NULL)
    {
        UserLED_ctl_cb = fun_cb;
    }
    else
    {
        printf("No correlation function is registered!\r\n");
    }
}


/**
 * @brief   Convert an Integer to a string
 * @param   str: The string
 * @param   intnum: The intger to be converted
 * @return  none
 */
void Int2Str(char *str, uint32_t intnum)
{
    int i, Div = 1000000000, j = 0, Status = 0;
    for (i = 0; i < 10; i++)
    {
        str[j++]  = (intnum / Div) + 48;
        intnum    = intnum % Div;
        Div      /= 10;
        if ((str[j - 1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/**
 * @brief   Convert a string to an integer
 * @param   inputstr: The string to be converted
 * @param   intnum: The intger value
 * @return  1: Correct;0: Error
 */
uint8_t Str2Int(char *inputstr, uint32_t *intnum)
{
    uint8_t  i = 0, res = 0;
    uint32_t val = 0;
    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                res     = 1; /* return 1; */
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }
        if (i >= 11)
            res = 0; /* over 8 digit hex --invalid */
    }
    else             /* max 10-digit decimal input */
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val     = val << 10;
                *intnum = val;
                res     = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val     = val << 20;
                *intnum = val;
                res     = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
                val = val * 10 + CONVERTDEC(inputstr[i]);
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }
        if (i >= 11)
            res = 0; /* Over 10 digit decimal --invalid */
    }
    return res;
}

char *STRTOK(char *strToken, const char *strDelimit)
{
    static char *pCurrent;
    char        *pDelimit;

    if (strToken != NULL)
        pCurrent = strToken;
    else
        strToken = pCurrent;

    //       if ( *pCurrent == NULL ) return NULL;

    while (*pCurrent)
    {
        pDelimit = (char *)strDelimit;
        while (*pDelimit)
        {
            if (*pCurrent == *pDelimit)
            {
                //*pCurrent = NULL;
                *pCurrent = 0;
                ++pCurrent;
                return strToken;
            }
            ++pDelimit;
        }
        ++pCurrent;
    }

    return strToken;
}


/**
 * @brief   Display/Manage a Menu on HyperTerminal Window
 * @param   sn: use for SSDP; sn2: use for run tcp/udp loopback; sn3: use for listenes IGD event message
 * @param   buf: use for tcp/udp loopback rx/tx buff; tcps_port: use for tcp loopback listen; udps_port: use for udp loopback receive
 * @return  none
 */
void Main_Menu(uint8_t sn, uint8_t sn2, uint8_t sn3, uint8_t *buf, uint16_t tcps_port, uint16_t udps_port)
{
    static char           choice[3];
    static char           msg[256], ipaddr[12], protocol[4];
    static unsigned short ret, external_port, internal_port;
    static uint8_t        bTreat;
    static uint8_t        Sip[4];

    while (1)
    {
        /* Display Menu on HyperTerminal Window */
        bTreat = RESET;
        printf("\r\n====================== WIZnet Chip Control Point ===================\r\n");
        printf("This Application is basic example of UART interface with\r\n");
        printf("Windows Hyper Terminal. \r\n");
        printf("\r\n==========================================================\r\n");
        printf("                          APPLICATION MENU :\r\n");
        printf("\r\n==========================================================\r\n\n");
        printf(" 1 - Set LED on \r\n");
        printf(" 2 - Set LED off \r\n");
        printf(" 3 - Show network setting\r\n");
        printf(" 4 - Set  network setting\r\n");
        printf(" 5 - Run TCP Loopback\r\n");
        printf(" 6 - Run UDP Loopback\r\n");
        printf(" 7 - UPnP PortForwarding: AddPort\r\n");
        printf(" 8 - UPnP PortForwarding: DeletePort\r\n");

        printf("Enter your choice : ");
        memset(choice, 0, sizeof(choice));
        scanf("%s", choice);
        printf("%c\r\n", choice[0]);
        /* Set LD1 on */
        if (choice[0] == '1')
        {
            bTreat = SET;
            UserLED_ctl_cb(LED_ON);
        }
        /* Set LD1 off */
        if (choice[0] == '2')
        {
            bTreat = SET;
            UserLED_ctl_cb(LED_OFF);
        }
        if (choice[0] == '3')
        {
            bTreat = SET;
            print_network_information();
        }

        if (choice[0] == '4')
        {
            bTreat = SET;
            wiz_NetInfo get_info;
            wizchip_getnetinfo(&get_info);
            // IP address
            printf("\r\nIP address : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            if (!VerifyIPAddress(msg, get_info.ip))
            {
                printf("\aInvalid.");
            }

            // Subnet mask
            printf("\r\nSubnet mask : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            if (!VerifyIPAddress(msg, get_info.sn))
            {
                printf("\aInvalid.");
            }

            // gateway address
            printf("\r\nGateway address : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            if (!VerifyIPAddress(msg, get_info.gw))
            {
                printf("\aInvalid.");
            }

            // DNS address
            printf("\r\nDNS address : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            if (!VerifyIPAddress(msg, get_info.dns))
            {
                printf("\aInvalid.");
            }

            get_info.dhcp = NETINFO_STATIC;

            wizchip_setnetinfo(&get_info);
        }

        if (choice[0] == '5')
        {
            bTreat = SET;

            printf("\r\nRun TCP loopback");
            printf("\r\nRun TCP loopback, port number [%d] is listened", tcps_port);
            printf("\r\nTo Exit, press [Q]\r\n");
            close(sn2);
            while (1)
            {
                if (get_char_non_blocking() == 'Q')
                {
                    printf("\r\n Stop ");
										close(sn2);
                    break;
                }
                loopback_tcps(sn2, buf, tcps_port);
            }
        }

        if (choice[0] == '6')
        {
            bTreat = SET;

            printf("\r\nRun UDP loopback");
            printf("\r\nRun UDP loopback, port number [%d] is listened", udps_port);
            printf("\r\nTo Exit, press [Q]\r\n");
            close(sn2);
            while (1)
            {
                if (get_char_non_blocking() == 'Q')
                {
                    printf("\r\n Stop ");
                    break;
                }
                loopback_udps(sn2, buf, udps_port);
            }
        }
        if (choice[0] == '7')
        {
            bTreat = SET;

            printf("\r\nType a Protocol(TCP/UDP) : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            strncpy(protocol, msg, 3);
            protocol[3] = '\0';

            printf("\r\nType a External Port Number : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            external_port = ATOI(msg, 10);

            printf("\r\nType a Internal Port Number : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            internal_port = ATOI(msg, 10);
					  if(strcmp(protocol,"tcp") || strcmp(protocol,"TCP"))
                tcps_port     = internal_port;
						else
						    udps_port     = internal_port;
            close(sn2);
            // Try to Add Port Action
            getSIPR(Sip);
            sprintf(ipaddr, "%d.%d.%d.%d", Sip[0], Sip[1], Sip[2], Sip[3]);
            if ((ret = AddPortProcess(sn, protocol, external_port, ipaddr, internal_port, "W5500_uPnPGetway")) == 0)
                printf("AddPort Success!!\r\n");
            else
                printf("AddPort Error Code is %d\r\n", ret);
        }

        if (choice[0] == '8')
        {
            bTreat = SET;

            printf("\r\nType a Protocol(TCP/UDP) : ");
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            //GetInputString(msg);
            strncpy(protocol, msg, 3);
            protocol[3] = '\0';

            printf("\r\nType a External Port Number : ");

            //			TCP_LISTEN_PORT=num;
            //			UDP_LISTEN_PORT=num;
            //			printf("%d\r\n",TCP_LISTEN_PORT);
            memset(msg, 0, sizeof(msg));
            scanf("%s", msg);
            printf("%s\r\n", msg);
            external_port = ATOI(msg, 10);

            // Try to Delete Port Action
            if ((ret = DeletePortProcess(sn, protocol, external_port)) == 0)
                printf("DeletePort Success!!\r\n");
            else
                printf("DeletePort Error Code is %d\r\n", ret);
        }

        /* OTHERS CHOICE*/
        if (bTreat == RESET)
        {
            printf(" wrong choice  \r\n");
        }

        eventing_listener(sn3);

    } /* While(1)*/
} /* Main_Menu */
