# stm32f030-vusb
Use gpio to emulate usb low-speed on stm32f030f4p6.Use internal rc instead of crystal.


  我注意到zib、wlianmin、rikka0w0实现和改进了stm8的usb。考虑到stm32f030与stm8s103定位相似，且速度更快，于是重写了usb_rx.s和usb_tx.s，再对usb.c进行了修改,添加若干nop指令以适应时序，且利用内部rc代替外部晶振，结果能够被识别为鼠标，且工作正常。只不过自己对usb上层协议不熟悉，还不能添加新功能,如字符串描述符、CDC。在此上传代码，希望大家能够改进。

简述：
PA0:D+(EXTI)
PA1:D-

PA2:与PA1输出相同，用于调试，在逻辑分析器中用于识别设备发出的包.可移除
PA6:PWM，用逻辑分析器查看其周期，用于校准rc,在usb_rx()和usb_tx()中添加nop；或与D+连接，用于校准usb_rx().可移除

如有疑问及改进方案，可联系tuoqiang@outlook.com

  I noticed zib,wlianmin,rikka0w0 fullfilled and improved VUSB on stm8s103.Considering that stm32f030f4p6 is similar to and faster than stm8s103f3p6,I rewrite usb_rx.s and usb_tx.s in cortex-m0 asm.Then I modefy the usb.c by adding several "nop" to adjust time and use internal rc instead of crystal.As a result,the devices can be recognized as a mouse and works perfectly.But I don't konw much about usb and can't add other functions,such as string descriper,CDC,etc.I hope everyon can do a little for it.

IMPORTANT:
PA0:D+(EXTI)
PA1:D-

PA2:The ODR of it is same to PA1's.Used to debug.In logic analyzer it can be used to distinguish the packet send by device.Can be removed.
PA6:PWM.You can measure the period of it in logic analyzer to adjust internal rc by adding nop in usb_rx() and usb_tx();Connect it to usb d+ to adjust usb_rx().Can be removed.

If you have problems and improvements,please contact tuoqiang@outlook.com

