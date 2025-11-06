// key.c

#include "key.h"

/* 多按键处理结构 */
static struct {
    Key_InitTypeDef config[KEY_ID_MAX];  // 4个按键的配置
    Key_Callback PressCallback;
    Key_Callback ReleaseCallback;
    uint8_t keyCount;                    // 已注册的按键数量
} Key_Handler = {0};

/**
  * @brief  按键初始化
  */
void Key_Init(Key_InitTypeDef* Key_InitStruct)
{
	if(Key_Handler.keyCount >= KEY_ID_MAX) return;
	
	uint8_t idx = Key_Handler.keyCount;
	
    /* 保存配置 */
    Key_Handler.config[idx].GPIOx = Key_InitStruct->GPIOx;
    Key_Handler.config[idx].GPIO_Pin = Key_InitStruct->GPIO_Pin;
    Key_Handler.config[idx].INT_Channel = Key_InitStruct->INT_Channel;
    Key_Handler.config[idx].INT_Port = Key_InitStruct->INT_Port;
    Key_Handler.config[idx].ActiveLevel = Key_InitStruct->ActiveLevel;
    Key_Handler.config[idx].KeyID = Key_InitStruct->KeyID;
    
    /* GPIO初始化 - 上拉输入模式 */
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = Key_Handler.config[idx].GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_PU;  // 上拉输入
    GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0;
    GPIO_Init(Key_Handler.config[idx].GPIOx, &GPIO_InitStructure);
    
    /* 外部中断初始化 */
    INT_InitTypeDef INT_InitStructure;
    INT_InitStructure.INT_Channel = Key_Handler.config[idx].INT_Channel;
    INT_InitStructure.INT_INTSEL = Key_Handler.config[idx].INT_Port;
    
    /* 根据有效电平配置触发方式 */
    if(Key_Handler.config[idx].ActiveLevel == KEY_PRESSED) {
        // 按下为低电平：下降沿触发
        INT_InitStructure.INT_Trigger = INT_Trigger_Falling;
    } else {
        // 按下为高电平：上升沿触发
        INT_InitStructure.INT_Trigger = INT_Trigger_Rising;
    }
    
    INT_Init(&INT_InitStructure);
    
    /* 使能外部中断 */
    INT_ITConfig(Key_Handler.config[idx].INT_Channel, 
                 (Key_Handler.config[idx].ActiveLevel == KEY_PRESSED) ? INT_IT_Falling : INT_IT_Rising, 
                 ENABLE);
    Key_Handler.keyCount++;
}

/**
  * @brief  读取按键当前状态
  */
Key_State Key_Read(Key_ID keyId)
{
	if(keyId >= Key_Handler.keyCount) return KEY_RELEASED;  // 边界检查
    BitAction pin_state = GPIO_ReadDataBit(Key_Handler.config[keyId].GPIOx, Key_Handler.config[keyId].GPIO_Pin);
    
    if((pin_state == SET && Key_Handler.config[keyId].ActiveLevel == KEY_PRESSED) ||
       (pin_state == RESET && Key_Handler.config[keyId].ActiveLevel == KEY_RELEASED)) {
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
    Key_Handler.PressCallback = callback;
}

/**
  * @brief  设置按键释放回调函数  
  */
void Key_SetReleaseCallback(Key_Callback callback)
{
    Key_Handler.ReleaseCallback = callback;
}

/**
  * @brief  外部中断处理函数（需要在SC_it.c中调用）
  */
void Key_IRQHandler(Key_ID keyId)
{
	if(keyId >= Key_Handler.keyCount) return;
	
    /* 清除中断标志 */
    INT_ClearFlag(Key_Handler.config[keyId].INT_Channel);
    
    /* 读取当前按键状态 */
    Key_State current_state = Key_Read(keyId);
    
    /* 调用对应的回调函数 */
    if(current_state == KEY_PRESSED && Key_Handler.PressCallback != 0) {
        Key_Handler.PressCallback(keyId);
    } else if(current_state == KEY_RELEASED && Key_Handler.ReleaseCallback != 0) {
        Key_Handler.ReleaseCallback(keyId);
    }
}
