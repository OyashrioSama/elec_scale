#ifndef __KEY_H
#define __KEY_H

#include "sc32f1xxx.h"
#include "sc32f1xxx_gpio.h"
#include "sc32f1xxx_int.h"

/** @brief 按键状态 */
typedef enum {
    KEY_RELEASED = 0,  // 按键释放
    KEY_PRESSED = 1    // 按键按下
} Key_State;

/** @brief 按键配置结构体 */
typedef struct {
    GPIO_TypeDef* GPIOx;           // GPIO端口
    uint16_t GPIO_Pin;             // GPIO引脚
    INT_Channel_Typedef INT_Channel; // 外部中断通道
    INT_INTSEL_Typedef INT_Port;   // 外部中断端口
    Key_State ActiveLevel;         // 有效电平（按下时的电平）
} Key_InitTypeDef;

/** @brief 按键回调函数指针 */
typedef void (*Key_Callback)(void);

/* 按键驱动函数 */
void Key_Init(Key_InitTypeDef* Key_InitStruct);
Key_State Key_Read(void);
void Key_SetPressCallback(Key_Callback callback);
void Key_SetReleaseCallback(Key_Callback callback);

#endif /* __KEY_H */