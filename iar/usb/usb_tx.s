/*
 * File: usb_tx.s
 * 2017.04.19
 * Created by Ads830e
 * Email:tuoqiang@outlook.com
*/
/**************************************************************************/
#include "usbconfig.h"
/**************************************************************************/    
    //import
    EXTERN	USB_TX_COUNT,USB_TX_BUFFER_POINTER
    EXTERN PORT_ODR,PORT_MODER
    //export
    PUBLIC USB_TX
/**************************************************************************/
    ;MODULE  usb_tx
    ;Declaration of sections.
    SECTION .text:CODE:NOROOT:REORDER(2)
    THUMB
    CODE
//////////////////////////////////////////////////////////////////////////
USB_TX:
    push {r0-r7}
    mov r0,r8
    mov r1,r9
    mov r2,r10
    mov r3,r11
    push {r0-r3}
    mov r0,lr
    push {r0}
/////////////////////////////////////////////////////////////////////////

    //r9����˿�ָ��
    ldr r0,=PORT_ODR
    ldr r0,[r0]
    mov r9,r0
    
    ////USB_TX���������жϣ���GPIOA->ODR������λΪ����
    //���䱣����r7,�ҽ���ʼ״̬��Ϊ01
    ;r7=GPIOA->ODR
    ldr r7,=PORT_ODR
    ldr r7,[r7]
    ldrh r7,[r7]
    ldr r0,=(~VUSB_MASK)
    ands r7,r0
    ldr r0,=VUSB_MINUS
    orrs r7,r0
    
    /////////////////////////
    //���01
    ;GPIOA->ODR=r7
    mov r0,r9
    strh r7,[r0]
/////////////////////////////////////////////////////////////////////////
    //��������Ϊ���
    ldr r3,=PORT_MODER
    ldr r3,[r3]
    ldr r2,[r3]
    //d-
    movs r0,#01h
    lsls r0,r0,#VUSB_MINUS_BIT*2
    orrs r2,r0
    lsls r0,r0,#01h
    orrs r2,r0
    eors r2,r0
    //d+
    movs r0,#01h
    lsls r0,r0,#VUSB_PLUS_BIT*2
    orrs r2,r0
    lsls r0,r0,#01h
    orrs r2,r0
    eors r2,r0
    //��д
    str r2,[r3]
/////////////////////////////////////////////////////////////////////////
    //bit stuffing��ʼΪ6
    movs r6,#6
    
    //r5����ָ��
    ldr r5,=USB_TX_BUFFER_POINTER
    ldr r5,[r5]
//////////////////////////
    //r11�����ֽ���
    ldr r0,=USB_TX_COUNT
    ldrh r0,[r0]
    mov r11,r0
    
    //r10�����������λ��
    movs r0,#0h
    mov r10,r0
    
    //r8��������
    ldr r0,=VUSB_MASK
    ldr r1,=VUSB_OUT_DBG
    orrs r0,r1
    mov r8,r0

////////////////////////////////////////////////////////////////////////////    
    //����
    nop
    nop
    b TX_LOOP0
/////////////////////////////////////////////////////////////////////////////    
    //ѭ��
TX_LOOP0:
/////////////////////////////////////
//Ƶ��У��
//���������ֻ��1��nop
//rc������׼�����Ժ��1~2��nop
    nop
    nop
    //nop
//    nop
//    nop
//    nop
/////////////////////////////////////
    //ȡ���ݲ���ָ���1
    mov r0,r10
    ldrb r3,[r5,r0]
    cmp r0,r11
    bne TX_LOOP0_NEXT
    b TX_END
TX_LOOP0_NEXT:
    adds r0,#01h
    mov r10,r0
////////////////////////////////////
BIT0:
    lsrs r3,r3,#01h
    bcc BIT0_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT0_END
    bl TX_BIT_STUFFING
    b BIT0_END
BIT0_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT0_END
BIT0_END:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
////////////////////////////////////
BIT1:
    lsrs r3,r3,#01h
    bcc BIT1_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT1_END
    bl TX_BIT_STUFFING
    b BIT1_END
BIT1_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT1_END
BIT1_END:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
////////////////////////////////////
BIT2:
    lsrs r3,r3,#01h
    bcc BIT2_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT2_END
    bl TX_BIT_STUFFING
    b BIT2_END
BIT2_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT2_END
BIT2_END:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
////////////////////////////////////
BIT3:
    lsrs r3,r3,#01h
    bcc BIT3_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT3_END
    bl TX_BIT_STUFFING
    b BIT3_END
BIT3_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT3_END
BIT3_END:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
////////////////////////////////////
BIT4:
    lsrs r3,r3,#01h
    bcc BIT4_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT4_END
    bl TX_BIT_STUFFING
    b BIT4_END
BIT4_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT4_END
BIT4_END:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
////////////////////////////////////
BIT5:
    lsrs r3,r3,#01h
    bcc BIT5_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT5_END
    bl TX_BIT_STUFFING
    b BIT5_END
BIT5_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT5_END
BIT5_END:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
////////////////////////////////////
BIT6:
    lsrs r3,r3,#01h
    bcc BIT6_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT6_END
    bl TX_BIT_STUFFING
    b BIT6_END
BIT6_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT6_END
BIT6_END:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
////////////////////////////////////
BIT7:
    lsrs r3,r3,#01h
    bcc BIT7_Z
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;bit stuffing
    subs r6,#01h
    bne BIT7_END
    bl TX_BIT_STUFFING
    b BIT7_END
BIT7_Z:
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    b BIT7_END
BIT7_END:
    b TX_LOOP0
/////////////////////////////////////
TX_END:
    //????
    nop
    nop
    nop
    nop
    nop
    mov r0,r8
    orrs r7,r0
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    
    
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ldr r0,=VUSB_MINUS
    orrs r7,r0
    mov r0,r9
    strh r7,[r0]
/////////////////////////////////////////////////////////////////////////////   
    //��������Ϊ����
    ldr r3,=PORT_MODER
    ldr r3,[r3]
    ldr r2,[r3]
    //d-
    movs r0,#03h
    lsls r0,r0,#VUSB_MINUS_BIT*2
    orrs r2,r0
    eors r2,r0
    //d+
    movs r0,#03h
    lsls r0,r0,#VUSB_PLUS_BIT*2
    orrs r2,r0
    eors r2,r0
    //��д
    str r2,[r3]
/////////////////////////////////////////////////////////////////////////
    pop {r0}
    mov lr,r0
    pop {r0-r3}
    mov r8,r0
    mov r9,r1
    mov r10,r2
    mov r11,r3
    pop {r0-r7}
    bx lr
/////////////////////////////////////////////////////////////////////////////
TX_BIT_STUFFING:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    movs r6,#06h
    mov r0,r8
    eors r7,r0
    mov r0,r9
    strh r7,[r0]
    nop
    nop
    nop
    nop
    nop
    bx lr
/////////////////////////////////////////////////////////////////////////////
    END
/****************************************************************************/