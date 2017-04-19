/*
 * File: usb_it.c
 * 2017.04.19
 * Created by Ads830e
 * Email:tuoqiang@outlook.com
*/
#include "usb.h"
extern void USB_RX(void);

void EXTI0_1_IRQHandler(void){  
    //if(EXTI_GetITStatus(EXTI_Line0)!=RESET)  
    //{  
        //中断处理  
        USB_RX();
    //}  
    //退出中断时注意清除标志位  
    EXTI_ClearFlag(EXTI_Line0);
}

