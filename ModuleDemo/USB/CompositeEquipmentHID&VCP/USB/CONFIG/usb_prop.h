

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _HID_REQUESTS
{
    GET_REPORT = 1,
    GET_IDLE,
    GET_PROTOCOL,

    SET_REPORT = 9,
    SET_IDLE,
    SET_PROTOCOL
} HID_REQUESTS;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void     CustomHID_init(void);
void     CustomHID_Reset(void);
void     CustomHID_SetConfiguration(void);
void     CustomHID_SetDeviceAddress(void);
void     CustomHID_Status_In(void);
void     CustomHID_Status_Out(void);
RESULT   CustomHID_Data_Setup(uint8_t);
RESULT   CustomHID_NoData_Setup(uint8_t);
RESULT   CustomHID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *CustomHID_GetDeviceDescriptor(uint16_t);
uint8_t *CustomHID_GetConfigDescriptor(uint16_t);
uint8_t *CustomHID_GetStringDescriptor(uint16_t);
RESULT   CustomHID_SetProtocol(void);
uint8_t *CustomHID_GetProtocolValue(uint16_t Length);
RESULT   CustomHID_SetProtocol(void);
uint8_t *CustomHID_GetReportDescriptor(uint16_t Length);
uint8_t *CustomHID_GetHIDDescriptor(uint16_t Length);


/* Exported define -----------------------------------------------------------*/
#define CustomHID_GetConfiguration NOP_Process
//#define CustomHID_SetConfiguration          NOP_Process
#define CustomHID_GetInterface       NOP_Process
#define CustomHID_SetInterface       NOP_Process
#define CustomHID_GetStatus          NOP_Process
#define CustomHID_ClearFeature       NOP_Process
#define CustomHID_SetEndPointFeature NOP_Process
#define CustomHID_SetDeviceFeature   NOP_Process
//#define CustomHID_SetDeviceAddress          NOP_Process

#define REPORT_DESCRIPTOR 0x22

#define Virtual_Com_Port_GetConfiguration NOP_Process
//#define Virtual_Com_Port_SetConfiguration          NOP_Process
#define Virtual_Com_Port_GetInterface       NOP_Process
#define Virtual_Com_Port_SetInterface       NOP_Process
#define Virtual_Com_Port_GetStatus          NOP_Process
#define Virtual_Com_Port_ClearFeature       NOP_Process
#define Virtual_Com_Port_SetEndPointFeature NOP_Process
#define Virtual_Com_Port_SetDeviceFeature   NOP_Process
//#define Virtual_Com_Port_SetDeviceAddress          NOP_Process
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

#define SEND_ENCAPSULATED_COMMAND 0x00
#define GET_ENCAPSULATED_RESPONSE 0x01
#define SET_COMM_FEATURE          0x02
#define GET_COMM_FEATURE          0x03
#define CLEAR_COMM_FEATURE        0x04
#define SET_LINE_CODING           0x20
#define GET_LINE_CODING           0x21
#define SET_CONTROL_LINE_STATE    0x22
#define SEND_BREAK                0x23

typedef struct
{
    uint32_t bitrate;
    uint8_t  format;
    uint8_t  paritytype;
    uint8_t  datatype;
} LINE_CODING;

#endif /* __USB_PROP_H */


