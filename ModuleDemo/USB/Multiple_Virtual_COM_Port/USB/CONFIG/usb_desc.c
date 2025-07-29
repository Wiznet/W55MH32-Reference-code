

#include "usb_lib.h"
#include "usb_desc.h"

/* USB Standard Device Descriptor */
const uint8_t Virtual_Com_Port_DeviceDescriptor[] =
    {
        0x12,                       /* bLength */
        USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
        0x00,
        0x02,                       /* bcdUSB = 2.00 */
        0xEF,                       /* bDeviceClass: CDC */
        0x02,                       /* bDeviceSubClass */
        0x01,                       /* bDeviceProtocol */
        0x40,                       /* bMaxPacketSize0 */
        0x83,
        0x04,                       /* idVendor = 0x0483 */
        0x40,
        0x57,                       /* idProduct = 0x7540 */
        0x00,
        0x02,                       /* bcdDevice = 2.00 */
        1,                          /* Index of string descriptor describing manufacturer */
        2,                          /* Index of string descriptor describing product */
        3,                          /* Index of string descriptor describing the device's serial number */
        0x01                        /* bNumConfigurations */
};

const uint8_t Virtual_Com_Port_ConfigDescriptor[VIRTUAL_COM_PORT_SIZ_CONFIG_DESC] =
    {
        /*Configuration Descriptor*/
        0x09,                                    /* bLength: Configuration Descriptor size */
        USB_CONFIGURATION_DESCRIPTOR_TYPE,       /* bDescriptorType: Configuration */
        VIRTUAL_COM_PORT_SIZ_CONFIG_DESC & 0xFF, /* wTotalLength:no of returned bytes */
        (VIRTUAL_COM_PORT_SIZ_CONFIG_DESC >> 8) & 0xFF,
        14,                                      /* bNumInterfaces: 2 interface */
        0x01,                                    /* bConfigurationValue: Configuration value */
        0x00,                                    /* iConfiguration: Index of string descriptor describing the configuration */
        0xC0,                                    /* bmAttributes: self powered */
        0x32,                                    /* MaxPower 0 mA */

        /*******************************COM 1***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        0, /*bFirstInterface*/                    //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01,                                     /*bFunctionProtocol*/
        0x02,                                     /*iFunction*/
        /*72*/
        0x09,                                                           /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */ /* Interface descriptor type */
        0,                                                              /* bInterfaceNumber: Number of Interface */
        0x00,                                                           /* bAlternateSetting: Alternate setting */
        0x00,                                                           /* bNumEndpoints: 0 endpoints used */
        0x02,                                                           /* bInterfaceClass: Communication Interface Class */
        0x02,                                                           /* bInterfaceSubClass: Abstract Control Model */
        0x01,                                                           /* bInterfaceProtocol: Common AT commands */
        0x00,                                                           /* iInterface: */
        /*81*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*85*/
        /*Union Functional Descriptor*/
        0x05,                          /* bFunctionLength */
        0x24,                          /* bDescriptorType: CS_INTERFACE */
        0x06,                          /* bDescriptorSubtype: Union func desc */
        0,                             /* bMasterInterface: Communication class interface */
        1,                             /* bSlaveInterface0: Data Class Interface */
                                       /*90*/
                                       /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        1,                             /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
                                       /*99*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x81,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x20,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
                                      /*106*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x01,                         /* bEndpointAddress: (OUT1) */
        0x02,                         /* bmAttributes: Bulk */
        0x20,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */

        /*******************************COM 2***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        2, /*bFirstInterface*/                    //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01,                                     /*bFunctionProtocol*/
        0x02,                                     /*iFunction*/
        /*72*/
        0x09,                                                           /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */ /* Interface descriptor type */
        2,                                                              /* bInterfaceNumber: Number of Interface */
        0x00,                                                           /* bAlternateSetting: Alternate setting */
        0x00,                                                           /* bNumEndpoints: 0 endpoints used */
        0x02,                                                           /* bInterfaceClass: Communication Interface Class */
        0x02,                                                           /* bInterfaceSubClass: Abstract Control Model */
        0x01,                                                           /* bInterfaceProtocol: Common AT commands */
        0x00,                                                           /* iInterface: */
        /*81*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*85*/
        /*Union Functional Descriptor*/
        0x05,                          /* bFunctionLength */
        0x24,                          /* bDescriptorType: CS_INTERFACE */
        0x06,                          /* bDescriptorSubtype: Union func desc */
        2,                             /* bMasterInterface: Communication class interface */
        3,                             /* bSlaveInterface0: Data Class Interface */
                                       /*90*/
                                       /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        3,                             /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
                                       /*99*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x82,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x20,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
                                      /*106*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x02,                         /* bEndpointAddress: (OUT1) */
        0x02,                         /* bmAttributes: Bulk */
        0x20,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */

        /*******************************COM 3***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        4, /*bFirstInterface*/                    //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01,                                     /*bFunctionProtocol*/
        0x02,                                     /*iFunction*/
        /*72*/
        0x09,                                                           /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */ /* Interface descriptor type */
        4,                                                              /* bInterfaceNumber: Number of Interface */
        0x00,                                                           /* bAlternateSetting: Alternate setting */
        0x00,                                                           /* bNumEndpoints: 0 endpoints used */
        0x02,                                                           /* bInterfaceClass: Communication Interface Class */
        0x02,                                                           /* bInterfaceSubClass: Abstract Control Model */
        0x01,                                                           /* bInterfaceProtocol: Common AT commands */
        0x00,                                                           /* iInterface: */
        /*81*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*85*/
        /*Union Functional Descriptor*/
        0x05,                          /* bFunctionLength */
        0x24,                          /* bDescriptorType: CS_INTERFACE */
        0x06,                          /* bDescriptorSubtype: Union func desc */
        4,                             /* bMasterInterface: Communication class interface */
        5,                             /* bSlaveInterface0: Data Class Interface */
                                       /*90*/
                                       /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        5,                             /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
                                       /*99*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x83,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x20,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
                                      /*106*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x03,                         /* bEndpointAddress: (OUT1) */
        0x02,                         /* bmAttributes: Bulk */
        0x20,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */

        /*******************************COM 4***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        6, /*bFirstInterface*/                    //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01, /*bFunctionProtocol*/               //
        0x02,                                     /*iFunction*/
        /*72*/
        0x09,                                                           /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */ /* Interface descriptor type */
        6,                                                              /* bInterfaceNumber: Number of Interface */
        0x00,                                                           /* bAlternateSetting: Alternate setting */
        0x00,                                                           /* bNumEndpoints: 0 endpoints used */
        0x02,                                                           /* bInterfaceClass: Communication Interface Class */
        0x02,                                                           /* bInterfaceSubClass: Abstract Control Model */
        0x01,                                                           /* bInterfaceProtocol: Common AT commands */
        0x00,                                                           /* iInterface: */
        /*81*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*85*/
        /*Union Functional Descriptor*/
        0x05,                          /* bFunctionLength */
        0x24,                          /* bDescriptorType: CS_INTERFACE */
        0x06,                          /* bDescriptorSubtype: Union func desc */
        6,                             /* bMasterInterface: Communication class interface */
        7,                             /* bSlaveInterface0: Data Class Interface */
                                       /*90*/
                                       /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        7,                             /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
                                       /*99*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x84,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
                                      /*106*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x04,                         /* bEndpointAddress: (OUT1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */

        /*******************************COM 5***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        8, /*bFirstInterface*/                    //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01, /*bFunctionProtocol*/               //
        0x02,                                     /*iFunction*/
        /*72*/
        0x09,                                                           /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */ /* Interface descriptor type */
        8,                                                              /* bInterfaceNumber: Number of Interface */
        0x00,                                                           /* bAlternateSetting: Alternate setting */
        0x00,                                                           /* bNumEndpoints: 0 endpoints used */
        0x02,                                                           /* bInterfaceClass: Communication Interface Class */
        0x02,                                                           /* bInterfaceSubClass: Abstract Control Model */
        0x01,                                                           /* bInterfaceProtocol: Common AT commands */
        0x00,                                                           /* iInterface: */
        /*81*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*85*/
        /*Union Functional Descriptor*/
        0x05,                          /* bFunctionLength */
        0x24,                          /* bDescriptorType: CS_INTERFACE */
        0x06,                          /* bDescriptorSubtype: Union func desc */
        8,                             /* bMasterInterface: Communication class interface */
        9,                             /* bSlaveInterface0: Data Class Interface */
                                       /*90*/
                                       /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        9,                             /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
                                       /*99*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x85,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
                                      /*106*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x05,                         /* bEndpointAddress: (OUT1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */

        /*******************************COM 6***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        10, /*bFirstInterface*/                   //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01, /*bFunctionProtocol*/               //
        0x02,                                     /*iFunction*/
        /*72*/
        0x09,                                                           /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */ /* Interface descriptor type */
        10,                                                             /* bInterfaceNumber: Number of Interface */
        0x00,                                                           /* bAlternateSetting: Alternate setting */
        0x00,                                                           /* bNumEndpoints: 0 endpoints used */
        0x02,                                                           /* bInterfaceClass: Communication Interface Class */
        0x02,                                                           /* bInterfaceSubClass: Abstract Control Model */
        0x01,                                                           /* bInterfaceProtocol: Common AT commands */
        0x00,                                                           /* iInterface: */
        /*81*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*85*/
        /*Union Functional Descriptor*/
        0x05,                          /* bFunctionLength */
        0x24,                          /* bDescriptorType: CS_INTERFACE */
        0x06,                          /* bDescriptorSubtype: Union func desc */
        10,                            /* bMasterInterface: Communication class interface */
        11,                            /* bSlaveInterface0: Data Class Interface */
                                       /*90*/
                                       /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        11,                            /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
                                       /*99*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x86,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
                                      /*106*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x06,                         /* bEndpointAddress: (OUT1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */

        /*******************************COM 7***************************************/
        /*Interface Descriptor*/
        /*IAD  Interface Association Descriptor*/ //IAD descriptor needs to be added
        0x08,                                     /*bLength: Interface Descriptor size*/
        0x0B,                                     /*bDescriptorType: IAD*/
        12, /*bFirstInterface*/                   //The serial number of the first control interface, control interface 0
        0x02, /*bInterfaceCount*/                 //Number of interfaces for this IDA, default 2
        0x02, /*bFunctionClass: CDC*/             //Indicates that the IAD is a CDC device
        0x02, /*bFunctionSubClass*/               //default
        0x01, /*bFunctionProtocol*/               //
        0x02,                                     /*iFunction*/
        /*72*/
        0x09,                                                           /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */ /* Interface descriptor type */
        12,                                                             /* bInterfaceNumber: Number of Interface */
        0x00,                                                           /* bAlternateSetting: Alternate setting */
        0x00,                                                           /* bNumEndpoints: 0 endpoints used */
        0x02,                                                           /* bInterfaceClass: Communication Interface Class */
        0x02,                                                           /* bInterfaceSubClass: Abstract Control Model */
        0x01,                                                           /* bInterfaceProtocol: Common AT commands */
        0x00,                                                           /* iInterface: */
        /*81*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*85*/
        /*Union Functional Descriptor*/
        0x05,                          /* bFunctionLength */
        0x24,                          /* bDescriptorType: CS_INTERFACE */
        0x06,                          /* bDescriptorSubtype: Union func desc */
        12,                            /* bMasterInterface: Communication class interface */
        13,                            /* bSlaveInterface0: Data Class Interface */
                                       /*90*/
                                       /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        13,                            /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
                                       /*99*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x87,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
                                      /*106*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x07,                         /* bEndpointAddress: (OUT1) */
        0x02,                         /* bmAttributes: Bulk */
        0x10,                         /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */
};

