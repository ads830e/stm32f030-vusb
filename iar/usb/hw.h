/*
 * Created by Tuoqiang
 * Email:tuoqiang@outlook.com
*/
#ifndef HW_H
#define HW_H

#include "stm32f0xx.h"

#ifndef nop
#define nop() __asm("nop");
#endif

void VUSB_GPIO_Init(void);
void VUSB_Clock_Init(void);
void VUSB_EXTI_Init(void);
#endif