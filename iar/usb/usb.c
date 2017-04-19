/*
 * File: usb.c
 * 2017.04.19
 * Modified by Ads830e
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
typedef enum{
    FALSE=0,
    TRUE=!FALSE
}Bool;
#define ARRAY_LENGHT(Value)     (sizeof(Value) / sizeof(Value[0]))
/*************************************************************************/
extern void USB_TX(void);
/*************************************************************************/
uint16_t usb_ready,usb_ready_reg,data_sync;
uint16_t USB_TX_COUNT,USB_RX_COUNT;
uint8_t USB_RX_BUFFER[48];
uint8_t *USB_TX_BUFFER_POINTER;
/*************************************************************************/
enum usb_settings
{
    USB_VENDOR_ID_LOW = 0x11, //
    USB_VENDOR_ID_HIGH = 0x22,
    USB_DEVICE_ID_LOW = 0x33, //
    USB_DEVICE_ID_HIGH = 0x44,
    USB_DEVICE_VERSION_LOW = 0x55, //
    USB_DEVICE_VERSION_HIGH = 0x66
};

enum usb_events_list
{
    USB_EVENT_NO = 0, //
    USB_EVENT_RECEIVE_SETUP_DATA = 1, // приняты данные токена SETUP
    USB_EVENT_READY_DATA_IN = 2, // готовы данные для передачи в ПК
    USB_EVENT_WAIT_DATA_IN = 3, // ожидание готовности данных для передачи в ПК
    USB_EVENT
};

enum usb_states_list
{
    USB_STATE_IDLE = 0, //
    USB_STATE_SETUP = 1,
    USB_STATE_IN = 2,
    USB_STATE_OUT = 3,
    USB_STATE_DATA = 4,
    USB_STATE_ADDRESS_CHANGE = 5
};

enum usb_packets_id_list
{
    USB_PID_SETUP = 0x2D, //
    USB_PID_DATA0 = 0xC3,
    USB_PID_DATA1 = 0x4B,
    USB_PID_IN = 0x69, // данные для ПК
    USB_PID_OUT = 0xE1, // данные от ПК
    USB_PID_ACK = 0xD2,
    USB_PID_NACK = 0x5A,
    USB_PID_STALL = 0x1E
};

enum usb_request_standart_type_list
{
    USB_REQUEST_TYPE_TO_DEVICE = 0x00,
    USB_REQUEST_TYPE_TO_INTERFACE = 0x01,
    USB_REQUEST_TYPE_TO_ENDPOINT = 0x02,
    USB_REQUEST_TYPE_FROM_DEVICE = 0x80,
    USB_REQUEST_TYPE_FROM_INTERFACE = 0x81,
    USB_REQUEST_TYPE_FROM_ENDPOINT = 0x82
};