/* USB String Descriptors */
const uint8_t Virtual_Com_Port_StringLangID[VIRTUAL_COM_PORT_SIZ_STRING_LANGID] =
    {
        VIRTUAL_COM_PORT_SIZ_STRING_LANGID,
        USB_STRING_DESCRIPTOR_TYPE,
        0x09,
        0x04 /* LangID = 0x0409: U.S. English */
};

const uint8_t Virtual_Com_Port_StringVendor[VIRTUAL_COM_PORT_SIZ_STRING_VENDOR] =
    {
        VIRTUAL_COM_PORT_SIZ_STRING_VENDOR, /* Size of Vendor string */
        USB_STRING_DESCRIPTOR_TYPE,         /* bDescriptorType*/
        'W', 0, 'I', 0, 'Z', 0, 'n', 0, 'e', 0, 't', 0,
        ' ', 0, 'V', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
        'c', 0, 's', 0};

const uint8_t Virtual_Com_Port_StringProduct[VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT] =
    {
        VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT, /* bLength */
        USB_STRING_DESCRIPTOR_TYPE,          /* bDescriptorType */
        'W', 0, 'I', 0, 'Z', 0, 'n', 0, 'e', 0, 't', 0,
        ' ', 0, 'V', 0, 'c', 0, 't', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0,
        'M', 0, ' ', 0, 'P', 0, 'o', 0, 'r', 0, 't', 0, ' ', 0, ' ', 0};

uint8_t Virtual_Com_Port_StringSerial[VIRTUAL_COM_PORT_SIZ_STRING_SERIAL] =
    {
        VIRTUAL_COM_PORT_SIZ_STRING_SERIAL, /* bLength */
        USB_STRING_DESCRIPTOR_TYPE,         /* bDescriptorType */
        'W', 0, '5', 0, '5', 0, 'M', 0, 'H', 0, ' ', 0, '3', 0, '2', 0};
