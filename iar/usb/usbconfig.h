/*
 * File: usbconfig.h
 * 2017.04.19
 * Created by Ads830e
 * Email:tuoqiang@outlook.com
*/

#ifndef VUSB_CONFIG
#define VUSB_CONFIG

/*
D+ PA.0
D- PA.1
*/
#define VUSB_PLUS_BIT 0
#define VUSB_PLUS (1<<VUSB_PLUS_BIT)

#define VUSB_MINUS_BIT 1
#define VUSB_MINUS (1<<VUSB_MINUS_BIT)

#define VUSB_OUT_DBG_BIT 2
#define VUSB_OUT_DBG (1<<VUSB_OUT_DBG_BIT)

#define VUSB_MASK (VUSB_PLUS|VUSB_MINUS)

#endif

