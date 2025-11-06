//app_init.c

#include "app_init.h"
#include "scale_manager.h"
#include "key_handler.h"

static WeightSensor_InitTypeDef WeightSensor_InitStruct;
static Buzzer_InitTypeDef Buzzer_InitStruct;
static Key_InitTypeDef Key_InitStruct;

/**
  * @brief  应用层初始化
  */
void Application_Init(void)
{
	/* 系统定时器初始化*/
    SystemTimer_Init();
    
    /* 重量传感器初始化 */
    WeightSensor_InitStruct.OPx = OP;
    WeightSensor_InitStruct.ADCx = ADC;
    WeightSensor_InitStruct.ADC_Channel = ADC_Channel_OP;
    WeightSensor_InitStruct.OP_Gain = OP_PGAGain_NonInvert16_Invert15;
    WeightSensor_InitStruct.AutoCalib = ENABLE;
    WeightSensor_Init(&WeightSensor_InitStruct);
    
    /* 蜂鸣器初始化 */
    Buzzer_InitStruct.PWMx = PWM0;
    Buzzer_InitStruct.Channel = PWM_Channel_0;
    Buzzer_InitStruct.Frequency = 4000;
    Buzzer_InitStruct.DutyCycle = 50;
    Buzzer_Init(&Buzzer_InitStruct);
    
    /* 4个按键初始化 */
    Key_InitStruct.GPIOx = GPIOA;
    Key_InitStruct.INT_Port = INT_INTSEL_PA;
    Key_InitStruct.ActiveLevel = KEY_PRESSED;
    
    // 按键1: 去皮
    Key_InitStruct.GPIO_Pin = GPIO_Pin_0;
    Key_InitStruct.INT_Channel = INT_Channel_0;
    Key_InitStruct.KeyID = KEY_ID_TARE;
    Key_Init(&Key_InitStruct);
    
    // 按键2: 校准
    Key_InitStruct.GPIO_Pin = GPIO_Pin_1;
    Key_InitStruct.INT_Channel = INT_Channel_1;
    Key_InitStruct.KeyID = KEY_ID_CALIB;
    Key_Init(&Key_InitStruct);
    
    // 按键3: 测量启动/停止
    Key_InitStruct.GPIO_Pin = GPIO_Pin_2;
    Key_InitStruct.INT_Channel = INT_Channel_2;
    Key_InitStruct.KeyID = KEY_ID_MEASURE;
    Key_Init(&Key_InitStruct);
    
    // 按键4: 电源
    Key_InitStruct.GPIO_Pin = GPIO_Pin_3;
    Key_InitStruct.INT_Channel = INT_Channel_3;
    Key_InitStruct.KeyID = KEY_ID_POWER;
    Key_Init(&Key_InitStruct);
    
    /* 设置回调函数 */
    Key_SetPressCallback(Key_Pressed_Handler);
    
    /* 初始化完成提示 */
    Buzzer_Beep(100);
}
