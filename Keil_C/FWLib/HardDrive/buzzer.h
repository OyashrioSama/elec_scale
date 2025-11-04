#ifndef __BUZZER_H
#define __BUZZER_H

#include "sc32f1xxx.h"
#include "sc32f1xxx_gpio.h"
#include "sc32f1xx_tim.h"
#include "sc32f1xxx_pwm.h"

/** @brief 蜂鸣器工作模式 */
typedef enum {
	BUZZER_MODE_GPIO = 0,
	BUZZER_MODE_PWM = 1
} Buzzer_ModeTypeDef;

/** @brief 蜂鸣器配置结构体 */
typedef struct {
    Buzzer_ModeTypeDef Mode;     // 工作模式
    GPIO_TypeDef* GPIOx;         // GPIO端口
    uint16_t GPIO_Pin;           // GPIO引脚
    TIM_TypeDef* TIMx;           // 定时器（PWM模式使用）
    PWM_TypeDef* PWMx;           // PWM（PWM模式使用）
    uint32_t Frequency;          // 频率（Hz，PWM模式使用）
    uint8_t Volume;              // 音量/占空比（0-100）
} Buzzer_InitTypeDef;

/* 蜂鸣器驱动函数 */
void Buzzer_Init(Buzzer_InitTypeDef* Buzzer_InitStruct);
void Buzzer_DeInit(void);
void Buzzer_Beep(uint32_t duration_ms);
void Buzzer_BeepWithFreq(uint32_t frequency, uint32_t duration_ms);
void Buzzer_Stop(void);
void Buzzer_SetVolume(uint8_t volume);
void Buzzer_SetFrequency(uint32_t frequency);

#endif /* __BUZZER_H */