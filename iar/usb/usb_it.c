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
        //�жϴ���  
        USB_RX();
    //}  
    //�˳��ж�ʱע�������־λ  
    EXTI_ClearFlag(EXTI_Line0);
}

