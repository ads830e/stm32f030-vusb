/*
 * File: usb_rx.s
 * 2017.04.19
 * Created by Ads830e
 * Email:tuoqiang@outlook.com
*/
/**************************************************************************/
#include "usbconfig.h"
/**************************************************************************/    
    //import
    EXTERN USB_RX_COUNT,USB_RX_BUFFER
    EXTERN PORT_IDR
    
    EXTERN USB_RX_OK
    //export
    PUBLIC USB_RX
/**************************************************************************/
    ;MODULE  usb_tx
    ;Declaration of sections.
    SECTION .text:CODE:NOROOT:REORDER(2)
    THUMB
    CODE
//////////////////////////////////////////////////////////////////////////
USB_RX:
    push {r0-r7}
    mov r0,r8
    mov r1,r9
    mov r2,r10
    mov r3,r11
    push {r0-r3}
    mov r0,lr
    push {r0}
//////////////////////////////////////////////
    //r9����˿�ָ��
    ldr r0,=PORT_IDR
    ldr r0,[r0]
    mov r9,r0
    
    //r8����usb����,�����жϽ��ս���
    ldr r0,=VUSB_MASK
    mov r8,r0
    
//////////////////////////////////////////////
    //r7����״̬
    //��һ�ֽ�Ϊ0x80
    movs r7,#1h
    
    //bit stuffing��ʼΪ6
    movs r6,#6
    
    //r5����ָ��
    ldr r5,=USB_RX_BUFFER
    
    //r4�����ֽ���
    movs r4,#0h
    //��һ���ֽ�Ϊ0x80
    movs r0,#080h
    strb r0,[r5,r4]
//////////////////////////////////////////////
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
    movs r0,#0ah
RX_DELAY
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
    subs r0,#01h
    bne RX_DELAY
    
//////////////////////////////////////////////
    movs r2,#01h
    b LOOP0
//////////////////////////////////////////////
RX_END_C:
    b RX_END_1
//////////////////////////////////////////////
LOOP0:
///////////////////////////////////////////////
//Ƶ��У��
//���������ֻ��1��nop
//rc������׼�����Ժ��1~2��nop
    nop
    nop
    //nop
    //nop
    //nop
//////////////////////////////////////////////
    //ÿbyte��8bit
BIT0:
    //���r3,Ϊbit0����
    movs r3,#00h
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    bne BIT0_NEXT
    b RX_END
BIT0_NEXT:
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ��ȡ��
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT0_BITSTUFFING
    cmp r7,#00h
    bne BIT0_BITSTUFFING_NORESET
    movs r6,#06h
    mov r7,r0
    b BIT0_NEXT1
BIT0_BITSTUFFING_NORESET
    mov r7,r0
    b BIT0_NEXT1
BIT0_BITSTUFFING:
    bl BIT_BUFFING
BIT0_NEXT1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    ;nop
    ;nop
    cmp r4,#20h
    beq RX_END_C
    //��bit����
BIT1:
    //����
    rors r3,r2
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    bne BIT1_NEXT
    b RX_END
BIT1_NEXT:
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT1_BITSTUFFING
    cmp r7,#00h
    bne BIT1_BITSTUFFING_NORESET
    nop
    movs r6,#06h
    mov r7,r0
    b BIT1_NEXT1
BIT1_BITSTUFFING_NORESET
    mov r7,r0
    b BIT1_NEXT1
BIT1_BITSTUFFING:
    bl BIT_BUFFING
BIT1_NEXT1:
    nop
    nop
    nop
    nop
    nop
    ;nop
    adds r4,#01h
    //��bit����
BIT2:
    //����
    rors r3,r2
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    bne BIT2_NEXT
    b RX_END
BIT2_NEXT:
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT2_BITSTUFFING
    cmp r7,#00h
    bne BIT2_BITSTUFFING_NORESET
    nop
    movs r6,#06h
    mov r7,r0
    b BIT2_NEXT1
BIT2_BITSTUFFING_NORESET
    mov r7,r0
    b BIT2_NEXT1
BIT2_BITSTUFFING:
    bl BIT_BUFFING