enum usb_request_list
{
    USB_REQUEST_GET_STATUS = 0x00,
    USB_REQUEST_SET_ADDRESS = 0x05,
    USB_REQUEST_GET_DESCRIPTOR = 0x06,
    USB_REQUEST_SET_DESCRIPTOR = 0x07,
    USB_REQUEST_GET_CONFIGURATION = 0x08,
    USB_REQUEST_SET_CONFIGURATION = 0x09
};
/***************************************************************************/
const uint8_t usb_device_descriptor[] = { 
    0x12, // Size of the Descriptor in Bytes
    0x01, // Device Descriptor (0x01)
    0x10, 
    0x01, // USB 1.1 = 0x0110， USB 1.0 = 0x0100
    0x00, // Class Code
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

const uint8_t usb_configuration_descriptor[] = { 
    0x09, // Size of Descriptor in Bytes
    0x02, // Configuration Descriptor (0x02)
    34,
    0x00, // Total length in bytes of data returned
    0x01, // Number of Interfaces
    0x01, // Value to use as an argument to select this configuration
    0x00, // Index of String Descriptor describing this configuration
    0x80, // D7 Reserved, set to 1. (USB 1.0 Bus Powered), D6 Self Powered, D5 Remote Wakeup, D4..0 Reserved, set to 0.
    50,   // Maximum Power Consumption in 2mA units

    // interface descriptor
    0x09, // Size of Descriptor in Bytes (9 Bytes)
    0x04, // Interface Descriptor (0x04)
    0x00, // Number of Interface
    0x00, // Value used to select alternative setting
    0x01, // Number of Endpoints used for this interface
    0x03, // Class Code
    0x01, // Subclass Code
    0x02, // Protocol Code
    0x00, // Index of String Descriptor Describing this interface

    // HID descriptor
    9,    // Size of Descriptor in Bytes (9 Bytes)
    0x21, // HID descriptor (0x21)
    0x10, 
    0x01, // BCD representation of HID version
    0x21, // Target country code
    0x01, // Number of HID Report (or other HID class) Descriptor infos to follow */
    0x22, // Descriptor type: report
    0x34,
    0,  /* total length of report descriptor */

    // Endpoint descriptor
    0x07, // Size of Descriptor in Bytes (7 Bytes)
    0x05, // Endpoint descriptor (0x05)
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
    0x04,
    0x03,
    0x09,
    0x04
};
const uint8_t usb_string_descriptor_vendor[] = {
    0x10,
    0x03,
    't',0,'u',0,'o',0,'q',0,'i',0,'a',0,'n',0,'g',0,
    '@',0,
    'o',0,'u',0,'t',0,'l',0,'o',0,'o',0,'k',0,
    '.',0,'c',0,'o',0,'m',0
};
const uint8_t usb_string_descriptor_device[0x10] = {
    0x10,
    0x03,
    'U',0,'S',0,'B',0,'-',0,'2',0,'3',0,'2',0
};
//const uint8_t usb_string_descriptor_device[] = {
//    0x04,
//    0x03,
//    'U',0,'S',0,'B',0,
//};
/***************************************************************************/
struct usb_type
{
    volatile uint8_t state;
    volatile uint8_t event;
    volatile uint8_t device_address;

    uint8_t endpoint_number;
    uint8_t setup_address;
    uint8_t setup_endpoint;

    uint8_t rx_buffer[16];
    uint8_t rx_lenght;

    uint8_t tx_buffer[16];
    uint8_t tx_lenght;
    uint8_t tx_is_all;
}usb;
/***************************************************************************/
void usb_init(void)
{
    usb.state = USB_STATE_IDLE;
    usb.event = USB_EVENT_NO;
    usb.device_address = 0;
    usb.setup_address  = 0;
    usb.tx_is_all = TRUE;
    usb_ready = 0;
    usb_ready_reg = 0;
    usb.endpoint_number=0;
    
    VUSB_GPIO_Init();
    VUSB_Clock_Init();
    VUSB_EXTI_Init();
}
/*************************************************************************/
void usb_send_nack(void)
{
    uint8_t data[2];
    data[0] = 0x80;
    data[1] = USB_PID_NACK;
    USB_TX_COUNT = 2;
    USB_TX_BUFFER_POINTER = data;
    USB_TX();        
}

void usb_send_ack(void)
{
    uint8_t data[2];
    data[0] = 0x80;
    data[1] = USB_PID_ACK;
    USB_TX_COUNT = 2;
    USB_TX_BUFFER_POINTER = data;       
    USB_TX();
}

void usb_send_answer(void)
{
    USB_TX_COUNT = usb.tx_lenght;
    USB_TX_BUFFER_POINTER = &usb.tx_buffer[0];
    USB_TX();
}
/*************************************************************************/
void usb_copy_rx_buffer(void)
{
    uint8_t index = 0;
    for (index = 0; index < 16; index++) usb.rx_buffer[index] = USB_RX_BUFFER[index];
}
/*************************************************************************/
void USB_RX_OK(void)
{
    switch (USB_RX_BUFFER[1])
    {
        case (USB_PID_SETUP):
            usb.state = USB_STATE_SETUP;
            usb.endpoint_number=USB_RX_BUFFER[2]>>7+(USB_RX_BUFFER[3]&0x07)<<1;
            break;
        case (USB_PID_OUT):
            usb.state = USB_STATE_OUT;
            usb.endpoint_number=USB_RX_BUFFER[2]>>7+(USB_RX_BUFFER[3]&0x07)<<1;
            break;
        case (USB_PID_IN):
            if((USB_RX_BUFFER[2]&0x7F)==usb.device_address) // It's our address
            {
                if(usb.setup_address!=0)	//需要找个更好的地方
                {
                    usb.device_address=usb.setup_address;
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
                    usb_send_nack();
                    usb.endpoint_number=USB_RX_BUFFER[2]>>7+(USB_RX_BUFFER[3]&0x07)<<1;
                }
                else{
                    //ooooooooooooo
                    nop();
                    nop();
                    usb_send_answer();
                    usb.state = USB_STATE_IN;
                    usb.event = USB_EVENT_WAIT_DATA_IN;
                    usb.endpoint_number=USB_RX_BUFFER[2]>>7+(USB_RX_BUFFER[3]&0x07)<<1;
                }
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
                
                usb_send_ack();
                
                if (USB_RX_BUFFER[2] == USB_REQUEST_TYPE_TO_DEVICE 
                    &&USB_RX_BUFFER[3] == USB_REQUEST_SET_ADDRESS)
                {
                    usb.setup_address = USB_RX_BUFFER[4];
                }	
                
                usb_copy_rx_buffer();
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
		
                usb_send_ack();
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
                
                usb_send_ack();
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
void usb_calc_crc16(uint8_t * buffer, uint8_t lenght)
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

void usb_send_data(const uint8_t * buffer, uint8_t lenght, uint8_t mode)
{
    uint8_t index;
    
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
        usb_calc_crc16(&usb.tx_buffer[2], (uint8_t) (usb.tx_lenght - 4));
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

void usb_send_null_data1(void)
{
    usb.tx_lenght = 4;
    usb.tx_buffer[0] = 0x80;
    usb.tx_buffer[1] = USB_PID_DATA1;
    usb.tx_buffer[2] = 0;
    usb.tx_buffer[3] = 0;
    usb.event = USB_EVENT_READY_DATA_IN;
}

void usb_send_stall(void)
{
    usb.tx_lenght = 2;
    usb.tx_buffer[0] = 0x80;
    usb.tx_buffer[1] = USB_PID_STALL;
    usb.event = USB_EVENT_READY_DATA_IN;
}

void usb_process(void)
{
    if (usb.event == USB_EVENT_RECEIVE_SETUP_DATA)
    {
        switch (usb.rx_buffer[2])
        {
            case (USB_REQUEST_TYPE_FROM_DEVICE): //0x80
            {
                switch (usb.rx_buffer[3])
                {
                    case (USB_REQUEST_GET_DESCRIPTOR)://0x06
                    {
                        switch (usb.rx_buffer[5])
                        {
                            case (1):	// device descriptor
                            {
                                usb_send_data(&usb_device_descriptor[0], ARRAY_LENGHT(usb_device_descriptor), 1);
                                break;
                            }
                            case (2):	// configuration descriptor
                            {
                                if(usb.rx_buffer[8]<ARRAY_LENGHT(usb_configuration_descriptor))
                                    usb_send_data(&usb_configuration_descriptor[0],usb.rx_buffer[8], 1);
                                else
                                    usb_send_data(&usb_configuration_descriptor[0], ARRAY_LENGHT(usb_configuration_descriptor), 1);
                                break;
                            }
                            case (3):	// string descriptor
                            {
                              
                                if(usb.rx_buffer[4]==0) usb_send_data(usb_string_descriptor_language, ARRAY_LENGHT(usb_string_descriptor_language),1);
                                else if(usb.rx_buffer[4]==1) usb_send_data(usb_string_descriptor_vendor, ARRAY_LENGHT(usb_string_descriptor_vendor),1);
                                else if(usb.rx_buffer[4]==2) {
                                      usb_send_data(usb_string_descriptor_device, ARRAY_LENGHT(usb_string_descriptor_device),1);
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
            case (USB_REQUEST_TYPE_TO_DEVICE):	//0x00
            {
                switch (usb.rx_buffer[3])
                {
                    case (USB_REQUEST_SET_ADDRESS):	//0x05
                    {
                        usb.setup_address = usb.rx_buffer[4];
                        usb_send_null_data1();
                        break;
                    }
                    case (USB_REQUEST_SET_CONFIGURATION):	//0x09
                    {
                        usb_send_null_data1();
                        break;
                    }
                }
                break;
            }
            case (0x81):
            {
                switch (usb.rx_buffer[3])
                {
                    case(USB_REQUEST_GET_DESCRIPTOR):
                    {
                        usb_send_data(&usb_report_descriptor[0], ARRAY_LENGHT(usb_report_descriptor), 1);
                        usb_ready_reg=1;
                        break;
                    }
                    default: break;
                }
                break;
            }
            case (0x21):
            {
                usb_send_stall();
                break;
            }
        }
    }
}

void usb_process_loop(void){
    while(usb_ready == 0){
        usb_process();
    }
}