/*
 * Modified by Tuoqiang
 * Email:tuoqiang@outlook.com
*/
/*
 * File: usb.c
 * Date: 07.02.2014
 * Denis Zheleznyakov aka ZiB @ http://ziblog.ru
 */
/*************************************************************************/
#include "usb.h"
#include "hw.h"
/*************************************************************************/

#define ARRAY_LENGHT(Value)     (sizeof(Value) / sizeof(Value[0]))
/*************************************************************************/
extern void USB_Tx(void);
/*************************************************************************/
uint16_t usb_ready,usb_ready_reg;
uint16_t usb_tx_count,usb_rx_count;
uint8_t usb_rx_buffer[48];
uint8_t *usb_tx_buffer_ptr;
/*************************************************************************/
enum USB_Events
{
    USB_EVENT_NO = 0, //
    USB_EVENT_RECEIVE_SETUP_DATA = 1, // приняты данные токена SETUP
    USB_EVENT_READY_DATA_IN = 2, // готовы данные для передачи в ПК
    USB_EVENT_WAIT_DATA_IN = 3, // ожидание готовности данных для передачи в ПК
};

enum USB_States
{
    USB_STATE_IDLE = 0, //
    USB_STATE_SETUP = 1,
    USB_STATE_IN = 2,
    USB_STATE_OUT = 3,
    USB_STATE_DATA = 4,
    USB_STATE_ADDRESS_CHANGE = 5
};

enum USB_Packet_ID
{
    USB_PID_SETUP = 0x2D,
    USB_PID_DATA0 = 0xC3,
    USB_PID_DATA1 = 0x4B,
    USB_PID_IN = 0x69,
    USB_PID_OUT = 0xE1,
    USB_PID_ACK = 0xD2,
    USB_PID_NACK = 0x5A,
    USB_PID_STALL = 0x1E
};

enum USB_Request_Type
{
    USBRQ_STD_TO_DEVICE = 0x00,
    USBRQ_STD_TO_INTERFACE = 0x01,
    USBRQ_STD_TO_ENDPOINT = 0x02,
    USBRQ_STD_FROM_DEVICE = 0x80,
    USBRQ_STD_FROM_INTERFACE = 0x81,
    USBRQ_STD_FROM_ENDPOINT = 0x82,
    
    USBRQ_CLASS_TO_DEVICE = 0x20,
    USBRQ_CLASS_TO_INTERFACE = 0x21,
    USBRQ_CLASS_TO_ENDPOINT = 0x22,
    USBRQ_CLASS_FROM_DEVICE = 0xa0,
    USBRQ_CLASS_FROM_INTERFACE = 0xa1,
    USBRQ_CLASS_FROM_ENDPOINT = 0xa2,
      
    USBRQ_VENDOR_TO_DEVICE = 0x40,
    USBRQ_VENDOR_TO_INTERFACE = 0x41,
    USBRQ_VENDOR_TO_ENDPOINT = 0x42,
    USBRQ_VENDOR_FROM_DEVICE = 0xc0,
    USBRQ_VENDOR_FROM_INTERFACE = 0xc1,
    USBRQ_VENDOR_FROM_ENDPOINT = 0xc2,
    
    USBRQ_CLASS=0x20,
    USBRQ_VENDOR=0x40
};

// USB Standard Requests 
enum USB_Request
{
    USBRQ_GET_STATUS = 0x00,
    USBRQ_CLEAR_FEATURE=0x01,
    USBRQ_SET_FEATURE=0x03,
    USBRQ_SET_ADDRESS = 0x05,
    USBRQ_GET_DESCRIPTOR = 0x06,
    USBRQ_SET_DESCRIPTOR = 0x07,
    USBRQ_GET_CONFIGURATION = 0x08,
    USBRQ_SET_CONFIGURATION = 0x09,
    USBRQ_GET_INTERFACE=0x0a,
    USBRQ_SET_INTERFACE=0x0b,
    USBRQ_SYNCH_FRAME=0x0c
};

