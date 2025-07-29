

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const uint8_t CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] =
    {
        0x12,                       /*bLength */
        USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
        0x00,                       /*bcdUSB */
        0x02,
        0xEF,                       /*bDeviceClass*/
        0x02,                       /*bDeviceSubClass*/
        0x01,                       /*bDeviceProtocol*/
        0x40,                       /*bMaxPacketSize40*/
        0x83,                       /*idVendor (0x0483)*/
        0x04,
        0x50,                       /*idProduct = 0x5750*/
        0x57,
        0x00,                       /*bcdDevice rel. 2.00*/
        0x02,
        1,                          /*Index of string descriptor describing
                                              manufacturer */
        2,                          /*Index of string descriptor describing
                                             product*/
        3,                          /*Index of string descriptor describing the
                                             device serial number */
        0x01                        /*bNumConfigurations*/
}; /* CustomHID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] =
    {
        0x09,                              /* bLength: Configuration Descriptor size */
        USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
        CUSTOMHID_SIZ_CONFIG_DESC,
        /* wTotalLength: Bytes returned */
        0x00,
        0x03, /* bNumInterfaces: 1 interface */
        0x01, /* bConfigurationValue: Configuration value */
        0x00, /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
        0xC0, /* bmAttributes: Self powered */
        0x32, /* MaxPower 100 mA: this current is used for detecting Vbus */
        /**************IAD HID**********************************************/
        0x08,
        0x0B,
        0x00, //Start from 0
        0x01,
        0x03, //HID device
        0x00,
        0x00,
        0x01,
        /************** Descriptor of Custom HID interface ****************/
        /* 09 */
        0x09,                          /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface descriptor type */
        0x00,                          /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints */
        0x03,                          /* bInterfaceClass: HID */
        0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
        0x00,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
        0,                             /* iInterface: Index of string descriptor */
        /******************** Descriptor of Custom HID HID ********************/
        /* 18 */
        0x09,                      /* bLength: HID Descriptor size */
        HID_DESCRIPTOR_TYPE,       /* bDescriptorType: HID */
        0x10,                      /* bcdHID: HID Class Spec release number */
        0x01,
        0x00,                      /* bCountryCode: Hardware target country */
        0x01,                      /* bNumDescriptors: Number of HID class descriptors to follow */
        0x22,                      /* bDescriptorType */
        CUSTOMHID_SIZ_REPORT_DESC, /* wItemLength: Total length of Report descriptor */
        0x00,
        /******************** Descriptor of Custom HID endpoints ******************/
        /* 27 */
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */

        0x81,                         /* bEndpointAddress: Endpoint Address (IN) */
        0x03,                         /* bmAttributes: Interrupt endpoint */
        0x02,                         /* wMaxPacketSize: 2 Bytes max */
        0x00,
        0x20,                         /* bInterval: Polling Interval (32 ms) */
        /* 34 */

        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */
                                      /*	Endpoint descriptor type */
        0x01,                         /* bEndpointAddress: */
                                      /*	Endpoint Address (OUT) */
        0x03,                         /* bmAttributes: Interrupt endpoint */
        0x02,                         /* wMaxPacketSize: 2 Bytes max  */
        0x00,
        0x20,                         /* bInterval: Polling Interval (20 ms) */
                                      /* 41 */

                                      /*******************************COM 1***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        1, /*bFirstInterface*/                    //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01, /*bFunctionProtocol*/               //I don't understand the control protocol and other things, just default to it.
        0x02,                                     /*iFunction*/

        /* USB_INTERFACE_DESCRIPTOR*/
        0x09,
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */
        /* Interface descriptor type */
        0x01, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x00, /* bNumEndpoints: One endpoints used */
        0x02, /* bInterfaceClass: Communication Interface Class */
        0x02, /* bInterfaceSubClass: Abstract Control Model */
        0x01, /* bInterfaceProtocol: Common AT commands */
        0x00, /* iInterface: */
        /*Header Functional Descriptor*/
        0x05, /* bLength: Endpoint Descriptor size */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x00, /* bDescriptorSubtype: Header Func Desc */
        0x10, /* bcdCDC: spec release number */
        0x01,
        /*Call Management Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x01, /* bDescriptorSubtype: Call Management Func Desc */
        0x00, /* bmCapabilities: D0+D1 */
        0x01, /* bDataInterface: 1 */
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*Union Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        0x01, /* bMasterInterface: Communication class interface */
        0x02, /* bSlaveInterface0: Data Class Interface */
        /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        0x02,                          /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x07,                         /* bEndpointAddress: (OUT7) */
        0x02,                         /* bmAttributes: Bulk */
        VIRTUAL_COM_PORT_DATA_SIZE,   /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x87,                         /* bEndpointAddress: (IN7) */
        0x02,                         /* bmAttributes: Bulk */
        VIRTUAL_COM_PORT_DATA_SIZE,   /* wMaxPacketSize: */
        0x00,
        0x00                          /* bInterval */
}; /* CustomHID_ConfigDescriptor */

