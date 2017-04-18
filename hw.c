/*
 * Created by Tuoqiang
 * Email:tuoqiang@outlook.com
*/
/*************************************************************************/
#include "hw.h"
#include "usbconfig.h"
/*************************************************************************/
uint16_t volatile *PORT_ODR,*PORT_IDR;
uint32_t volatile *PORT_MODER;
/*************************************************************************/
void VUSB_GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin=VUSB_PLUS;//usb d+
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin=VUSB_MINUS;//usb d-
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    
    GPIO_InitStructure.GPIO_Pin=VUSB_OUT_DBG;//usb d+ floating,in
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    
    PORT_ODR=&GPIOA->ODR;
    PORT_IDR=&GPIOA->IDR;
    PORT_MODER=&GPIOA->MODER;
}

void VUSB_Clock_Init(void){
    SystemInit();
}

void VUSB_EXTI_Init(void){
    //将EXTI0指向PA0  
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);  
    //EXTI0中断线配置  
    EXTI_InitTypeDef EXTI_InitStructure;  
    EXTI_InitStructure.EXTI_Line=EXTI_Line0;  
    EXTI_InitStructure.EXTI_Mode=EXTI_Mode_Interrupt;  
    EXTI_InitStructure.EXTI_Trigger=EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd=ENABLE;  
    EXTI_Init(&EXTI_InitStructure);  

    //EXTI0中断向量配置  
    NVIC_InitTypeDef NVIC_InitStructure;  
    NVIC_InitStructure.NVIC_IRQChannel=EXTI0_1_IRQn;  
    NVIC_InitStructure.NVIC_IRQChannelPriority=0x01;  
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;  
    NVIC_Init(&NVIC_InitStructure);
}

/*************************************************************************/