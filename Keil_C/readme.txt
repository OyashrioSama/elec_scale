文件结构
+Application 应用层
+--app_init.c/h 硬件抽象初始化
+--key_handler.c/h 按键业务逻辑
+--scale_manager.c/h 称重业务逻辑
+--system_timer.c/h 系统定时器管理

+-SC32F1XXX_Lib 固件库
+--inc
+--src

+-HardDrive 硬件驱动
+--buzzer.c/h 蜂鸣器驱动
+--key.c/h 按键驱动
+--weight_sensor.c/h 重量传感器驱动

+-User
+--main.c
+--SC_Init.c
+--SC_it.c

函数说明
buzzer.c
Buzzer_Init蜂鸣器初始化
Buzzer_Beep延时鸣叫
Buzzer_Start开始鸣叫
Buzzer_Stop停止鸣叫
Buzzer_SetFrequency设置频率
Buzzer_SetDutyCycle设置占空比


key.c
Key_Init按键初始化
Key_Read读取按键状态
Key_SetPressCallback设置按键按下回调函数
Key_SetReleaseCallback设置按键释放回调函数
Key_IROHandler处理中断函数