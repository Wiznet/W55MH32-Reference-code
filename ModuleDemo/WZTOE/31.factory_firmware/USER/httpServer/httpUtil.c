/**
 * @file	httpUtil.c
 * @brief	HTTP Server Utilities	
 * @version 1.0
 * @date	2014/07/15
 * @par Revision
 *			2014/07/15 - 1.0 Release
 * @author	
 * \n\n @par Copyright (C) 1998 - 2014 WIZnet. All rights reserved.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "httpUtil.h"
#include "webpage.h"
#include "wizchip_conf.h"
#include "wiz_interface.h"
#include "eeprom.h"

uint8_t http_get_cgi_handler(uint8_t *uri_name, uint8_t *buf, uint32_t *file_len)
{
    uint8_t  ret = HTTP_OK;
    uint16_t len = 0;

    if (predefined_get_cgi_processor(uri_name, buf, &len))
    {
        ;
    }
    else if (strcmp((const char *)uri_name, "example.cgi") == 0)
    {
        // To do
        ;
    }
    else
    {
        // CGI file not found
        ret = HTTP_FAILED;
    }

    if (ret) *file_len = len;
    return ret;
}

uint8_t http_post_cgi_handler(uint8_t *uri_name, st_http_request *p_http_request, uint8_t *buf, uint32_t *file_len)
{
    uint8_t  ret = HTTP_OK;
    uint16_t len = 0;
    uint8_t  val = 0;

    if (predefined_set_cgi_processor(uri_name, p_http_request->URI, buf, &len))
    {
        ;
    }
    else if (strcmp((const char *)uri_name, "example.cgi") == 0)
    {
        // To do
        val = 1;
        len = sprintf((char *)buf, "%d", val);
    }

    else
    {
        // CGI file not found
        ret = HTTP_FAILED;
    }

    if (ret) *file_len = len;
    return ret;
}


__attribute__((weak)) uint8_t predefined_get_cgi_processor(uint8_t *uri_name, uint8_t *buf, uint16_t *len)
{
    return 0;
}

__attribute__((weak)) uint8_t predefined_set_cgi_processor(uint8_t *uri_name, uint8_t *uri, uint8_t *buf, uint16_t *len)
{
    return 0;
}

//uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len)
//{
//	return 0;
//}

//uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len)
//{
//
////	if(strcmp((const char *)uri_name, "config.cgi") == 0)
////	{
////		uint8_t *param;
////		wiz_NetInfo new_net_info={0};
////		wizchip_getnetinfo(&new_net_info);
////		param = get_http_param_value((char*)uri,"ip");
////		parse_ip((char*)param,new_net_info.ip);
////		param = get_http_param_value((char*)uri,"subnet");
////		parse_ip((char*)param,new_net_info.sn);
////		param = get_http_param_value((char*)uri,"gateway");
////		parse_ip((char*)param,new_net_info.gw);
////		param = get_http_param_value((char*)uri,"dns");
////		parse_ip((char*)param,new_net_info.dns);
////
////    // ���� dhcp ģʽΪ STATIC
////    new_net_info.dhcp = NETINFO_STATIC;
////		ee_WriteBytes((uint8_t*)&new_net_info,0,sizeof(wiz_NetInfo));
////		reboot_flag = 1;
////		//memcpy((char*)buf,(char*)CONFIG_SUCCESS_PAGE,sizeof(CONFIG_SUCCESS_PAGE));
////		sprintf((char*)buf,(char*)CONFIG_SUCCESS_PAGE,new_net_info.ip[0],new_net_info.ip[1],new_net_info.ip[2],new_net_info.ip[3]);
////		*len=strlen((char*)buf);
////		return 1;
////	}
//	return 0;
//}

