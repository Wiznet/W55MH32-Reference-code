#ifndef __DO_MQTT_H__
#define __DO_MQTT_H__

#include <stdio.h>
#include <stdint.h>
#include "MQTTClient.h"

enum status
{
    CONN = 0,
    SUB,
    PUB_MESSAGE,
    KEEPALIVE,
    RECV,
    ERR = 255
};

typedef struct MQTTCONNECTION
{
    char     mqttHostUrl[1024]; /*Server URL*/
    uint8_t  server_ip[4];      /*Server IP*/
    int      port;              /*Server port number*/
    char     clientid[1024];    /*client ID*/
    char     username[1024];    /*user name*/
    char     passwd[1024];      /*user passwords*/
    char     pubtopic[255];     /*publication*/
    char     subtopic[255];     /*subscription*/
    enum QoS pubQoS;            /* publishing messages*/
    enum QoS subQoS;            /* subscription messages*/
    char     willtopic[255];    /*Will topic  */
    enum QoS willQoS;           /*Will message  */
    char     willmsg[255];      /*Will */
} mqttconn;

/**
 * @brief Initializing the MQTT client side
 *
 * Initialize the MQTT client side with the given parameters, including network configuration and MQTT connection parameters.
 *
 * @param sn socket number
 * @param send_buf send buffer pointer
 * @param recv_buf recv buffer pointer
 */
void mqtt_init(uint8_t sn, uint8_t *send_buf, uint8_t *recv_buf);

/**
 * @brief Perform MQTT operations
 *
 * Perform the corresponding operations of MQTT based on the current operating state, including connecting, subscribing, publishing messages, and maintaining connections.
 */
void do_mqtt(void);
#endif
