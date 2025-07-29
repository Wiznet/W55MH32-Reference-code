#include <stdio.h>
#include <string.h>
#include "socket.h"
#include "wizchip_conf.h"
#include "smtp.h"
#include <stdlib.h>

/* matters need attention:
    1.Please modify the email account and sender and recipient information
    2.The SMTP function must be enabled for the mailbox
    3.Please confirm the SMTP server address and port number used by your mailbox
 */

#define ETHERNET_MAX_BUF_SIZE (1024 * 2)
uint8_t Mail_Send_OK;
char    hello[50]     = "HELO localhost";                   // Identity command
char    hello_reply[] = "250 OK";                           // Id successfully responded
char    AUTH[50]      = "AUTH LOGIN";                       // Authentication request
char    AUTH_reply[]  = "334 dXNlcm5hbWU6";                 // The authentication request was successfully sent
char    name_126[100] = "wiznethk@126.com";                 // 126 Login email address
char    base64name_126[200];                                // 126 base64 encoding of the login mailbox name
char    name_reply[]     = "334 UGFzc3dvcmQ6";              // The login name was sent successfully
char    password_126[50] = "ZPURADLGRUPQLVBK";              // 126 Email login password
char    base64password_126[100];                            // base64 123 Password for logging in to the mailbox
char    password_reply[] = "235 Authentication successful"; // Login successful response
char    from[]           = "wiznethk@126.com";              // Sender email
char    from_reply[]     = "250 Mail OK";
char    to[]             = "2510582273@qq.com";             // Recipient email address
char    to_reply[]       = "250 Mail OK";
char    data_init[10]    = "data";                          // Request data transfer
char    data_reply[]     = "354";                           // The request was successfully responded to HEAD
char    Cc[]             = "";                              // Cc to email
char    subject[]        = "Hello!WIZnet!";                 // subject
char    content[]        = "Hello!WIZnet!";                 // text part
char    mime_reply[]     = "250 Mail OK queued as";         // The email was sent successfully
char    mailfrom[50]     = "MAIL FROM:<>";
char    rcptto[50]       = "rcpt to:<>";
char    mime[200]        = "From:\r\n";
char    mime1[50]        = "To:\r\n";
char    mime2[50]        = "Cc:\r\n";
char    mime3[50]        = "Subject:\r\n";
char    mime4[50]        = "MIME-Version:1.0\r\nContent-Type:text/plain\r\n\r\n";
char    mime5[50]        = "\r\n.\r\n";

uint8_t SMTP_STATE = waitfor220; // SMTP status

/**
 * @brief   send mail
 * @param   sn:	socket number
 * @param   buf:	ethernet buffer
 * @param   smtp_server_ip:	smtp server ip
 * @return  none
 */
void send_mail(uint8_t sn, uint8_t *buf, uint8_t *smtp_server_ip)
{
    volatile uint8_t ret;
    switch (SMTP_STATE)
    {
    case waitfor220:
        if (strstr((const char *)buf, "220") != NULL)
        {
            ret        = send(sn, (uint8_t *)hello, strlen(hello));
            SMTP_STATE = waitforHELO250;
        }
        else
        {
            printf("Connected failed!\r\n");
        }
        break;
    case waitforHELO250:
        if (strstr((const char *)buf, hello_reply) != NULL && strstr((const char *)buf, "Mail") == NULL)
        {
            ret        = send(sn, (uint8_t *)AUTH, strlen(AUTH));
            SMTP_STATE = waitforAUTH334;
        }
        else
        {
            printf("smtp handshake failed!\r\n");
        }
        break;
    case waitforAUTH334:
        if (strstr((const char *)buf, AUTH_reply) != NULL)
        {
            ret        = send(sn, (uint8_t *)base64name_126, strlen(base64name_126));
            SMTP_STATE = waitforuser334;
        }
        else
        {
            printf("AUTH authentication request failed!\r\n");
        }
        break;
    case waitforuser334:
        if (strstr((const char *)buf, name_reply) != NULL)
        {
            ret        = send(sn, (uint8_t *)base64password_126, strlen(base64password_126));
            SMTP_STATE = waitforpassword235;
        }
        else
        {
            printf("username send failed!\r\n");
        }
        break;
    case waitforpassword235:
        if (strstr((const char *)buf, password_reply) != NULL)
        {
            ret        = send(sn, (uint8_t *)mailfrom, strlen(mailfrom));
            SMTP_STATE = waitforsend250;
        }
        else
        {
            printf("password error!\r\n");
        }
        break;
    case waitforsend250:
        if (strstr((const char *)buf, from_reply) != NULL && strstr((const char *)buf, "queued as") == NULL)
        {
            ret        = send(sn, (uint8_t *)rcptto, strlen(rcptto));
            SMTP_STATE = waitforrcpt250;
        }
        else
        {
            printf("Send email failed to set up!\r\n");
        }
        break;
    case waitforrcpt250:
        if (strstr((const char *)buf, to_reply) != NULL)
        {
            ret        = send(sn, (uint8_t *)data_init, strlen(data_init));
            SMTP_STATE = waitfordate354;
        }
        else
        {
            printf("Failed to set the receiving mailbox!\r\n");
        }
        break;
    case waitfordate354:
        if (strstr((const char *)buf, data_reply) != NULL)
        {
            ret        = send(sn, (uint8_t *)mime, strlen(mime));
            SMTP_STATE = waitformime250;
        }
        else
        {
            printf("Failed to send content setup\r\n");
        }
        break;
    case waitformime250:
        if (strstr((const char *)buf, mime_reply) != NULL)
        {
            Mail_Send_OK = 1;
            printf("mail send OK\r\n");
        }
        break;
    default:
        break;
    }
}

