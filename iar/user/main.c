/*
 * File: main.c
 * Modified by Tuoqiang
 * Email:tuoqiang@outlook.com
 */
/*
 * File: main.c
 * Date: 02.01.2017
 * Denis Zheleznyakov aka ZiB @ http://ziblog.ru
*/

#include "project.h"
#include "usb.h"



uint8_t data_buffer[4];
void delay(uint16_t n)
{
	uint16_t i;
        while(n--)
            for(i=0;i<4177;i++);
}

void TIM3CH1_Init(void){
    //此函数会使mcu输出250khz的占空比为0.5的方波
    //配合逻辑分析器可用于校正频率
    //mcu工作于48Mhz
    //此程序使用了内部rc
    //内部rc不如晶振稳定，包太长就会出现偏差
    //在我所使用的mcu上，使用saleae测量，平均6.5x2us偏离0.04us,换言之，偏差为0.04/13，约为1/320
    //在USB_RX和USB_TX中，如果每字节插入一个nop,则修正的比例为1/(8*0.6666666*48)=1/256
    //因而修正后精度可以达到1/512
    //例如在此，1/320-1/256=1/1280
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;                          //这里设置的输出引脚为PA6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);       //F0还需要设置AF为功能1
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能定时器3时钟
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period        = 0xbf;// 设置自动重装周期值
    TIM_TimeBaseStructure.TIM_Prescaler     = TIM_ICPSC_DIV1;//设置预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Down;//向上计数
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);    //初始化定时器3
    TIM_ARRPreloadConfig(TIM3,ENABLE);
    
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;// PWM2模式
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;//输出高
    TIM_OCInitStructure.TIM_Pulse=0x5f;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    
    TIM_CtrlPWMOutputs(TIM3, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
//      TIM_SetCompare1(TIM3,50);
}

void main(void)
{
    char x=10;
    char y=10;
    
    asm("cpsid i");
    //TIM3CH1_Init();
    usb_init();
    asm("cpsie i");
    
    usb_process_loop();
    
    while(1)
    {
        delay(50);
        
        x=-x;
        y=-y;
        
        data_buffer[0] = 0x00;
        data_buffer[1] = x;
        data_buffer[2] = y;
        data_buffer[3] = 0x00;
        usb_send_data(&data_buffer[0], 4, 0);
    }
}