// USB descriptor constants
enum Descriptor_Type{
    USBDESCR_DEVICE=0x01,
    USBDESCR_CONFIG=0x02,
    USBDESCR_STRING=0x03,
    USBDESCR_INTERFACE=0x04,
    USBDESCR_ENDPOINT=0x05,
    USBDESCR_HID=0x21,
    USBDESCR_HID_REPORT=0x22,
    USBDESCR_HID_PHYS=0x23
};

enum USB_Power{
    USBATTR_BUSPOWER=0x80,  // USB 1.1 does not define this value any more
    USBATTR_SELFPOWER=0x40,
    USBATTR_REMOTEWAKE=0x20,
};
#define USB_CFG_MAX_BUS_POWER 100
/***************************************************************************/
enum USB_PID_VID
{
    USB_VENDOR_ID_LOW = 0xc0, //
    USB_VENDOR_ID_HIGH = 0x16,
    USB_DEVICE_ID_LOW = 0xdc, //
    USB_DEVICE_ID_HIGH = 0x05,
    USB_DEVICE_VERSION_LOW = 0x01, //
    USB_DEVICE_VERSION_HIGH = 0x01
};
const uint8_t usb_device_descriptor[] = { 
    18, // Size of the Descriptor in Bytes
    USBDESCR_DEVICE, // Device Descriptor (0x01)
    0x10, 
    0x01, // USB 1.1 = 0x0110， USB 1.0 = 0x0100
    00,//0x00, // Class Code
    0x00, // Subclass Code
    0x00, // Protocol Code
    0x08, // Maximum Packet Size for Zero Endpoint
    USB_VENDOR_ID_LOW, USB_VENDOR_ID_HIGH, // VID
    USB_DEVICE_ID_LOW, USB_DEVICE_ID_HIGH,// PID
    USB_DEVICE_VERSION_LOW, USB_DEVICE_VERSION_HIGH, // Device Release Number
    0x00, // Index of Manufacturer String Descriptor
    0x00, // Index of Product String Descriptor
    0x00, // Index of Serial Number String Descriptor
    0x01, // Number of Possible Configurations
};
/*
const uint8_t usb_configuration_descriptor[] = {   // USB configuration descriptor 
    9,          // sizeof(usbDescrConfig): length of descriptor in bytes 
    USBDESCR_CONFIG,    // descriptor type 
    67,
    0,          // total length of data returned (including inlined descriptors) 
    2,          // number of interfaces in this configuration 
    1,          // index of this configuration
    0,          // configuration name string index
    (1 << 7),                           // attributes
    USB_CFG_MAX_BUS_POWER/2,            // max USB current in 2mA units

    // interface descriptor follows inline:
    9,          // sizeof(usbDescrInterface): length of descriptor in bytes
    USBDESCR_INTERFACE, // descriptor type
    0,          // index of this interface
    0,          // alternate setting for this interface
    1,//USB_CFG_HAVE_INTRIN_ENDPOINT,   // endpoints excl 0: number of endpoint descriptors to follow
    2,//USB_CFG_INTERFACE_CLASS,
    2,//USB_CFG_INTERFACE_SUBCLASS,
    1,//USB_CFG_INTERFACE_PROTOCOL,
    0,          // string index for interface

    // CDC Class-Specific descriptor
    5,           // sizeof(usbDescrCDC_HeaderFn): length of descriptor in bytes
    0x24,        // descriptor type
    0,           // header functional descriptor
    0x10, 0x01,

    4,           // sizeof(usbDescrCDC_AcmFn): length of descriptor in bytes
    0x24,        // descriptor type
    2,           // abstract control management functional descriptor
    0x02,        // SET_LINE_CODING,    GET_LINE_CODING, SET_CONTROL_LINE_STATE

    5,           // sizeof(usbDescrCDC_UnionFn): length of descriptor in bytes
    0x24,        // descriptor type
    6,           // union functional descriptor
    0,           // CDC_COMM_INTF_ID
    1,           // CDC_DATA_INTF_ID

    5,           // sizeof(usbDescrCDC_CallMgtFn): length of descriptor in bytes
    0x24,        // descriptor type
    1,           // call management functional descriptor
    3,           // allow management on data interface, handles call management by itself
    1,           // CDC_DATA_INTF_ID

    // Endpoint Descriptor
    7,           // sizeof(usbDescrEndpoint)
    USBDESCR_ENDPOINT,  // descriptor type = endpoint
    0x83,        // IN endpoint number 3
    0x03,        // attrib: Interrupt endpoint
    8, 0,        // maximum packet size
    64,//USB_CFG_INTR_POLL_INTERVAL,        // in ms

    // Interface Descriptor
    9,           // sizeof(usbDescrInterface): length of descriptor in bytes
    USBDESCR_INTERFACE,           // descriptor type
    1,           // index of this interface
    0,           // alternate setting for this interface
    2,           // endpoints excl 0: number of endpoint descriptors to follow
    0x0A,        // Data Interface Class Codes
    0,
    0,           // Data Interface Class Protocol Codes
    0,           // string index for interface

    // Endpoint Descriptor
    7,           // sizeof(usbDescrEndpoint)
    USBDESCR_ENDPOINT,  // descriptor type = endpoint
    0x01,        // OUT endpoint number 1
    0x02,        // attrib: Bulk endpoint
    
    8,//HW_CDC_BULK_OUT_SIZE,
    0,        // maximum packet size
    
    0,           // in ms

    // Endpoint Descriptor
    7,           // sizeof(usbDescrEndpoint)
    USBDESCR_ENDPOINT,  // descriptor type = endpoint
    0x81,        // IN endpoint number 1
    0x02,        // attrib: Bulk endpoint
    
    8,//HW_CDC_BULK_IN_SIZE, 
    0,        // maximum packet size
    
    0,           // in ms
};
*/

