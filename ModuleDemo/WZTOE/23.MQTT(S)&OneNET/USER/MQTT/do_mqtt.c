#include "do_mqtt.h"
#include "wizchip_conf.h"
#include "MQTTClient.h"
#include "mqtt_interface.h"
#include "wiz_interface.h"
#include "do_dns.h"
#include "cJSON.h"
#include <string.h>
#include "delay.h"
#include "SSLInterface.h"
#include "socket.h"
#include "aht20.h"
#include "user_gpio.h"

#define MQTT_ETHERNET_MAX_SIZE (1024 * 2)

extern wiz_tls_context tlsContext;
extern MYI2C_Struct    SENx;

MQTTClient     c = {0};
Network        n = {0};
int            connOK;
static uint8_t run_status  = TCP_CONN;
mqttconn       mqtt_params = {
          .server_ip   = {
                          0,
                          }, /*Define the Connection Server IP*/
#ifdef MQTTS
    .mqttHostUrl = "mqttstls.heclouds.com",
    .port = 8883, /*Define the connection service port number*/
#else
    .mqttHostUrl = "mqtts.heclouds.com",
    .port = 1883, /*Define the connection service port number*/
#endif
    .local_port     = 50000,
    .clientid       = "W5100S_W5500", /*Define the client ID*/
    .username       = "70TwP2gxl5", /*Define the user name*/
    .passwd         = "version=2018-10-31&res=products%2F70TwP2gxl5%2Fdevices%2FW5100S_W5500&et=1791400694&method=sha1&sign=0SchVg6Y2MRYn%2B9zItNZwt%2F%2FN4Y%3D", /*Define user passwords*/
    .pubtopic       = "$sys/70TwP2gxl5/W5100S_W5500/thing/property/post", /*Define the publication message*/
    .pubtopic_reply = "$sys/70TwP2gxl5/W5100S_W5500/thing/property/post/reply",
    .subtopic       = "$sys/70TwP2gxl5/W5100S_W5500/thing/property/set", /*Define subscription messages*/
    .subtopic_reply = "$sys/70TwP2gxl5/W5100S_W5500/thing/property/set_reply",
    .pubQoS         = QOS0, /*Defines the class of service for publishing messages*/
};

static char    mqtt_recv_msg[512] = {0};
static char    mqtt_send_msg[512] = {0};
static uint8_t mqtt_recv_flag     = 0;
uint8_t        mqtt_send_flag     = 0;


MQTTMessage pubmessage = {
    .qos      = QOS0,
    .retained = 0,
    .dup      = 0,
    .id       = 0,
    .payload  = mqtt_send_msg,
};

MQTTPacket_willOptions willdata = MQTTPacket_willOptions_initializer; /* Will subject struct initialization */
MQTTPacket_connectData data     = MQTTPacket_connectData_initializer; /*Define the parameters of the MQTT connection*/

#ifdef MQTTS
/*
 * @brief read function
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 *         buffer : pointer to a read buffer.
 *         len : buffer length.
 * @retval received data length or SOCKERR code
 */
int tls_mqtt_read(Network *n, unsigned char *buffer, int len, long time)
{
    return wiz_tls_read(&tlsContext, buffer, len);
}

/*
 * @brief write function
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 *         buffer : pointer to a read buffer.
 *         len : buffer length.
 * @retval length of data sent or SOCKERR code
 */
int tls_mqtt_write(Network *n, unsigned char *buffer, int len, long time)
{
    return wiz_tls_write(&tlsContext, buffer, len);
}

/*
 * @brief disconnect function
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 */
void tls_mqtt_disconnect(Network *n)
{
    wiz_tls_disconnect(&tlsContext, 2000);
}
#endif

/**
 * @brief Initializing the MQTT client side
 *
 * Initialize the MQTT client side with the given parameters, including network configuration and MQTT connection parameters.
 *
 * @param sn socket number
 * @param send_buf send buffer pointer
 * @param recv_buf recv buffer pointer
 */
