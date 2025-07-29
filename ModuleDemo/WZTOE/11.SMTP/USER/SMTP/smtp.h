#ifndef _SMTPUTIL_H_
#define _SMTPUTIL_H_

#include "socket.h"
// SMTP_STATE

enum run_state
{
    waitfor220 = 0,
    waitforHELO250,
    waitforAUTH334,
    waitforuser334,
    waitforpassword235,
    waitforsend250,
    waitforrcpt250,
    waitfordate354,
    waitformime250
};

/**
 * @brief Send mail message handler function
 * @param none
 * @return none
 */
void mailmessage(void);

/**
 * @brief   SMTP run function
 * @param   sn:	socket number
 * @return  none
 */
void do_smtp(uint8_t sn, uint8_t *buf, uint8_t *smtp_server_ip);
#endif