const uint8_t usb_configuration_descriptor[] = { 
    9, // Size of Descriptor in Bytes
    USBDESCR_CONFIG, // Configuration Descriptor (0x02)
    34,
    0x00, // Total length in bytes of data returned
    0x01, // Number of Interfaces
    0x01, // Value to use as an argument to select this configuration
    0x00, // Index of String Descriptor describing this configuration
    USBATTR_BUSPOWER, // D7 Reserved, set to 1. (USB 1.0 Bus Powered), D6 Self Powered, D5 Remote Wakeup, D4..0 Reserved, set to 0.
    USB_CFG_MAX_BUS_POWER/2,   // Maximum Power Consumption in 2mA units

    // interface descriptor
    9, // Size of Descriptor in Bytes (9 Bytes)
    USBDESCR_INTERFACE, // Interface Descriptor (0x04)
    0x00, // Number of Interface
    0x00, // Value used to select alternative setting
    0x01, // Number of Endpoints used for this interface
    0x03, // Class Code
    0x01, // Subclass Code
    0x02, // Protocol Code
    0x00, // Index of String Descriptor Describing this interface

    // HID descriptor
    9,    // Size of Descriptor in Bytes (9 Bytes)
    USBDESCR_HID, // HID descriptor (0x21)
    0x10, 
    0x01, // BCD representation of HID version
    0x21, // Target country code
    0x01, // Number of HID Report (or other HID class) Descriptor infos to follow
    0x22, // Descriptor type: report
    0x34,
    0,  // total length of report descriptor

    // Endpoint descriptor
    7, // Size of Descriptor in Bytes (7 Bytes)
    USBDESCR_ENDPOINT, // Endpoint descriptor (0x05)
    0x81, // IN endpoint number 1 (0x81)
    0x03, // attrib: Interrupt endpoint
    0x04, // 
    0x00, // maximum packet size
    0x0A  // POLL INTERVAL (ms)
};

