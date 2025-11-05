//buzzer.c
#include "buzzer.h"

static Buzzer_InitTypeDef Buzzer_Handler;

/**
  * @brief  蜂鸣器初始化
  */
void Buzzer_Init(Buzzer_InitTypeDef* Buzzer_InitStruct)
{
    /* 保存配置 */
    Buzzer_Handler.PWMx = Buzzer_InitStruct->PWMx;
    Buzzer_Handler.Channel = Buzzer_InitStruct->Channel;
    Buzzer_Handler.Frequency = Buzzer_InitStruct->Frequency;
    Buzzer_Handler.DutyCycle = Buzzer_InitStruct->DutyCycle;
    
    /* 配置PWM */
    PWM_InitTypeDef PWM_InitStructure;
    
    /* PWM结构体初始化 */
    PWM_StructInit(&PWM_InitStructure);
    
    /* 配置PWM参数 */
    PWM_InitStructure.PWM_Prescaler = PWM_PRESCALER_DIV1;     // 预分频
    PWM_InitStructure.PWM_AlignedMode = PWM_AlignmentMode_Edge; // 边沿对齐模式
    PWM_InitStructure.PWM_WorkMode = PWM_WorkMode_Independent;  // 独立模式
    
    /* 计算周期值 */
    // 假设系统时钟为16MHz，周期 = 时钟频率 / PWM频率
    uint32_t system_clock = 16000000; // 需要根据实际系统时钟调整
    PWM_InitStructure.PWM_Cycle = system_clock / Buzzer_Handler.Frequency;
    
    /* 配置输出通道 */
    PWM_InitStructure.PWM_OutputChannel = Buzzer_Handler.Channel;
    PWM_InitStructure.PWM_LowPolarityChannl = PWMChannel_Less; // 正常极性
    
    /* 初始化PWM */
    PWM_Init(Buzzer_Handler.PWMx, &PWM_InitStructure);
    
    /* 设置占空比 */
    uint16_t duty_value = (PWM_InitStructure.PWM_Cycle * Buzzer_Handler.DutyCycle) / 100;
    PWM_SetDuty(Buzzer_Handler.PWMx, Buzzer_Handler.Channel, duty_value);
    
    /* 初始状态关闭PWM */
    PWM_Cmd(Buzzer_Handler.PWMx, DISABLE);
}

/**
  * @brief  蜂鸣器延时鸣叫
  */
void Buzzer_Beep(uint32_t duration_ms)
{
    /* 开启PWM输出,开始鸣叫 */
    PWM_Cmd(Buzzer_Handler.PWMx, ENABLE);
    
    /* 等待指定时间 */
    // 需要实现延时函数
    // Delay_ms(duration_ms);
    
    /* 关闭PWM输出，停止鸣叫 */
    PWM_Cmd(Buzzer_Handler.PWMx, DISABLE);
}

/**
  * @brief  启动蜂鸣器（持续鸣叫，需要手动停止）
  */
void Buzzer_Start(void)
{
    /* 开启PWM输出 */
    PWM_Cmd(Buzzer_Handler.PWMx, ENABLE);
}

/**
  * @brief  停止蜂鸣器
  */
void Buzzer_Stop(void)
{
    PWM_Cmd(Buzzer_Handler.PWMx, DISABLE);
}

/**
  * @brief  设置蜂鸣器频率
  */
void Buzzer_SetFrequency(uint32_t frequency)
{
    Buzzer_Handler.Frequency = frequency;
    
    /* 重新计算并设置周期 */
    uint32_t system_clock = 16000000; // 需要根据实际系统时钟调整
    uint16_t cycle_value = system_clock / frequency;
    PWM_SetCycle(Buzzer_Handler.PWMx, cycle_value);
    
    /* 更新占空比以保持比例 */
    uint16_t duty_value = (cycle_value * Buzzer_Handler.DutyCycle) / 100;
    PWM_SetDuty(Buzzer_Handler.PWMx, Buzzer_Handler.Channel, duty_value);
}

/**
  * @brief  设置占空比
  */
void Buzzer_SetDutyCycle(uint8_t duty_cycle)
{
    if(duty_cycle > 100) duty_cycle = 100;
    Buzzer_Handler.DutyCycle = duty_cycle;
    
    /* 获取当前周期值 */
    uint16_t cycle_value = PWM_GetCycle(Buzzer_Handler.PWMx);
    
    /* 计算并设置新的占空比值 */
    uint16_t duty_value = (cycle_value * duty_cycle) / 100;
    PWM_SetDuty(Buzzer_Handler.PWMx, Buzzer_Handler.Channel, duty_value);
}