BIT2_NEXT1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    //��bit����
BIT3:
    //����
    rors r3,r2
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    bne BIT3_NEXT
    b RX_END
BIT3_NEXT:
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT3_BITSTUFFING
    cmp r7,#00h
    bne BIT3_BITSTUFFING_NORESET
    nop
    movs r6,#06h
    mov r7,r0
    b BIT3_NEXT1
BIT3_BITSTUFFING_NORESET
    mov r7,r0
    b BIT3_NEXT1
BIT3_BITSTUFFING:
    bl BIT_BUFFING
BIT3_NEXT1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    //��bit����
BIT4:
    //����
    rors r3,r2
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    bne BIT4_NEXT
    b RX_END
BIT4_NEXT:
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT4_BITSTUFFING
    cmp r7,#00h
    bne BIT4_BITSTUFFING_NORESET
    nop
    movs r6,#06h
    mov r7,r0
    b BIT4_NEXT1
BIT4_BITSTUFFING_NORESET
    mov r7,r0
    b BIT4_NEXT1
BIT4_BITSTUFFING:
    bl BIT_BUFFING
BIT4_NEXT1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    //��bit����
BIT5:
    //����
    rors r3,r2
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    bne BIT5_NEXT
    b RX_END
BIT5_NEXT:
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT5_BITSTUFFING
    cmp r7,#00h
    bne BIT5_BITSTUFFING_NORESET
    nop
    movs r6,#06h
    mov r7,r0
    b BIT5_NEXT1
BIT5_BITSTUFFING_NORESET
    mov r7,r0
    b BIT5_NEXT1
BIT5_BITSTUFFING:
    bl BIT_BUFFING
BIT5_NEXT1:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    //��bit����
BIT6:
    //����
    rors r3,r2
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    bne BIT6_NEXT
    b RX_END
BIT6_NEXT:
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT6_BITSTUFFING
    cmp r7,#00h
    bne BIT6_BITSTUFFING_NORESET
    nop
    movs r6,#06h
    mov r7,r0
    b BIT6_NEXT1
BIT6_BITSTUFFING_NORESET
    mov r7,r0
    b BIT6_NEXT1
BIT6_BITSTUFFING:
    bl BIT_BUFFING
BIT6_NEXT1:
    nop
    nop
    nop
    nop
    nop
    nop
    ;nop
    //��bit����
    
BIT7:
    //����
    rors r3,r2
    //�õ�����,������bit0
    mov r0,r9
    ldrh r0,[r0]
    mov r1,r8
    ands r0,r1
    beq RX_END
#if VUSB_PLUS_BIT==0
    mov r0,r0
#else
    lsrs r0,r0,#VUSB_PLUS_BIT
#endif
    //ȥ������λ
    eors r7,r0
    ands r7,r2
    eors r7,r2
    //Ϊ1ʱ˵������Ϊ1��Ϊ0ʱ˵������Ϊ0
    add r3,r7
    subs r6,r6,r7
    beq BIT7_BITSTUFFING
    cmp r7,#00h
    bne BIT7_BITSTUFFING_NORESET
    nop
    movs r6,#06h
    mov r7,r0
    b BIT7_NEXT1
BIT7_BITSTUFFING_NORESET
    mov r7,r0
    b BIT7_NEXT1
BIT7_BITSTUFFING:
    bl BIT_BUFFING
BIT7_NEXT1:
    //��bit����
BYTE_NEXT:
    ;nop
    rors r3,r2
    nop
    lsrs r3,r3,#24
    strb r3,[r5,r4]
    b LOOP0
//////////////////////////////////////////////
RX_END:
    adds r4,#01h
RX_END_1:
    ldr r0,=USB_RX_COUNT
    strh r4,[r0]
///////////////////////////////////////////////
//    nop
//    nop
//    nop
//    nop
//    nop
//    nop
//    nop
//    nop
    bl USB_RX_OK
//////////////////////////////////////////////
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
BIT_BUFFING:
    movs r6,#06h
    eors r0,r2
    mov r7,r0
    //nop
    //nop
    
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
    
    bx lr
/////////////////////////////////////////////////////////////////////////////
    END
/***************************************************************************/