/*
 * Created by Tuoqiang
 * Email:tuoqiang@outlook.com
*/
#include "usb.h"
extern void USB_Rx(void);

void EXTI0_1_IRQHandler(void){  
    //if(EXTI_GetITStatus(EXTI_Line0)!=RESET)  
    //{  
        //中断处理  
        USB_Rx();
    //}  
    //退出中断时注意清除标志位  
    EXTI_ClearFlag(EXTI_Line0);
}

