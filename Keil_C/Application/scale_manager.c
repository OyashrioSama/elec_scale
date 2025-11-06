//scale_manager.c

#include "scale_manager.h"
#include "system_timer.h"
#include "weight_sensor.h"
#include "buzzer.h"
#include <math.h>

extern const float OVERWEIGHT_LIMIT;
extern const uint32_t OVERWEIGHT_CHECK_FREQ;
extern const float ACTIVITY_THRESHOLD;
extern const uint32_t INACTIVITY_TIMEOUT;

/* 全局状态变量定义 */
ScaleState_t Scale_State = {0};

/**
  * @brief  更新重量显示
  */
void UpdateWeightDisplay(void)
{
    if(Scale_State.isMeasuring) {
        Scale_State.currentWeight = WeightSensor_GetWeightGram();
    } else {
        Scale_State.currentWeight = 0.0f;
    }
}

/**
  * @brief  触发超重警报
  */
void TriggerOverweightAlarm(void)
{
    Buzzer_Start();  // 蜂鸣器持续鸣叫
}

/**
  * @brief  停止警报
  */
void StopAlarm(void)
{
    Buzzer_Stop();   // 停止蜂鸣器
}

/**
  * @brief  实时监控超重检查
  */
void OverweightCheck_RealTime(void)
{
    uint32_t currentTime = GetSystemTime();
    
    // 每500ms检查一次
    if(currentTime - Scale_State.lastOverweightCheck >= OVERWEIGHT_CHECK_FREQ) {
        if(Scale_State.overweightMode == OVERWEIGHT_REAL_TIME && 
           Scale_State.isMeasuring) {
            float weight = WeightSensor_GetWeightGram();
            if(weight > OVERWEIGHT_LIMIT) {
                TriggerOverweightAlarm();
            } else {
                StopAlarm();  // 重量恢复正常，停止警报
            }
        }
        Scale_State.lastOverweightCheck = currentTime;
    }
}

/**
  * @brief  按键时超重检查
  */
uint8_t OverweightCheck_OnKeyPress(void)
{
    float weight = WeightSensor_GetWeightGram();
    if(weight > OVERWEIGHT_LIMIT) {
        TriggerOverweightAlarm();
        return 1;  // 超重，不允许操作
    }
    StopAlarm();  // 重量正常，确保警报停止
    return 0;     // 正常，允许操作
}

/**
  * @brief  检测重量变化活动
  */
void CheckWeightActivity(void)
{
    static float lastStableWeight = 0.0f;
    float currentWeight = WeightSensor_GetWeightGram();
    
    // 重量变化超过5g，认为有用户操作
    if(fabs(currentWeight - lastStableWeight) > ACTIVITY_THRESHOLD) {
        ResetInactivityTimer();
        lastStableWeight = currentWeight;
    }
}

/**
  * @brief  检查60秒无操作
  */
void CheckInactivityTimeout(void)
{
    uint32_t currentTime = GetSystemTime();
    uint32_t inactiveTime = currentTime - Scale_State.lastActivityTime;
    
    if(inactiveTime > INACTIVITY_TIMEOUT) {
        Scale_State.screenState = SCREEN_STANDBY;  // 进入待机
    } else {
        Scale_State.screenState = SCREEN_ACTIVE;   // 保持活跃
    }
}

