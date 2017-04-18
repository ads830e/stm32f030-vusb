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
    //�˺�����ʹmcu���250khz��ռ�ձ�Ϊ0.5�ķ���
    //����߼�������������У��Ƶ��
    //mcu������48Mhz
    //�˳���ʹ�����ڲ�rc
    //�ڲ�rc���羧���ȶ�����̫���ͻ����ƫ��
    //������ʹ�õ�mcu�ϣ�ʹ��saleae������ƽ��6.5x2usƫ��0.04us,����֮��ƫ��Ϊ0.04/13��ԼΪ1/320
    //��USB_RX��USB_TX�У����ÿ�ֽڲ���һ��nop,�������ı���Ϊ1/(8*0.6666666*48)=1/256
    //��������󾫶ȿ��Դﵽ1/512
    //�����ڴˣ�1/320-1/256=1/1280
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
      
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;                          //�������õ��������ΪPA6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_1);       //F0����Ҫ����AFΪ����1
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ�ܶ�ʱ��3ʱ��
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period        = 0xbf;// �����Զ���װ����ֵ
    TIM_TimeBaseStructure.TIM_Prescaler     = TIM_ICPSC_DIV1;//����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//����ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Down;//���ϼ���
    TIM_TimeBaseStructure.TIM_RepetitionCounter=0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);    //��ʼ����ʱ��3
    TIM_ARRPreloadConfig(TIM3,ENABLE);
    
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;// PWM2ģʽ
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;//�����
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