/**
 * @brief base64 encode function
 * @param s : Input string
 * @param r : A string to store encoding results
 * @return none
 */
void base64encode(char *s, char *r) // base64 encoding conversion function
{
    char padstr[4];
    char base64chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    uint16_t i = 0, c;
    uint32_t n;
    c = strlen(s) % 3;
    if (c > 0)
    {
        for (i = 0; c < 3; c++)
        {
            padstr[i++] = '=';
        }
    }
    padstr[i] = 0;
    i         = 0;
    for (c = 0; c < strlen(s); c += 3)
    {
        n   = s[c];
        n <<= 8;
        n  += s[c + 1];
        if (c + 2 > strlen(s))
        {
            n &= 0xff00;
        }
        n <<= 8;
        n  += s[c + 2];
        if (c + 1 > strlen(s))
        {
            n &= 0xffff00;
        }
        r[i++] = base64chars[(n >> 18) & 63];
        r[i++] = base64chars[(n >> 12) & 63];
        r[i++] = base64chars[(n >> 6) & 63];
        r[i++] = base64chars[n & 63];
    }
    i -= strlen(padstr);
    for (c = 0; c < strlen(padstr); c++)
    {
        r[i++] = padstr[c];
    }
    r[i] = 0;
}

/**
 * @brief Insert string s2 at the specified position of string s1
 * @param s1 :Target string for inserting string s2
 * @param s2 :The string to insert
 * @param pos :Insert position, counting from 0
 * @return none
 */
void str_insert(char *s1, char *s2, int pos) // Insert the string s2 at the beginning of the pos bit of s1
{
    int i;
    int len = strlen(s2);
    for (i = 0; i < len; i++)
    {
        *(s1 + pos + len + i) = s1[pos + i];
        *(s1 + pos + i)       = s2[i];
    }
}

/**
 * @brief Send email content initialization
 * @param none
 * @return none
 */
void mailmessage(void)
{
    uint16_t len_from = strlen(from);
    uint16_t len_to   = strlen(to);
    uint16_t len_Cc   = strlen(Cc);
    uint16_t len_sub  = strlen(subject);
    strcat(hello, "\r\n");
    strcat(AUTH, "\r\n");
    base64encode(name_126, base64name_126);
    base64encode(password_126, base64password_126);
    strcat(base64name_126, "\r\n");
    strcat(base64password_126, "\r\n");
    str_insert(mailfrom, from, 11);
    strcat(mailfrom, "\r\n");
    str_insert(rcptto, to, 9);
    strcat(rcptto, "\r\n");
    strcat(data_init, "\r\n");

    str_insert(mime, from, 5);
    str_insert(mime1, to, 3);
    str_insert(mime2, Cc, 3);
    str_insert(mime3, subject, 8);
    str_insert(mime5, content, 0);
    strcat(mime, mime1);
    strcat(mime, mime2);
    strcat(mime, mime3);
    strcat(mime, mime4);
    strcat(mime, mime5);
}

/**
 * @brief   SMTP run function
 * @param   sn:	socket number
 * @return  none
 */
void do_smtp(uint8_t sn, uint8_t *buf, uint8_t *smtp_server_ip)
{
    volatile uint8_t ret;
    uint32_t         len       = 0;
    uint16_t         anyport   = 5000;
    uint8_t          Smtp_PORT = 25;
    memset(buf, 0, ETHERNET_MAX_BUF_SIZE);
    switch (getSn_SR(sn))
    {
    case SOCK_INIT:
        ret = connect(sn, smtp_server_ip, Smtp_PORT);
        break;
    case SOCK_ESTABLISHED:
        if (getSn_IR(sn) & Sn_IR_CON)
        {
            setSn_IR(sn, Sn_IR_CON);
        }

        while (!Mail_Send_OK)
        {
            len = getSn_RX_RSR(sn);
            if (len > 0)
            {
                memset(buf, 0, ETHERNET_MAX_BUF_SIZE);
                len = recv(sn, (uint8_t *)buf, len);
                send_mail(sn, buf, smtp_server_ip);
            }
        }
        disconnect(sn);
        break;
    case SOCK_CLOSE_WAIT:
        if ((len = getSn_RX_RSR(sn)) > 0)
        {
            while (!Mail_Send_OK)
            {
                len = recv(sn, (uint8_t *)buf, len);
                send_mail(sn, buf, smtp_server_ip);
            }
        }
        disconnect(sn);
        break;
    case SOCK_CLOSED:
        socket(sn, Sn_MR_TCP, anyport++, 0x00);
        break;
    default:
        break;
    }
    if (Mail_Send_OK)
    {
        while (1)
        {
        }
    }
}