void mqtt_init(uint8_t sn, uint8_t *send_buf, uint8_t *recv_buf)
{
    wiz_NetInfo get_info = {0};
    wizchip_getnetinfo(&get_info);
    /* DNS parsing */
    if (do_dns(send_buf, (uint8_t *)mqtt_params.mqttHostUrl, mqtt_params.server_ip))
    {
        while (1);
    }
    NewNetwork(&n, sn); /*Obtain network configuration information*/
    MQTTClientInit(&c, &n, 2000, send_buf, MQTT_ETHERNET_MAX_SIZE, recv_buf, MQTT_ETHERNET_MAX_SIZE);
#ifdef MQTTS
    n.mqttread   = tls_mqtt_read;
    n.mqttwrite  = tls_mqtt_write;
    n.disconnect = tls_mqtt_disconnect;
#endif

    data.will                         = willdata;
    data.willFlag                     = 0;                                         /* will flag: If the will annotation bit is 0, the following will-related settings are invalid*/
    willdata.qos                      = mqtt_params.willQoS;                       /* will QoS */
    willdata.topicName.lenstring.data = mqtt_params.willtopic;                     /* will topic */
    willdata.topicName.lenstring.len  = strlen(willdata.topicName.lenstring.data); /* will topic len */
    willdata.message.lenstring.data   = mqtt_params.willmsg;                       /* will message */
    willdata.message.lenstring.len    = strlen(willdata.message.lenstring.data);   /* will message len */
    willdata.retained                 = 0;
    willdata.struct_version           = 3;
    data.MQTTVersion                  = 4;
    data.clientID.cstring             = mqtt_params.clientid;
    data.username.cstring             = mqtt_params.username;
    data.password.cstring             = mqtt_params.passwd;
    data.keepAliveInterval            = 30;
    data.cleansession                 = 1;
}

/**
 * @brief Cloud JSON message parsing
 *
 * Parses the given JSON message and executes the corresponding operation based on the parsed result.
 *
 * @param msg ：JSON message pointer
 */
void json_decode(char *msg)
{
    int         ret;
    static char replymsg[128] = {0};
    cJSON      *id            = NULL;
    cJSON      *jsondata      = NULL;
    cJSON      *params        = NULL;
    cJSON      *LED           = NULL;
    cJSON      *BEEP          = NULL;
    jsondata                  = cJSON_Parse(msg);
    if (jsondata == NULL)
    {
        printf("json parse fail.\r\n");
        return;
    }
    id     = cJSON_GetObjectItem(jsondata, "id");
    params = cJSON_GetObjectItem(jsondata, "params");
    LED    = cJSON_GetObjectItem(params, "LEDSwitch");
    if (LED != NULL)
    {
        if (LED->valueint)
        {
            printf("LED ON\r\n");
            GPIO_ResetBits(LED_PORT, LED_PIN);
        }
        else
        {
            printf("LED OFF\r\n");
            GPIO_SetBits(LED_PORT, LED_PIN);
        }
    }

    BEEP = cJSON_GetObjectItem(params, "BEEP");

    if (BEEP != NULL)
    {
        if (BEEP->valueint)
        {
            printf("BEEP ON\r\n");
            GPIO_SetBits(BEEP_PORT, BEEP_PIN);
        }
        else
        {
            printf("BEEP OFF\r\n");
            GPIO_ResetBits(BEEP_PORT, BEEP_PIN);
        }
    }
    pubmessage.qos = QOS0;
    sprintf(replymsg, "{\"id\":\"%s\",\"code\":200,\"msg\":\"success\"}", id->valuestring);
    printf("reply:%s\r\n", replymsg);
    pubmessage.payload    = replymsg;
    pubmessage.payloadlen = strlen(replymsg);
    ret                   = MQTTPublish(&c, mqtt_params.subtopic_reply, &pubmessage); /* Publish message */
    if (ret != SUCCESSS)
    {
        run_status = CLOSE;
    }
    else
    {
        printf("publish:%s,%s\r\n\r\n", mqtt_params.subtopic_reply, (char *)pubmessage.payload);
    }
    cJSON_Delete(jsondata);
}

/**
 * @brief mqtt Receive message callback function
 *
 * This function is called when a message is received. The function will parse the message content and process it accordingly.
 *
 * @param md :message data pointer
 */
void messageArrived(MessageData *md)
{
    static char topicname[64] = {0};
    static char msg[512]      = {0};
    sprintf(topicname, "%.*s", (int)md->topicName->lenstring.len, md->topicName->lenstring.data);
    sprintf(msg, "%.*s", (int)md->message->payloadlen, (char *)md->message->payload);
    printf("recv:%s,%s\r\n\r\n", topicname, msg);

    if (strcmp(topicname, mqtt_params.subtopic) == 0)
    {
        mqtt_recv_flag = 1;
        memset(mqtt_recv_msg, 0, sizeof(mqtt_recv_msg));
        memcpy(mqtt_recv_msg, msg, strlen(msg));
    }
}

/**
 * @brief Perform MQTT operations
 *
 * Perform the corresponding operations of MQTT based on the current operating state, including connecting, subscribing, publishing messages, and maintaining connections.
 */
