//scale_manager.h

#ifndef __SCALE_MANAGER_H
#define __SCALE_MANAGER_H

#include "sc32f1xxx.h"

/* 超重检查模式 */
typedef enum {
    OVERWEIGHT_REAL_TIME = 0,   // 实时监控（默认）
    OVERWEIGHT_KEY_CHECK        // 按键时检查
} OverweightMode;

/* 屏幕状态 */
typedef enum {
    SCREEN_ACTIVE = 0,    // 显示重量等信息
    SCREEN_STANDBY        // 显示空内容或简单待机界面
} ScreenState;

/* 电子秤状态结构 */
typedef struct {
    uint8_t isMeasuring;          // 测量状态标志
    float currentWeight;          // 当前重量
    ScreenState screenState;      // 屏幕状态
    OverweightMode overweightMode;// 超重检查模式
    uint32_t lastActivityTime;    // 最后活动时间
    uint32_t lastOverweightCheck; // 最后超重检查时间
} ScaleState_t;

/* 全局状态变量声明 */
extern ScaleState_t Scale_State;

/* 函数声明 */
void UpdateWeightDisplay(void);

/* 超重检查相关函数 */
void TriggerOverweightAlarm(void);
void StopAlarm(void);
void OverweightCheck_RealTime(void);
uint8_t OverweightCheck_OnKeyPress(void);

/* 活动检测和无操作检查函数 */
void CheckWeightActivity(void);
void CheckInactivityTimeout(void);

#endif /* __SCALE_MANAGER_H */