const uint8_t usb_report_descriptor[] = {
    0x05,
    0x01, // USAGE_PAGE (Generic Desktop)
    0x09,
    0x02, // USAGE (Mouse)
    0xa1,
    0x01, // COLLECTION (Application)
    0x09,
    0x01, // USAGE (Pointer)
    0xa1,
    0x00, // COLLECTION (Physical)
    0x05,
    0x09, // USAGE_PAGE (Button)
    0x19,
    0x01, // USAGE_MINIMUM (Button 1)
    0x29,
    0x03, // USAGE_MAXIMUM (Button 3)
    0x15,
    0x00, // LOGICAL_MINIMUM (0)
    0x25,
    0x01, // LOGICAL_MAXIMUM (1)
    0x95,
    0x03, // REPORT_COUNT (3)
    0x75,
    0x01, // REPORT_SIZE (1)
    0x81,
    0x02, // INPUT (Data,Var,Abs)
    0x95,
    0x01, // REPORT_COUNT (1)
    0x75,
    0x05, // REPORT_SIZE (5)
    0x81,
    0x03, // INPUT (Cnst,Var,Abs)
    0x05, 
    0x01, // USAGE_PAGE (Generic Desktop)
    0x09,
    0x30, // USAGE (X)
    0x09, 
    0x31, // USAGE (Y)
    0x09, 
    0x38, // USAGE (Wheel)
    0x15,
    0x81, // LOGICAL_MINIMUM (-127)
    0x25,
    0x7f, // LOGICAL_MAXIMUM (127)
    0x75, 
    0x08, // REPORT_SIZE (8)
    0x95,
    0x03, // REPORT_COUNT (3)
    0x81, 
    0x06, // INPUT (Data,Var,Rel)
    0xc0, // END_COLLECTION
    0xc0  // END_COLLECTION
};
/***************************************************************************/
const uint8_t usb_string_descriptor_language[] = {
    4,
    USBDESCR_STRING,
    0x09,
    0x04
};
const uint8_t usb_string_descriptor_vendor[] = {
    42,
    USBDESCR_STRING,
    't',0,'u',0,'o',0,'q',0,'i',0,'a',0,'n',0,'g',0,
    '@',0,
    'o',0,'u',0,'t',0,'l',0,'o',0,'o',0,'k',0,
    '.',0,'c',0,'o',0,'m',0
};
//I don't why the device string is not recognized when it is shorter than 0x12,for example,0x10.
//So I add a '\0'
const uint8_t usb_string_descriptor_device[] = {
    18,
    USBDESCR_STRING,
    'U',0,'S',0,'B',0,'-',0,'2',0,'3',0,'2',0,'\0',0
};


const uint8_t usb_string_descriptor_serial[] = {
    8,
    USBDESCR_STRING,
    '1',0,'0',0,'0',0,
};
/***************************************************************************/
struct USB_Type
{
    volatile uint8_t state;
    volatile uint8_t event;
    volatile uint8_t device_address;

    uint8_t endpoint;
    uint8_t setup_address;
    uint8_t setup_endpoint;

    uint8_t rx_buffer[16];
    uint8_t rx_lenght;

    uint8_t tx_buffer[16];
    uint8_t tx_lenght;
}usb;
/***************************************************************************/
void USB_Init(void)
{
    usb.state = USB_STATE_IDLE;
    usb.event = USB_EVENT_NO;
    usb.device_address = 0;
    usb.setup_address  = 0;
    usb_ready = 0;
    usb_ready_reg = 0;
    usb.endpoint=0;
    
    VUSB_GPIO_Init();
    VUSB_Clock_Init();
    VUSB_EXTI_Init();
}
/*************************************************************************/
void USB_Send_NACK(void)
{
    uint8_t data[2];
    data[0] = 0x80;
    data[1] = USB_PID_NACK;
    usb_tx_count = 2;
    usb_tx_buffer_ptr = data;
    USB_Tx();        
}

void USB_Send_ACK(void)
{
    uint8_t data[2];
    data[0] = 0x80;
    data[1] = USB_PID_ACK;
    usb_tx_count = 2;
    usb_tx_buffer_ptr = data;       
    USB_Tx();
}

