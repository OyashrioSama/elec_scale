#include "buzzer.h"

static Buzzer_InitTypeDef Buzzer_Handler;

/**
  * @brief  蜂鸣器初始化
  * @param  Buzzer_InitStruct: 蜂鸣器配置结构体
  * @retval None
  */
void Buzzer_Init(Buzzer_InitTypeDef* Buzzer_InitStruct)
{
    /* 保存配置 */
    Buzzer_Handler.GPIOx = Buzzer_InitStruct->GPIOx;
    Buzzer_Handler.GPIO_Pin = Buzzer_InitStruct->GPIO_Pin;
    
    /* GPIO初始化 */
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 配置GPIO为推挽输出 */
    GPIO_InitStructure.GPIO_Pin = Buzzer_Handler.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
    GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0;
    GPIO_Init(Buzzer_Handler.GPIOx, &GPIO_InitStructure);
    
    /* 初始状态关闭蜂鸣器 */
    GPIO_ResetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
}

/**
  * @brief  蜂鸣器鸣叫
  * @param  duration_ms: 鸣叫持续时间（毫秒）
  * @retval None
  */
void Buzzer_Beep(uint32_t duration_ms)
{
    /* 开启蜂鸣器 */
    GPIO_SetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
    
    /* 等待指定时间 */
    // 需要实现延时函数
    // Delay_ms(duration_ms);
    
    /* 关闭蜂鸣器 */
    GPIO_ResetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
}

/**
  * @brief  立即停止蜂鸣器
  * @retval None
  */
void Buzzer_Stop(void)
{
    GPIO_ResetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
}
