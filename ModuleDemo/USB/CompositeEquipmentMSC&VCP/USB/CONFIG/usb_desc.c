/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  none
  * @version V4.1.0
  * @date    17-Oct-2024
  * @brief   Descriptors for Mass Storage Device
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; copyright (c) 2024 WIZnet. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of WIZnet nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for WIZnet.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY WIZnet AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL WIZnet OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_desc.h"

const uint8_t MASS_DeviceDescriptor[MASS_SIZ_DEVICE_DESC] =
    {
        0x12,                       /* bLength  */
        USB_DEVICE_DESCRIPTOR_TYPE, /* bDescriptorType */
        0x00,                       /* bcdUSB, version 2.00 */
        0x02,
        0xEF,                       /* bDeviceClass : each interface define the device class */
        0x02,                       /* bDeviceSubClass */
        0x01,                       /* bDeviceProtocol */
        ENDP0_SIZE,                 /* bMaxPacketSize0 0x40 = 64 */
        0x83,                       /* idVendor     (0483) */
        0x04,
        0x20,                       /* idProduct */
        0x57,
        0x00,                       /* bcdDevice 2.00*/
        0x02,
        1,                          /* index of string Manufacturer  */
        /**/
        2, /* index of string descriptor of product*/
        /* */
        3, /* */
        /* */
        /* */
        0x01 /*bNumConfigurations */
};
const uint8_t MASS_ConfigDescriptor[MASS_SIZ_CONFIG_DESC] =
    {
        0x09,                              /* bLength: Configuration Descriptor size */
        USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
        MASS_SIZ_CONFIG_DESC,

        0x00,
        0x05, /* bNumInterfaces: 1 interface */
        0x01, /* bConfigurationValue: */
        /*      Configuration value */
        0x00, /* iConfiguration: */
        /*      Index of string descriptor */
        /*      describing the configuration */
        0xC0, /* bmAttributes: */
        /*      Self powered */
        0x32, /* MaxPower 100 mA */

        /******************** Descriptor of Mass Storage interface ********************/
        /* 09 */
        0x09,                          /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        /*      Interface descriptor type */
        0x00, /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x02, /* bNumEndpoints*/
        0x08, /* bInterfaceClass: MASS STORAGE Class */
        0x06, /* bInterfaceSubClass : SCSI transparent*/
        0x50, /* nInterfaceProtocol */
        4,    /* iInterface: */
        /* 18 */
        0x07, /*Endpoint descriptor length = 7*/
        0x05, /*Endpoint descriptor type */
        0x81, /*Endpoint address (IN, address 1) */
        0x02, /*Bulk endpoint type */
        0x40, /*Maximum packet size (64 bytes) */
        0x00,
        0x00, /*Polling interval in milliseconds */
        /* 25 */
        0x07, /*Endpoint descriptor length = 7 */
        0x05, /*Endpoint descriptor type */
        0x02, /*Endpoint address (OUT, address 2) */
        0x02, /*Bulk endpoint type */
        0x40, /*Maximum packet size (64 bytes) */
        0x00,
        0x00, /*Polling interval in milliseconds*/
        /*32*/

        /******************** Descriptor of CDC1 Storage interface ********************/
        /*IAD  Interface Association Descriptor*/
        0x08, /*bLength: Interface Descriptor size*/
        0x0B, /*bDescriptorType: IAD*/
        1,    /*bFirstInterface*/
        0x02, /*bInterfaceCount*/
        0x02, /*bFunctionClass: CDC*/
        0x02, /*bFunctionSubClass*/
        0x01, /*bFunctionProtocol*/
        0x02, /*iFunction*/
        /*40*/

        0x09,                          /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */
        /* Interface descriptor type */
        1,    /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x00, /* bNumEndpoints: 0 endpoints used */
        0x02, /* bInterfaceClass: Communication Interface Class */
        0x02, /* bInterfaceSubClass: Abstract Control Model */
        0x01, /* bInterfaceProtocol: Common AT commands */
        0x00, /* iInterface: */
        /*49*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*53*/
        /*Union Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        1,    /* bMasterInterface: Communication class interface */
        2,    /* bSlaveInterface0: Data Class Interface */
        /*58*/
        /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        2,                             /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
        /*67*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x83,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        CDC1_TX_DATA_SIZE,            /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
        /*74*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x03,                         /* bEndpointAddress: (OUT3) */
        0x02,                         /* bmAttributes: Bulk */
        CDC1_RX_DATA_SIZE,            /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */
        /*81*/

        /******************** Descriptor of CDC2 Storage interface ********************/
        /*IAD  Interface Association Descriptor*/
        0x08, /*bLength: Interface Descriptor size*/
        0x0B, /*bDescriptorType: IAD*/
        3,    /*bFirstInterface*/
        0x02, /*bInterfaceCount*/
        0x02, /*bFunctionClass: CDC*/
        0x02, /*bFunctionSubClass*/
        0x01, /*bFunctionProtocol*/
        0x02, /*iFunction*/
        /*89*/

        0x09,                          /* bLength: Interface Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: Interface */
        /* Interface descriptor type */
        3,    /* bInterfaceNumber: Number of Interface */
        0x00, /* bAlternateSetting: Alternate setting */
        0x00, /* bNumEndpoints: 0 endpoints used */
        0x02, /* bInterfaceClass: Communication Interface Class */
        0x02, /* bInterfaceSubClass: Abstract Control Model */
        0x01, /* bInterfaceProtocol: Common AT commands */
        0x00, /* iInterface: */
        /*98*/
        /*ACM Functional Descriptor*/
        0x04, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x02, /* bDescriptorSubtype: Abstract Control Management desc */
        0x02, /* bmCapabilities */
        /*102*/
        /*Union Functional Descriptor*/
        0x05, /* bFunctionLength */
        0x24, /* bDescriptorType: CS_INTERFACE */
        0x06, /* bDescriptorSubtype: Union func desc */
        3,    /* bMasterInterface: Communication class interface */
        4,    /* bSlaveInterface0: Data Class Interface */
        /*107*/
        /*Data class interface descriptor*/
        0x09,                          /* bLength: Endpoint Descriptor size */
        USB_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType: */
        4,                             /* bInterfaceNumber: Number of Interface */
        0x00,                          /* bAlternateSetting: Alternate setting */
        0x02,                          /* bNumEndpoints: Two endpoints used */
        0x0A,                          /* bInterfaceClass: CDC */
        0x00,                          /* bInterfaceSubClass: */
        0x00,                          /* bInterfaceProtocol: */
        0x00,                          /* iInterface: */
        /*116*/
        /*Endpoint 1 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x84,                         /* bEndpointAddress: (IN1) */
        0x02,                         /* bmAttributes: Bulk */
        CDC2_TX_DATA_SIZE,            /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval */
        /*123*/
        /*Endpoint 3 Descriptor*/
        0x07,                         /* bLength: Endpoint Descriptor size */
        USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: Endpoint */
        0x04,                         /* bEndpointAddress: (OUT3) */
        0x02,                         /* bmAttributes: Bulk */
        CDC2_RX_DATA_SIZE,            /* wMaxPacketSize: */
        0x00,
        0x00,                         /* bInterval: ignore for Bulk transfer */
                                      /*130*/
};
const uint8_t MASS_StringLangID[MASS_SIZ_STRING_LANGID] =
    {
        MASS_SIZ_STRING_LANGID,
        USB_STRING_DESCRIPTOR_TYPE,
        0x09,
        0x04}; /* LangID = 0x0409: U.S. English */
