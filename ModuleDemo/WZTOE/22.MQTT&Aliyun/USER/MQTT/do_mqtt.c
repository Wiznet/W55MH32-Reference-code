#include "do_mqtt.h"
#include "wizchip_conf.h"
#include "MQTTClient.h"
#include "mqtt_interface.h"
#include "wiz_interface.h"
#include "do_dns.h"
#include "cJSON.h"
#include <string.h>
#include "delay.h"

#define MQTT_ETHERNET_MAX_SIZE (1024 * 2)

MQTTClient c = {0};
Network n = {0};
int connOK;
static uint8_t run_status = CONN;
mqttconn mqtt_params = {
    .mqttHostUrl = "iot-06z00dbroeg8dx3.mqtt.iothub.aliyuncs.com",
    .server_ip = {
        0,
    },                                                                                             /*Define the Connection Server IP*/
    .port = 1883,                                                                                  /*Define the connection service port number*/
    .clientid = "ieojgBm5q2c.W55MH32|securemode=2,signmethod=hmacsha256,timestamp=1737098648080|", /*Define the client ID*/
    .username = "W55MH32&ieojgBm5q2c",                                                             /*Define the user name*/
    .passwd = "92c427a483f8bc09d20baf445505e319ae4d89ec94360710d23b8ab2f12c58c2",                  /*Define user passwords*/
    .pubtopic = "/sys/ieojgBm5q2c/W55MH32/thing/event/property/post",                              /*Define the publication message*/
    .subtopic = "/sys/ieojgBm5q2c/W55MH32/thing/service/property/set",                             /*Define subscription messages*/
    .pubQoS = QOS0,                                                                                /*Defines the class of service for publishing messages*/
};

static char mqtt_recv_msg[512] = {0};
static uint8_t mqtt_recv_flag = 0;

MQTTMessage pubmessage = {
    .qos = QOS0,
    .retained = 0,
    .dup = 0,
    .id = 0,
};
MQTTPacket_willOptions willdata = MQTTPacket_willOptions_initializer; /* Will subject struct initialization */
MQTTPacket_connectData data = MQTTPacket_connectData_initializer;     /*Define the parameters of the MQTT connection*/

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
        while (1)
        {
        }
    }
    NewNetwork(&n, sn);                                          /*Obtain network configuration information*/
    ConnectNetwork(&n, mqtt_params.server_ip, mqtt_params.port); /*Connect to the MQTT server*/
    MQTTClientInit(&c, &n, 1000, send_buf, MQTT_ETHERNET_MAX_SIZE, recv_buf, MQTT_ETHERNET_MAX_SIZE);
    data.will = willdata;
    data.willFlag = 0;                                                            /* will flag: If the will annotation bit is 0, the following will-related settings are invalid*/
    willdata.qos = mqtt_params.willQoS;                                           /* will QoS */
    willdata.topicName.lenstring.data = mqtt_params.willtopic;                    /* will topic */
    willdata.topicName.lenstring.len = strlen(willdata.topicName.lenstring.data); /* will topic len */
    willdata.message.lenstring.data = mqtt_params.willmsg;                        /* will message */
    willdata.message.lenstring.len = strlen(willdata.message.lenstring.data);     /* will message len */
    willdata.retained = 0;
    willdata.struct_version = 3;
    data.MQTTVersion = 4;
    data.clientID.cstring = mqtt_params.clientid;
    data.username.cstring = mqtt_params.username;
    data.password.cstring = mqtt_params.passwd;
    data.keepAliveInterval = 30;
    data.cleansession = 1;
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
    cJSON *jsondata = NULL;
    cJSON *params = NULL;
    cJSON *LED = NULL;
    jsondata = cJSON_Parse(msg);
    if (jsondata == NULL)
    {
        printf("json parse fail.\r\n");
        return;
    }
    params = cJSON_GetObjectItem(jsondata, "params");
    LED = cJSON_GetObjectItem(params, "LEDSwitch");
    if (LED->valueint == 1)
    {
        printf("LED ON\r\n");
    }
    else
    {
        printf("LED OFF\r\n");
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
    char topicname[64] = {0};
    char msg[512] = {0};
    sprintf(topicname, "%.*s", (int)md->topicName->lenstring.len, md->topicName->lenstring.data);
    sprintf(msg, "%.*s", (int)md->message->payloadlen, (char *)md->message->payload);
    printf("recv:%s,%s\r\n\r\n", topicname, msg);

    mqtt_recv_flag = 1;
    memset(mqtt_recv_msg, 0, sizeof(mqtt_recv_msg));
    memcpy(mqtt_recv_msg, msg, strlen(msg));
}

/**
 * @brief Perform MQTT operations
 *
 * Perform the corresponding operations of MQTT based on the current operating state, including connecting, subscribing, publishing messages, and maintaining connections.
 */
void do_mqtt(void)
{
    uint8_t ret;
    switch (run_status)
    {
    case CONN:
    {
        ret = MQTTConnect(&c, &data); /* Connect to the MQTT server */
        printf("Connect to the MQTT server: %d.%d.%d.%d:%d\r\n", mqtt_params.server_ip[0], mqtt_params.server_ip[1], mqtt_params.server_ip[2], mqtt_params.server_ip[3], mqtt_params.port);
        printf("Connected:%s\r\n\r\n", ret == SUCCESSS ? "success" : "failed");
        if (ret != SUCCESSS)
        {
            run_status = ERR;
        }
        else
        {
            run_status = SUB;
        }
        break;
    }
    case SUB:
    {
        ret = MQTTSubscribe(&c, mqtt_params.subtopic, mqtt_params.subQoS, messageArrived); /* Subscribe to Topics */
        printf("Subscribing to %s\r\n", mqtt_params.subtopic);
        printf("Subscribed:%s\r\n\r\n", ret == SUCCESSS ? "success" : "failed");
        if (ret != SUCCESSS)
        {
            run_status = ERR;
        }
        else
        {
            run_status = PUB_MESSAGE;
        }
        break;
    }
    case PUB_MESSAGE:
    {
        pubmessage.qos = QOS0;
        pubmessage.payload = "{\"id\":\"123\",\"version\":\"1.0\",\"params\":{\"CurrentTemperature\":26.6,},\"method\":\"thing.event.property.post\"}";
        pubmessage.payloadlen = strlen(pubmessage.payload);
        ret = MQTTPublish(&c, (char *)&(mqtt_params.pubtopic), &pubmessage); /* Publish message */
        if (ret != SUCCESSS)
        {
            run_status = ERR;
        }
        else
        {
            printf("publish:%s,%s\r\n\r\n", mqtt_params.pubtopic, (char *)pubmessage.payload);
            run_status = KEEPALIVE;
        }
        break;
    }
    case KEEPALIVE:
    {
        if (MQTTYield(&c, 30) != SUCCESSS) /* keepalive MQTT */
        {
            run_status = ERR;
        }
    }
    case RECV:
    {
        if (mqtt_recv_flag)
        {
            mqtt_recv_flag = 0;
            json_decode(mqtt_recv_msg);
        }
        break;
    }
    case ERR: /* Running error */
        printf("system ERROR!");
        delay_ms(1000);
        break;

    default:
        break;
    }
}
