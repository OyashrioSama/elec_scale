#ifndef __BUZZER_H
#define __BUZZER_H

#include "sc32f1xxx.h"
#include "sc32f1xxx_gpio.h"

/** @brief 蜂鸣器配置结构体 */
typedef struct {
    GPIO_TypeDef* GPIOx;         // GPIO端口
    uint16_t GPIO_Pin;           // GPIO引脚
} Buzzer_InitTypeDef;

/* 蜂鸣器驱动函数 */
void Buzzer_Init(Buzzer_InitTypeDef* Buzzer_InitStruct);
void Buzzer_Beep(uint32_t duration_ms);
void Buzzer_Stop(void);

#endif /* __BUZZER_H */
