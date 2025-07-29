#ifndef __NETBIOS_H__
#define __NETBIOS_H__
#include <stdint.h>

/**
 *@brief  Execute the NetBIOS name resolver
 *@param  socket number
 *@return no
 */
void do_netbios(uint8_t SOCK_NETBIOS);

#endif //__NETBIOS_H__