void USB_Send_Answer(void)
{
    usb_tx_count = usb.tx_lenght;
    usb_tx_buffer_ptr = &usb.tx_buffer[0];
    USB_Tx();
}
/*************************************************************************/
void USB_Copy_Rx_Buffer(void)
{
    uint8_t index = 0;
    for (index = 0; index < 16; index++) usb.rx_buffer[index] = usb_rx_buffer[index];
}
/*************************************************************************/
void USB_Rx_Response(void)
{
    switch (usb_rx_buffer[1])
    {
        case (USB_PID_SETUP):
            if((usb_rx_buffer[2]&0x7F)==usb.device_address){
                usb.state = USB_STATE_SETUP;
                usb.endpoint=usb_rx_buffer[2]>>7+(usb_rx_buffer[3]&0x07)<<1;
            }
            break;
        case (USB_PID_OUT):
            if((usb_rx_buffer[2]&0x7F)==usb.device_address){
                usb.state = USB_STATE_OUT;
                usb.endpoint=usb_rx_buffer[2]>>7+(usb_rx_buffer[3]&0x07)<<1;
            }
            break;
        case (USB_PID_IN):
            if((usb_rx_buffer[2]&0x7F)==usb.device_address) // It's our address
            {
                if(usb.setup_address!=0)	//需要找个更好的地方
                {
                    usb.device_address = usb.setup_address;
                }
                else{
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                }
                if(usb_ready_reg==1){
                    usb_ready=1;
                }
                else{
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                    nop();
                }
                if (usb.event != USB_EVENT_READY_DATA_IN)
                {
                    //ooooooooooooooo
                    nop();
                    USB_Send_NACK();
                    
                }
                else{
                    //ooooooooooooo
                    nop();
                    nop();
                    USB_Send_Answer();
                    usb.state = USB_STATE_IN;
                    usb.event = USB_EVENT_WAIT_DATA_IN;
                }
                usb.endpoint=usb_rx_buffer[2]>>7+(usb_rx_buffer[3]&0x07)<<1;
            }
            break;
        case (USB_PID_DATA0):
            if (usb.state == USB_STATE_SETUP)
            {
                //ooooooooooo
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                
                nop();
                
                USB_Send_ACK();
                USB_Copy_Rx_Buffer();
                usb.event = USB_EVENT_RECEIVE_SETUP_DATA;
            }
            else if (usb.state == USB_STATE_OUT)
            {
                //can't be tested
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
		
                USB_Send_ACK();
                usb.event == USB_EVENT_NO;
            }
            break;
        case (USB_PID_DATA1):
            if (usb.state == USB_STATE_OUT)
            {
                //ooooooooooooooooo
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
		
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
		
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
                nop();
		
                nop();
                nop();
                nop();
                nop();
                nop();
                
                USB_Send_ACK();
                usb.event == USB_EVENT_NO;
            }
            break;
        case (USB_PID_ACK):break;
        case (USB_PID_NACK):break;
        default:
            usb.state = USB_STATE_IDLE;
            break;
    }
}
/*************************************************************************/
void USB_Calc_CRC16(uint8_t * buffer, uint8_t lenght)
{
    uint16_t crc = 0xFFFF;
    uint8_t index;
    uint8_t i;
    for(index = 0; index < lenght; index++)
    {
        crc ^= *buffer++;
        for (i = 8; i--;)
        {
            if ((crc & 0x01) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else crc >>= 1;
        }
    }
    crc = ~crc;
    *buffer++ = (uint8_t) crc;
    *buffer = (uint8_t) (crc >> 8);
}

void USB_Send_Data(const uint8_t * buffer, uint8_t lenght, uint8_t mode)
{
    uint8_t index;
    static uint16_t data_sync;
    
    if(mode) data_sync = USB_PID_DATA1;
    while (lenght > 0)
    {
        usb.tx_buffer[0] = 0x80;
        usb.tx_buffer[1] = data_sync;
        if (lenght >= 8)
        {
            usb.tx_lenght = 12;
            for (index = 2; index < 10; index++) usb.tx_buffer[index] = *buffer++;
            lenght -= 8;
        }
        else
        {
            usb.tx_lenght = (uint8_t) (4 + lenght);
            for (index = 2; index < 2 + lenght; index++) usb.tx_buffer[index] = *buffer++;
            lenght = 0;
        }
        // calculate CRC
        USB_Calc_CRC16(&usb.tx_buffer[2], (uint8_t) (usb.tx_lenght - 4));
        // toggle data0 data1
        if (data_sync == USB_PID_DATA1) data_sync = USB_PID_DATA0;
        else data_sync = USB_PID_DATA1;
        // data is available to send out 
        usb.event = USB_EVENT_READY_DATA_IN;
        // wait for transmission and then start the next
        while (usb.event == USB_EVENT_READY_DATA_IN)
        {
            //if ((usb.state != USB_STATE_IN)&&(usb.state != USB_STATE_SETUP))
            if (usb.event == USB_EVENT_WAIT_DATA_IN) break;
        }
    }
}

void USB_Send_Null_Data1(void)
{
    usb.tx_lenght = 4;
    usb.tx_buffer[0] = 0x80;
    usb.tx_buffer[1] = USB_PID_DATA1;
    usb.tx_buffer[2] = 0;
    usb.tx_buffer[3] = 0;
    usb.event = USB_EVENT_READY_DATA_IN;
}

void USB_Send_Stall(void)
{
    usb.tx_lenght = 2;
    usb.tx_buffer[0] = 0x80;
    usb.tx_buffer[1] = USB_PID_STALL;
    usb.event = USB_EVENT_READY_DATA_IN;
}

void EP0_Process(void){
    if (usb.event != USB_EVENT_RECEIVE_SETUP_DATA) return;
    if(usb.rx_buffer[2]==USBRQ_STD_FROM_DEVICE)
    {
        if(usb.rx_buffer[3]==USBRQ_GET_DESCRIPTOR) //0x06
        {
            switch (usb.rx_buffer[5])
            {
            case USBDESCR_DEVICE:	// device descriptor
                USB_Send_Data(usb_device_descriptor, ARRAY_LENGHT(usb_device_descriptor), 1);
                break;
            case USBDESCR_CONFIG:	// configuration descriptor
                if(usb.rx_buffer[8]<ARRAY_LENGHT(usb_configuration_descriptor)) USB_Send_Data(usb_configuration_descriptor,usb.rx_buffer[8], 1);
                else USB_Send_Data(usb_configuration_descriptor, ARRAY_LENGHT(usb_configuration_descriptor), 1);
                break;
            case USBDESCR_STRING:	// string descriptor
                if(usb.rx_buffer[4]==0) USB_Send_Data(usb_string_descriptor_language, ARRAY_LENGHT(usb_string_descriptor_language),1);
                else if(usb.rx_buffer[4]==1) USB_Send_Data(usb_string_descriptor_vendor, ARRAY_LENGHT(usb_string_descriptor_vendor),1);
                else if(usb.rx_buffer[4]==2) USB_Send_Data(usb_string_descriptor_device, ARRAY_LENGHT(usb_string_descriptor_device),1);
                else if(usb.rx_buffer[4]==3) USB_Send_Data(usb_string_descriptor_serial, ARRAY_LENGHT(usb_string_descriptor_serial),1);
                break;
            default:
                break;
            }
        }
        else if(usb.rx_buffer[3]==USBRQ_GET_STATUS){
            uint8_t data[2];
            //USB_Send_Stall();
            //__asm("cpsid i");
            //while(1);
        }
    }
    else if (usb.rx_buffer[2]==USBRQ_STD_TO_DEVICE)
    {
        switch (usb.rx_buffer[3])
        {
        case (USBRQ_SET_ADDRESS):	//0x05
            usb.setup_address = usb.rx_buffer[4];
            USB_Send_Null_Data1();
            break;
        case (USBRQ_SET_CONFIGURATION):	//0x09
            USB_Send_Null_Data1();
            break;
        }
    }
    else if (usb.rx_buffer[2]==USBRQ_STD_FROM_INTERFACE)
    {
        if (usb.rx_buffer[3]==USBRQ_GET_DESCRIPTOR)
        {
            USB_Send_Data(usb_report_descriptor, ARRAY_LENGHT(usb_report_descriptor), 1);
            usb_ready_reg=1;
        }
    }
    else if (usb.rx_buffer[2]==USBRQ_CLASS_TO_INTERFACE){
        USB_Send_Stall();
    }
}


void USB_Process(void)
{
    if(usb.endpoint==0) EP0_Process();
    else if(usb.endpoint==1){
    }
    else if(usb.endpoint==2){
    }
    else if(usb.endpoint==3){
    }
}


void USB_Process_LOOP(void){
    //while(usb_ready == 0)
    while(1)    USB_Process();
}
/****************************************************************************/
void EP1_Rx(void){}
void EP1_Tx(void){}
void EP2_Rx(void){}
void EP2_Tx(void){}
void EP3_Rx(void){}
void EP3_Tx(void){}