//buzzer.c
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
    Buzzer_Handler.Mode = Buzzer_InitStruct->Mode;
    Buzzer_Handler.GPIOx = Buzzer_InitStruct->GPIOx;
    Buzzer_Handler.GPIO_Pin = Buzzer_InitStruct->GPIO_Pin;
    Buzzer_Handler.Frequency = Buzzer_InitStruct->Frequency;
    Buzzer_Handler.Volume = Buzzer_InitStruct->Volume;
    
    if(Buzzer_Handler.Mode == BUZZER_MODE_GPIO) {
        /* GPIO模式初始化 */
        GPIO_InitTypeDef GPIO_InitStructure;
        
        /* 使能GPIO时钟 */
        if(Buzzer_Handler.GPIOx == GPIOA) {
            RCC_APB0PeriphClockCmd(RCC_APB0Periph_GPIOA, ENABLE);
        } else if(Buzzer_Handler.GPIOx == GPIOB) {
            RCC_APB0PeriphClockCmd(RCC_APB0Periph_GPIOB, ENABLE);
        } else if(Buzzer_Handler.GPIOx == GPIOC) {
            RCC_APB0PeriphClockCmd(RCC_APB0Periph_GPIOC, ENABLE);
        }
        
        /* 配置GPIO为推挽输出 */
        GPIO_InitStructure.GPIO_Pin = Buzzer_Handler.GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT_PP;
        GPIO_InitStructure.GPIO_DriveLevel = GPIO_DriveLevel_0; // 最大驱动能力
        GPIO_Init(Buzzer_Handler.GPIOx, &GPIO_InitStructure);
        
        /* 初始状态关闭蜂鸣器 */
        GPIO_ResetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
        
    } else if(Buzzer_Handler.Mode == BUZZER_MODE_PWM) {
        /* PWM模式初始化 */
        Buzzer_Handler.TIMx = Buzzer_InitStruct->TIMx;
        Buzzer_Handler.PWMx = Buzzer_InitStruct->PWMx;
    }
}
/**
  * @brief  蜂鸣器反初始化
  * @retval None
  */
void Buzzer_DeInit(void)
{
    Buzzer_Stop();
    
    if(Buzzer_Handler.Mode == BUZZER_MODE_GPIO) {
        GPIO_DeInit(Buzzer_Handler.GPIOx);
    } else {
        // PWM模式的反初始化
        if(Buzzer_Handler.TIMx) {
            TIM_DeInit(Buzzer_Handler.TIMx);
        }
        if(Buzzer_Handler.PWMx) {
            PWM_DeInit(Buzzer_Handler.PWMx);
        }
    }
}

/**
  * @brief  蜂鸣器鸣叫（固定频率）
  * @param  duration_ms: 鸣叫持续时间（毫秒）
  * @retval None
  */
void Buzzer_Beep(uint32_t duration_ms)
{
    if(Buzzer_Handler.Mode == BUZZER_MODE_GPIO) {
        /* GPIO模式：简单开关 */
        GPIO_SetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
        
        // 需要实现延时函数
        // Delay_ms(duration_ms);
        
        GPIO_ResetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
    } else {
        /* PWM模式：使用配置的频率 */
        Buzzer_BeepWithFreq(Buzzer_Handler.Frequency, duration_ms);
    }
}

/**
  * @brief  蜂鸣器以指定频率鸣叫
  * @param  frequency: 频率（Hz）
  * @param  duration_ms: 鸣叫持续时间（毫秒）
  * @retval None
  */
void Buzzer_BeepWithFreq(uint32_t frequency, uint32_t duration_ms)
{
    if(Buzzer_Handler.Mode == BUZZER_MODE_PWM) {
        /* PWM模式实现 */
        Buzzer_SetFrequency(frequency);
        // 启动PWM输出
        // 延时 duration_ms
        // 停止PWM输出
    } else {
        /* GPIO模式：模拟PWM（不精确） */
        uint32_t period_us = 1000000 / frequency; // 周期（微秒）
        uint32_t half_period_us = period_us / 2;
        uint32_t cycles = (duration_ms * 1000) / period_us;
        
        for(uint32_t i = 0; i < cycles; i++) {
            GPIO_SetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
            // Delay_us(half_period_us);
            GPIO_ResetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
            // Delay_us(half_period_us);
        }
    }
}

/**
  * @brief  停止蜂鸣器
  * @retval None
  */
void Buzzer_Stop(void)
{
    if(Buzzer_Handler.Mode == BUZZER_MODE_GPIO) {
        GPIO_ResetBits(Buzzer_Handler.GPIOx, Buzzer_Handler.GPIO_Pin);
    } else {
        // PWM模式：停止PWM输出
        if(Buzzer_Handler.PWMx) {
            PWM_Cmd(Buzzer_Handler.PWMx, DISABLE);
        }
    }
}

/**
  * @brief  设置蜂鸣器音量（PWM占空比）
  * @param  volume: 音量（0-100）
  * @retval None
  */
void Buzzer_SetVolume(uint8_t volume)
{
    if(volume > 100) volume = 100;
    Buzzer_Handler.Volume = volume;
    
    if(Buzzer_Handler.Mode == BUZZER_MODE_PWM && Buzzer_Handler.PWMx) {
        // 设置PWM占空比
        // 需要根据具体PWM配置实现
    }
}

/**
  * @brief  设置蜂鸣器频率（PWM模式）
  * @param  frequency: 频率（Hz）
  * @retval None
  */
void Buzzer_SetFrequency(uint32_t frequency)
{
    Buzzer_Handler.Frequency = frequency;
    
    if(Buzzer_Handler.Mode == BUZZER_MODE_PWM && Buzzer_Handler.TIMx) {
        // 设置定时器频率
        // 需要根据具体TIM配置实现
    }
}