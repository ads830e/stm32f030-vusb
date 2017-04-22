/*
 * Modified by Tuoqiang
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

void USB_Init(void);
void USB_Process_LOOP(void);
void USB_Send_Data(const uint8_t * buffer, uint8_t lenght, uint8_t mode);

#endif /* USB_H_ */
