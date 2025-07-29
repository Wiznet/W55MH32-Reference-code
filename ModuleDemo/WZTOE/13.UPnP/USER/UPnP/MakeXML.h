
#ifndef __MakeXML_H__
#define __MakeXML_H__

#define DELETE_PORT 0
#define ADD_PORT    1

/**
 * @brief   This function makes the HTTP POST Header.
 * @param   dest:Target string pointer
 * @param   content_length: content length
 * @param   action: action type
 * @return  none
 */
void MakePOSTHeader(char *dest, int content_length, int action);

/**
 * @brief   This function makes the HTTP GET header.
 * @param   dest:Target string pointer
 * @return  none
 */
void MakeGETHeader(char *dest);

/**
 * @brief   This function makes the Subscription message.
 * @param   dest:Target string pointer
 * @param   listen_port:Listen port
 * @return  none
 */
void MakeSubscribe(char *dest, const unsigned int listen_port);

/**
 * @brief   This function makes the Add Port Control message in SOAP.
 * @param   dest:Target string pointer
 * @param   protocol:Protocol type
 * @param   extertnal_port:External port
 * @param   internal_ip:Internal IP address
 * @param   internal_port:Internal port
 * @param   description:Description
 * @return  none
 */
void MakeSOAPAddControl(char *dest, const char *protocol, const unsigned int extertnal_port, const char *internal_ip, const unsigned int internal_port, const char *description);

/**
 * @brief   This function makes the Delete Port Control message in SOAP.
 * @param   dest:Target string pointer
 * @param   protocol:Protocol type
 * @param   extertnal_port:External port
 * @return  none
 */
void MakeSOAPDeleteControl(char *dest, const char *protocol, const unsigned int extertnal_port);

#endif
