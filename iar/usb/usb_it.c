/*
 * Created by Tuoqiang
 * Email:tuoqiang@outlook.com
*/
#include "usb.h"
extern void USB_Rx(void);

void EXTI0_1_IRQHandler(void){  
    //if(EXTI_GetITStatus(EXTI_Line0)!=RESET)  
    //{  
        //�жϴ���  
        USB_Rx();
    //}  
    //�˳��ж�ʱע�������־λ  
    EXTI_ClearFlag(EXTI_Line0);
}