const uint8_t MASS_StringVendor[MASS_SIZ_STRING_VENDOR] =
    {
        MASS_SIZ_STRING_VENDOR, /* Size of manufacturer string */
        0x03,                   /* bDescriptorType = String descriptor */
        /* Manufacturer: "WIZnet" */
        'W',
        0,
        'I',
        0,
        'Z',
        0,
        'n',
        0,
        'e',
        0,
        't',
        0,
};
const uint8_t MASS_StringProduct[MASS_SIZ_STRING_PRODUCT] =
    {
        MASS_SIZ_STRING_PRODUCT,
        0x03,
        /* Product name: "w55mh32:USB Mass Storage" */
        'W',
        0,
        'I',
        0,
        'Z',
        0,
        ' ',
        0,
        'C',
        0,
        'o',
        0,
        'm',
        0,
        'p',
        0,
        'o',
        0,
        's',
        0,
        'i',
        0,
        't',
        0,
        'e',
        0,
        ' ',
        0,
};

uint8_t MASS_StringSerial[MASS_SIZ_STRING_SERIAL] =
    {
        MASS_SIZ_STRING_SERIAL,
        0x03,
        /* Serial number*/
        'W', 0, '5', 0, '5', 0, 'M', 0, 'H', 0, '3', 0, '2', 0};
const uint8_t MASS_StringInterface[MASS_SIZ_STRING_INTERFACE] =
    {
        MASS_SIZ_STRING_INTERFACE,
        0x03,
        /* Interface 0: "WIZ Mass" */
        'W', 0, 'I', 0, 'Z', 0, ' ', 0, 'M', 0, 'a', 0, 's', 0, 's', 0};

/************************ (C) COPYRIGHT  2024  WIZnet *****END OF FILE****/