const uint8_t CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] =
    {
        0x06, 0xFF, 0x00, /* USAGE_PAGE (Vendor Page: 0xFF00) */
        0x09, 0x01,       /* USAGE (Demo Kit)               */
        0xa1, 0x01,       /* COLLECTION (Application)       */
        /* 6 */

        /* Led 1 */
        0x85, 0x01, /*     REPORT_ID (1)		     */
        0x09, 0x01, /*     USAGE (LED 1)	             */
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)        */
        0x25, 0x01, /*     LOGICAL_MAXIMUM (1)        */
        0x75, 0x08, /*     REPORT_SIZE (8)            */
        0x95, 0x01, /*     REPORT_COUNT (1)           */
        0xB1, 0x82, /*    FEATURE (Data,Var,Abs,Vol) */

        0x85, 0x01, /*     REPORT_ID (1)              */
        0x09, 0x01, /*     USAGE (LED 1)              */
        0x91, 0x82, /*     OUTPUT (Data,Var,Abs,Vol)  */
        /* 26 */

        /* Led 2 */
        0x85, 0x02, /*     REPORT_ID 2		     */
        0x09, 0x02, /*     USAGE (LED 2)	             */
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)        */
        0x25, 0x01, /*     LOGICAL_MAXIMUM (1)        */
        0x75, 0x08, /*     REPORT_SIZE (8)            */
        0x95, 0x01, /*     REPORT_COUNT (1)           */
        0xB1, 0x82, /*    FEATURE (Data,Var,Abs,Vol) */

        0x85, 0x02, /*     REPORT_ID (2)              */
        0x09, 0x02, /*     USAGE (LED 2)              */
        0x91, 0x82, /*     OUTPUT (Data,Var,Abs,Vol)  */
        /* 46 */

        /* Led 3 */
        0x85, 0x03, /*     REPORT_ID (3)		     */
        0x09, 0x03, /*     USAGE (LED 3)	             */
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)        */
        0x25, 0x01, /*     LOGICAL_MAXIMUM (1)        */
        0x75, 0x08, /*     REPORT_SIZE (8)            */
        0x95, 0x01, /*     REPORT_COUNT (1)           */
        0xB1, 0x82, /*    FEATURE (Data,Var,Abs,Vol) */

        0x85, 0x03, /*     REPORT_ID (3)              */
        0x09, 0x03, /*     USAGE (LED 3)              */
        0x91, 0x82, /*     OUTPUT (Data,Var,Abs,Vol)  */
        /* 66 */

        /* Led 4 */
        0x85, 0x04, /*     REPORT_ID 4)		     */
        0x09, 0x04, /*     USAGE (LED 4)	             */
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)        */
        0x25, 0x01, /*     LOGICAL_MAXIMUM (1)        */
        0x75, 0x08, /*     REPORT_SIZE (8)            */
        0x95, 0x01, /*     REPORT_COUNT (1)           */
        0xB1, 0x82, /*     FEATURE (Data,Var,Abs,Vol) */

        0x85, 0x04, /*     REPORT_ID (4)              */
        0x09, 0x04, /*     USAGE (LED 4)              */
        0x91, 0x82, /*     OUTPUT (Data,Var,Abs,Vol)  */
        /* 86 */

        /* key Push Button */
        0x85, 0x05, /*     REPORT_ID (5)              */
        0x09, 0x05, /*     USAGE (Push Button)        */
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)        */
        0x25, 0x01, /*     LOGICAL_MAXIMUM (1)        */
        0x75, 0x01, /*     REPORT_SIZE (1)            */
        0x81, 0x82, /*     INPUT (Data,Var,Abs,Vol)   */

        0x09, 0x05, /*     USAGE (Push Button)        */
        0x75, 0x01, /*     REPORT_SIZE (1)            */
        0xb1, 0x82, /*     FEATURE (Data,Var,Abs,Vol) */

        0x75, 0x07, /*     REPORT_SIZE (7)            */
        0x81, 0x83, /*     INPUT (Cnst,Var,Abs,Vol)   */
        0x85, 0x05, /*     REPORT_ID (2)              */

        0x75, 0x07, /*     REPORT_SIZE (7)            */
        0xb1, 0x83, /*     FEATURE (Cnst,Var,Abs,Vol) */
        /* 114 */

        /* Tamper Push Button */
        0x85, 0x06, /*     REPORT_ID (6)              */
        0x09, 0x06, /*     USAGE (Tamper Push Button) */
        0x15, 0x00, /*     LOGICAL_MINIMUM (0)        */
        0x25, 0x01, /*     LOGICAL_MAXIMUM (1)        */
        0x75, 0x01, /*     REPORT_SIZE (1)            */
        0x81, 0x82, /*     INPUT (Data,Var,Abs,Vol)   */

        0x09, 0x06, /*     USAGE (Tamper Push Button) */
        0x75, 0x01, /*     REPORT_SIZE (1)            */
        0xb1, 0x82, /*     FEATURE (Data,Var,Abs,Vol) */

        0x75, 0x07, /*     REPORT_SIZE (7)            */
        0x81, 0x83, /*     INPUT (Cnst,Var,Abs,Vol)   */
        0x85, 0x06, /*     REPORT_ID (6)              */

        0x75, 0x07, /*     REPORT_SIZE (7)            */
        0xb1, 0x83, /*     FEATURE (Cnst,Var,Abs,Vol) */
        /* 142 */

        /* ADC IN */
        0x85, 0x07,       /*     REPORT_ID (7)              */
        0x09, 0x07,       /*     USAGE (ADC IN)             */
        0x15, 0x00,       /*     LOGICAL_MINIMUM (0)        */
        0x26, 0xff, 0x00, /*     LOGICAL_MAXIMUM (255)      */
        0x75, 0x08,       /*     REPORT_SIZE (8)            */
        0x81, 0x82,       /*     INPUT (Data,Var,Abs,Vol)   */
        0x85, 0x07,       /*     REPORT_ID (7)              */
        0x09, 0x07,       /*     USAGE (ADC in)             */
        0xb1, 0x82,       /*     FEATURE (Data,Var,Abs,Vol) */
        /* 161 */

        0xc0 /*     END_COLLECTION	             */
}; /* CustomHID_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] =
    {
        CUSTOMHID_SIZ_STRING_LANGID,
        USB_STRING_DESCRIPTOR_TYPE,
        0x09,
        0x04}; /* LangID = 0x0409: U.S. English */

const uint8_t CustomHID_StringVendor[CUSTOMHID_SIZ_STRING_VENDOR] =
    {
        CUSTOMHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
        USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/

        'W', 0, 'I', 0, 'Z', 0, 'n', 0, 'e', 0, 't', 0};

const uint8_t CustomHID_StringProduct[CUSTOMHID_SIZ_STRING_PRODUCT] =
    {
        CUSTOMHID_SIZ_STRING_PRODUCT, /* bLength */
        USB_STRING_DESCRIPTOR_TYPE,   /* bDescriptorType */
        'W', 0, 'I', 0, 'Z', 0, 'n', 0, 'e', 0, 't', 0,
        ' ', 0, 'V', 0, 'C', 0, 'P', 0, 'H', 0, 'I', 0,
        'D', 0};
uint8_t CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL] =
    {
        CUSTOMHID_SIZ_STRING_SERIAL, /* bLength */
        USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType */
        'W', 0, '5', 0, '5', 0, 'M', 0, 'H', 0, ' ', 0, '3', 0, '2', 0};


