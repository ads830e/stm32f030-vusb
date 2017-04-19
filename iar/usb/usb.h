/*
 * File: usb.h
 * 2017.04.19
 * Modified by Ads830e
 * Email:tuoqiang@outlook.com
*/
/*
 * File: usb.h
 * Date: 07.02.2014
 * Denis Zheleznyakov aka ZiB @ http://ziblog.ru
 */

#ifndef USB_H_
#define USB_H_
#include "hw.h"

void usb_init(void);
void usb_process_loop(void);
void usb_send_data(const uint8_t * buffer, uint8_t lenght, uint8_t mode);

#endif /* USB_H_ */
