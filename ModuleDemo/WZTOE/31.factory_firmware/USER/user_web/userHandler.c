/**
 * @file	userHandler.c
 * @brief	User Control Example
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userHandler.h"
#include "wizchip_conf.h"
#include "httpParser.h"
#include "w55mh32.h"
#include "user_gpio.h"
#include "eeprom.h"
#include "aht20.h"
#include "hw_config.h"
#include "user_sdio.h"

// Pre-defined Get CGI functions
void make_json_netinfo(uint8_t *buf, uint16_t *len);
void make_json_update_page(uint8_t *buf, uint16_t *len);
// Pre-defined Set CGI functions
int8_t set_diodir(uint8_t *uri);
int8_t set_diostate(uint8_t *uri);
void   set_netinfo(uint8_t *uri, uint8_t *buf, uint16_t *len);

extern uint8_t      reboot_flag;
extern MYI2C_Struct SENx;
extern uint16_t     btn_cnt;
extern uint8_t      usb_conn_flag;
extern uint8_t      usb_data[20][255];
extern uint16_t     usb_data_len[20];
extern uint8_t      data_cnt;
extern uint8_t      file_cnt;
extern uint8_t      file_name[20][255];

uint8_t send_data[2048];

uint8_t predefined_get_cgi_processor(uint8_t *uri_name, uint8_t *buf, uint16_t *len)
{
    uint8_t ret = 1; // ret = 1 means 'uri_name' matched

    if (strcmp((const char *)uri_name, "todo.cgi") == 0)
    {
        // to do
        ; // make_json_todo(buf, len);
    }
    else if (strcmp((const char *)uri_name, "get_netinfo.cgi") == 0)
    {
        make_json_netinfo(buf, len);
    }
    else if (strcmp((const char *)uri_name, "get_update_page.cgi") == 0)
    {
        make_json_update_page(buf, len);
    }
    else if (strcmp((const char *)uri_name, "get_sd.cgi") == 0)
    {
        uint8_t temp[255] = {0};
        uint8_t i;
        if (get_sd_filename() == 0)
        {
            *len = sprintf((char *)buf, "alert('SD卡未插入或识别异常!');");
        }
        else
        {
            memset(send_data, 0, sizeof(send_data));
            printf("file_cnt:%d\r\n", file_cnt);
            for (i = 0; i < file_cnt; i++)
            {
                sprintf((char *)temp, "%s\\n", file_name[i]);
                strcat((char *)send_data, (char *)temp);
            }
            printf("send_data:%s\r\n", send_data);
            *len = sprintf((char *)buf, "$('sd-textbox').value=\"%s\";", send_data);
            memset(send_data, 0, sizeof(send_data));
            memset(file_name, 0, sizeof(file_name));
            file_cnt = 0;
        }
    }
    else
    {
        ret = 0;
    }
    return ret;
}

uint8_t predefined_set_cgi_processor(uint8_t *uri_name, uint8_t *uri, uint8_t *buf, uint16_t *len)
{
    uint8_t ret = 1; // ret = '1' means 'uri_name' matched

    if (strcmp((const char *)uri_name, "todo.cgi") == 0)
    {
        // to do
        ;   // val = todo(uri);
            //*len = sprintf((char *)buf, "%d", val);
    }
    else if (strcmp((const char *)uri_name, "set_io_state.cgi") == 0)
    {
        uint8_t *param;
        uint8_t  name[10];
        uint8_t  val;
        param = get_http_param_value((char *)uri, "name");
        if (param) // GPIO;
        {
            strcpy((char *)name, (char *)param);
            param = get_http_param_value((char *)uri, "val");
            if (param) // State; high(on)/low(off)
            {
                val = (uint8_t)ATOI(param, 10);
            }
            if (strcmp((char *)name, (char *)"led") == 0)
            {
                if (val == 1)
                {
                    led_on();
                }
                else
                {
                    led_off();
                }
            }
            if (strcmp((char *)name, (char *)"beep") == 0)
            {
                if (val == 1)
                {
                    beep_on();
                }
                else
                {
                    beep_off();
                }
            }
        }
        *len = sprintf((char *)buf, "console.log('%s is %s');", name, val ? "on" : "off");
    }
    else if (strcmp((const char *)uri_name, "set_netinfo.cgi") == 0)
    {
        set_netinfo(uri, buf, len);
    }
    else if (strcmp((const char *)uri_name, "web_send_usb_data.cgi") == 0)
    {
        uint8_t *param;
        if (usb_conn_flag)
        {
            param = get_http_param_value((char *)uri, "data");
            if (param)
            {
                *len = sprintf((char *)buf, "$('usb-textbox').value='Tx->:'+$('usb-input').value+'\\n'+$('usb-textbox').value;");
                usb_printf("Rx->:%s\r\n", param);
            }
        }
        else
        {
            *len = sprintf((char *)buf, "alert('USB未连接');");
        }
    }
    else if (strcmp((const char *)uri_name, "put_sd.cgi") == 0)
    {
        uint8_t  filename[255];
        uint16_t filelen;
        uint8_t *param;
        param = get_http_param_value((char *)uri, "filename");
        if (param) // GPIO;
        {
            strcpy((char *)filename, (char *)param);
        }
        param = get_http_param_value((char *)uri, "filelen");
        if (param) // GPIO;
        {
            filelen = ATOI(param, 10);
        }
        param = get_http_param_value((char *)uri, "filedata");
        if (put_sd(filename, param, filelen))
        {
            *len = sprintf((char *)buf, "alert('存储成功');readContent();");
        }
        else
        {
            *len = sprintf((char *)buf, "alert('存储文件失败！请检查SD卡是否正确插入！')");
        }
    }
    else
    {
        ret = 0;
    }

    return ret;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pre-defined Get CGI functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void make_json_netinfo(uint8_t *buf, uint16_t *len)
{
    wiz_NetInfo netinfo;
    // ctlnetwork(CN_GET_NETINFO, (void*) &netinfo);
    wizchip_getnetinfo((void *)&netinfo);
    // DHCP: 1 - Static, 2 - DHCP

    *len = sprintf((char *)buf, "get_netinfo_callback({"
                                "\"ip\":\"%d.%d.%d.%d\","
                                "\"gw\":\"%d.%d.%d.%d\","
                                "\"sn\":\"%d.%d.%d.%d\","
                                "\"dns\":\"%d.%d.%d.%d\"});",
                   netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3],
                   netinfo.gw[0], netinfo.gw[1], netinfo.gw[2], netinfo.gw[3],
                   netinfo.sn[0], netinfo.sn[1], netinfo.sn[2], netinfo.sn[3],
                   netinfo.dns[0], netinfo.dns[1], netinfo.dns[2], netinfo.dns[3]);
}

static void parse_ip(const char *str, uint8_t *ip)
{
    int temp[4];
    sscanf(str, "%d.%d.%d.%d", &temp[0], &temp[1], &temp[2], &temp[3]);
    for (int i = 0; i < 4; i++)
    {
        ip[i] = (uint8_t)temp[i];
    }
}

void set_netinfo(uint8_t *uri, uint8_t *buf, uint16_t *len)
{
    uint8_t     ip[20], sn[20], gw[20], dns[20], *param;
    wiz_NetInfo netinfo;
    wizchip_getnetinfo(&netinfo);
    param = get_http_param_value((char *)uri, "ip");
    strcpy((char *)ip, (char *)param);
    param = get_http_param_value((char *)uri, "sn");
    strcpy((char *)sn, (char *)param);
    param = get_http_param_value((char *)uri, "gw");
    strcpy((char *)gw, (char *)param);
    param = get_http_param_value((char *)uri, "dns");
    strcpy((char *)dns, (char *)param);
    parse_ip((char *)ip, netinfo.ip);
    parse_ip((char *)sn, netinfo.sn);
    parse_ip((char *)gw, netinfo.gw);
    parse_ip((char *)dns, netinfo.dns);
    netinfo.dhcp = NETINFO_STATIC;
    ee_WriteBytes((uint8_t *)&netinfo, 0, sizeof(wiz_NetInfo));
    reboot_flag = 1;
    *len        = sprintf((char *)buf, "alert(\"网络配置成功，即将跳转至新页面！\");setTimeout(function(){location.href='http://%d.%d.%d.%d/';}, 3000 );", netinfo.ip[0], netinfo.ip[1], netinfo.ip[2], netinfo.ip[3]);
}
void make_json_update_page(uint8_t *buf, uint16_t *len)
{
    uint8_t temp[255] = {0};
    uint8_t i;
    if (data_cnt > 0)
    {
        for (i = 0; i < data_cnt; i++)
        {
            sprintf((char *)temp, "Rx->:%s\\n", usb_data[i]);
            strcat((char *)send_data, (char *)temp);
            memset(temp, 0, sizeof(temp));
        }
        *len     = sprintf((char *)buf, "update_page_callback({"
                                            "\"temperature\":\"%.2f\","
                                            "\"humidity\":\"%.2f\","
                                            "\"button_cnt\":\"%d\","
                                            "\"usbdata\":\"%s\"});",
                           SENx.T, SENx.RH, btn_cnt, send_data);
        data_cnt = 0;
        printf("%d,%d\r\n", sizeof(usb_data), sizeof(send_data));
        memset(usb_data, 0, sizeof(usb_data));
        memset(send_data, 0, sizeof(send_data));
    }
    else
    {
        *len = sprintf((char *)buf, "update_page_callback({"
                                    "\"temperature\":\"%.2f\","
                                    "\"humidity\":\"%.2f\","
                                    "\"button_cnt\":\"%d\"});",
                       SENx.T, SENx.RH, btn_cnt);
    }
}
