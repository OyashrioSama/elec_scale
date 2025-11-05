#include "key.h"

static Key_InitTypeDef Key_Handler;
static Key_Callback PressCallback = 0;  // 使用0代替NULL
static Key_Callback ReleaseCallback = 0;

/**
  * @brief  按键初始化
  */
void Key_Init(Key_InitTypeDef* Key_InitStruct)
{
    /* 保存配置 */
    Key_Handler.GPIOx = Key_InitStruct->GPIOx;
    Key_Handler.GPIO_Pin = Key_InitStruct->GPIO_Pin;
    Key_Handler.INT_Channel = Key_InitStruct->INT_Channel;
    Key_Handler.INT_Port = Key_InitStruct->INT_Port;
    Key_Handler.ActiveLevel = Key_InitStruct->ActiveLevel;
    
    /* GPIO初始化 - 上拉输入模式 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = Key_Handler.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;  // 上拉输入
    GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0;
    GPIO_Init(Key_Handler.GPIOx, &GPIO_InitStructure);
    
    /* 外部中断初始化 */
    INT_InitTypeDef INT_InitStructure;
    INT_InitStructure.INT_Channel = Key_Handler.INT_Channel;
    INT_InitStructure.INT_INTSEL = Key_Handler.INT_Port;
    
    /* 根据有效电平配置触发方式 */
    if(Key_Handler.ActiveLevel == KEY_PRESSED) {
        // 按下为低电平：下降沿触发
        INT_InitStructure.INT_Trigger = INT_Trigger_Falling;
    } else {
        // 按下为高电平：上升沿触发
        INT_InitStructure.INT_Trigger = INT_Trigger_Rising;
    }
    
    INT_Init(&INT_InitStructure);
    
    /* 使能外部中断 */
    INT_ITConfig(Key_Handler.INT_Channel, 
                 (Key_Handler.ActiveLevel == KEY_PRESSED) ? INT_IT_Falling : INT_IT_Rising, 
                 ENABLE);
}

/**
  * @brief  读取按键当前状态
  */
Key_State Key_Read(void)
{
    BitAction pin_state = GPIO_ReadDataBit(Key_Handler.GPIOx, Key_Handler.GPIO_Pin);
    
    if((pin_state == SET && Key_Handler.ActiveLevel == KEY_PRESSED) ||
       (pin_state == RESET && Key_Handler.ActiveLevel == KEY_RELEASED)) {
        return KEY_RELEASED;
    } else {
        return KEY_PRESSED;
    }
}

/**
  * @brief  设置按键按下回调函数
  */
void Key_SetPressCallback(Key_Callback callback)
{
    PressCallback = callback;
}

/**
  * @brief  设置按键释放回调函数  
  */
void Key_SetReleaseCallback(Key_Callback callback)
{
    ReleaseCallback = callback;
}

/**
  * @brief  外部中断处理函数（需要在SC_it.c中调用）
  */
void Key_IRQHandler(void)
{
    /* 清除中断标志 */
    INT_ClearFlag(Key_Handler.INT_Channel);
    
    /* 读取当前按键状态 */
    Key_State current_state = Key_Read();
    
    /* 调用对应的回调函数 */
    if(current_state == KEY_PRESSED && PressCallback != 0) {
        PressCallback();
    } else if(current_state == KEY_RELEASED && ReleaseCallback != 0) {
        ReleaseCallback();
    }
}