void do_mqtt(void)
{
    int ret;
    switch (run_status)
    {
    case TCP_CONN: {
        if(mqtt_params.local_port > 60000)
        {
            mqtt_params.local_port = 50000;
        }
#ifdef MQTTS
        /* open socket */
        if(wiz_tls_socket(&tlsContext, n.my_socket, mqtt_params.local_port++) != n.my_socket)
        {
            printf("socket error.\r\n");
            run_status = CLOSE;
            break;
        }

        /* tls init */
        wiz_tls_init(&tlsContext, 2000, (char *)mqtt_params.server_ip, NULL, NULL, NULL);

        /* connect server*/
        if(connect(n.my_socket, (uint8_t *)mqtt_params.server_ip, mqtt_params.port) != SOCK_OK)
        {
            printf("connect error.\r\n");
            run_status = CLOSE;
            break;
        }
        /* tls handshake */
        wiz_tls_connect(&tlsContext, (char *)mqtt_params.server_ip, mqtt_params.port);
        mbedtls_ssl_conf_read_timeout(tlsContext.conf, 2000);
        run_status = MQTT_CONN;
        break;
#else
        if(socket(n.my_socket, Sn_MR_TCP,  mqtt_params.local_port++, 0) != n.my_socket)
        {
            printf("socket error.\r\n");
            run_status = CLOSE;
            break;
        }
        if(connect(n.my_socket, mqtt_params.server_ip, mqtt_params.port) != SOCK_OK)
        {
            printf("connect error.\r\n");
            run_status = CLOSE;
            break;
        }
        run_status = MQTT_CONN;
        break;
        // ConnectNetwork(&n, mqtt_params.server_ip, mqtt_params.port); /*Connect to the MQTT server*/
#endif
    }
    case MQTT_CONN: {
        ret = MQTTConnect(&c, &data); /* Connect to the MQTT server */
        printf("Connect to the MQTT server: %d.%d.%d.%d:%d\r\n", mqtt_params.server_ip[0], mqtt_params.server_ip[1], mqtt_params.server_ip[2], mqtt_params.server_ip[3], mqtt_params.port);
        printf("Connected:%s\r\n\r\n", ret == SUCCESSS ? "success" : "failed");
        if (ret != SUCCESSS)
        {
            run_status = CLOSE;
        }
        else
        {
            run_status = SUB;
        }
        break;
    }
    case SUB: {
        ret = MQTTSubscribe(&c, mqtt_params.subtopic, mqtt_params.subQoS, messageArrived); /* Subscribe to Topics */
        printf("Subscribing to %s\r\n", mqtt_params.subtopic);
        printf("Subscribed:%s\r\n\r\n", ret == SUCCESSS ? "success" : "failed");
        if (ret != SUCCESSS)
        {
            run_status = CLOSE;
            break;
        }
        else
        {
            run_status = PUB_MESSAGE;
        }
        ret = MQTTSubscribe(&c, mqtt_params.pubtopic_reply, mqtt_params.subQoS, messageArrived); /* Subscribe to Topics */
        printf("Subscribing to %s\r\n", mqtt_params.subtopic);
        printf("Subscribed:%s\r\n\r\n", ret == SUCCESSS ? "success" : "failed");
        if (ret != SUCCESSS)
        {
            run_status = CLOSE;
            break;
        }
        else
        {
            run_status = PUB_MESSAGE;
        }
        break;
    }
    case PUB_MESSAGE: {
        if (mqtt_send_flag)
        {
            mqtt_send_flag = 0;
            pubmessage.qos = QOS0;
            sprintf(pubmessage.payload, "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"LEDSwitch\":{\"value\":%s},\"BEEP\":{\"value\":%s},\"CurrentTemperature\":{\"value\":%.2f},\"CurrentHumidity\":{\"value\":%.2f}}}", GPIO_ReadOutputDataBit(LED_PORT, LED_PIN) == RESET ? "true" : "false", GPIO_ReadOutputDataBit(BEEP_PORT, BEEP_PIN) == SET ? "true" : "false", SENx.T, SENx.RH);
            pubmessage.payloadlen = strlen(pubmessage.payload);
            ret                   = MQTTPublish(&c, (char *)&(mqtt_params.pubtopic), &pubmessage); /* Publish message */
            if (ret != SUCCESSS)
            {
                run_status = CLOSE;
            }
            else
            {
                printf("publish:%s,%s\r\n\r\n", mqtt_params.pubtopic, (char *)pubmessage.payload);
            }
        }
    }
    case KEEPALIVE: {
        if (MQTTYield(&c, 30) != SUCCESSS) /* keepalive MQTT */
        {
            run_status = CLOSE;
        }
    }
    case RECV: {
        if (mqtt_recv_flag)
        {
            mqtt_recv_flag = 0;
            json_decode(mqtt_recv_msg);
        }
        break;
    }
    case CLOSE: /* Running error */
        printf("close socket.\r\n");
#ifdef MQTTS
        wiz_tls_disconnect(&tlsContext, 2000);
        wiz_tls_close(&tlsContext);
#else
        disconnect(n.my_socket);
        close(n.my_socket);
#endif
        run_status = TCP_CONN;
        delay_ms(1000);
        break;
    default:
        break;
    }
}
