//buzzer.h
#ifndef __BUZZER_H
#define __BUZZER_H

#include "sc32f1xxx.h"
#include "sc32f1xxx_gpio.h"
#include "sc32f1xxx_pwm.h"

/** @brief 蜂鸣器配置结构体 */
typedef struct {
    PWM_TypeDef* PWMx;           // PWM模块（PWM0）
    PWM_Channel_Typedef Channel; // PWM通道
    uint32_t Frequency;          // 频率（Hz）
    uint8_t DutyCycle;           // 占空比（0-100）
} Buzzer_InitTypeDef;

void Buzzer_Init(Buzzer_InitTypeDef* Buzzer_InitStruct);  // 初始化
void Buzzer_Beep(uint32_t duration_ms);  // 延时鸣叫
void Buzzer_Start(void);  // 启动蜂鸣器（持续鸣叫）
void Buzzer_Stop(void);  // 停止鸣叫
void Buzzer_SetFrequency(uint32_t frequency);  // 设置频率
void Buzzer_SetDutyCycle(uint8_t duty_cycle);  // 设置占空比

#endif /* __BUZZER_PWM_H */

